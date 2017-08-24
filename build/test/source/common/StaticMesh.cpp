#include "StaticMesh.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"

/***********************/
/* public - StaticMesh */
/***********************/

StaticMesh::StaticMesh() :
	m_pPipelineLayout(nullptr),
	m_pBuffer(nullptr),
	m_VertexOffset(0),
	m_VerticesSize(0),
	m_IndexOffset(0),
	m_IndicesSize(0),
	m_IndexType(V3D_INDEX_TYPE_UINT16),
	m_pUniformBuffer(nullptr),
	m_pDescriptorSet(nullptr)
{
}

StaticMesh::~StaticMesh()
{
	Dispose();
}

const Matrix4x4& StaticMesh::GetWorldMatrix() const
{
	return m_Uniform.worldMat;
}

void StaticMesh::SetWorldMatrix(const Matrix4x4& worldMatrix)
{
	m_Uniform.worldMat = worldMatrix;
}

void StaticMesh::BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindVertexBuffer(0, m_pBuffer, m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);
}

void StaticMesh::BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer)
{
	uint32_t dynamicOffset = m_UniformDynamicOffsetHandle.GetOffset();

	pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, m_pDescriptorSet, 1, &dynamicOffset);
}

uint32_t StaticMesh::GetSubsetCount() const
{
	return static_cast<uint32_t>(m_Subsets.size());
}

const MeshSubset& StaticMesh::GetSubset(uint32_t subset) const
{
	return m_Subsets[subset];
}

void StaticMesh::Draw(IV3DCommandBuffer* pCommandBuffer)
{
	uint32_t dynamicOffset = m_UniformDynamicOffsetHandle.GetOffset();

	pCommandBuffer->BindVertexBuffer(0, m_pBuffer, m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);
	pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, m_pDescriptorSet, 1, &dynamicOffset);

	const MeshSubset* pSubset = m_Subsets.data();
	const MeshSubset* pSubsetEnd = pSubset + m_Subsets.size();

	uint64_t materialKey = 0;

	while (pSubset != pSubsetEnd)
	{\
		MaterialPtr material = GetMaterial(pSubset->material);
		uint64_t nextMaterialKey = material->GetKey();

		if (nextMaterialKey != materialKey)
		{
			material->Bind(pCommandBuffer, RENDER_TYPE_GEOMETRY);
			materialKey = nextMaterialKey;
		}

		pCommandBuffer->DrawIndexed(pSubset->indexCount, 1, pSubset->firstIndex, 0, 0);

		pSubset++;
	}
}

std::shared_ptr<StaticMesh> StaticMesh::Clone()
{
	StaticMeshPtr mesh = std::make_shared<StaticMesh>();

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, m_pMeshManager);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	mesh->m_Key = m_Key;

	mesh->m_LocalCenter = m_LocalCenter;
	mesh->m_HalfExtent = m_HalfExtent;
	mesh->m_Bounds = m_Bounds;

	SAFE_ADD_REF(m_pBuffer);
	mesh->m_pBuffer = m_pBuffer;
	mesh->m_VertexOffset = m_VertexOffset;
	mesh->m_IndexOffset = m_IndexOffset;
	mesh->m_IndexType = m_IndexType;

	mesh->m_Subsets.assign(m_Subsets.begin(), m_Subsets.end());

	Mesh::CloneMaterials(mesh.get());

	m_pMeshManager->Add(m_Name.c_str(), mesh);

	return std::move(mesh);
}

/**************************/
/* public override - Mesh */
/**************************/

MESH_TYPE StaticMesh::GetType() const
{
	return MESH_TYPE_STATIC;
}

IV3DBuffer* StaticMesh::GetVertexIndexBuffer()
{
	return m_pBuffer;
}

uint64_t StaticMesh::GetVertexOffset() const
{
	return m_VertexOffset;
}

uint64_t StaticMesh::GetIndexOffset() const
{
	return m_IndexOffset;
}

V3D_INDEX_TYPE StaticMesh::GetIndexType() const
{
	return m_IndexType;
}

V3D_RESULT StaticMesh::Update()
{
	void* pMemory;

	V3D_RESULT result = m_pUniformBuffer->Map(m_UniformDynamicOffsetHandle.GetOffset(), sizeof(StaticMesh::Uniform), &pMemory);
	if (result != V3D_OK)
	{
		return result;
	}

	memcpy_s(pMemory, sizeof(StaticMesh::Uniform), &m_Uniform, sizeof(StaticMesh::Uniform));

	result = m_pUniformBuffer->Unmap();
	if (result != V3D_OK)
	{
		return result;
	}

	Vector3 center = m_Uniform.worldMat * m_LocalCenter;
	Vector3 halfExtent = m_HalfExtent;

	m_Uniform.worldMat.TransformSR(1, &halfExtent);

	m_Bounds.aabb.minimum = center - halfExtent;
	m_Bounds.aabb.maximum = center + halfExtent;
	m_Bounds.aabb.UpdateCenterAndPoints();

	m_Bounds.sphere.center = center;
	m_Bounds.sphere.radius = halfExtent.GetLength();

	return V3D_OK;
}

bool StaticMesh::Save(const wchar_t* pFilePath)
{
	// ----------------------------------------------------------------------------------------------------
	// マテリアルリストを作成
	// ----------------------------------------------------------------------------------------------------

	std::vector<Material::SerialData> materials;

	if (Mesh::SerializeMaterials(materials) == false)
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスをホストメモリにロード
	// ----------------------------------------------------------------------------------------------------

	IV3DBuffer* pHostVertexIndexBuffer;

	if (Mesh::DownloadVertexIndexBuffer(m_pBuffer, &pHostVertexIndexBuffer) == false)
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// 保存
	// ----------------------------------------------------------------------------------------------------

	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		return false;
	}

	DWORD writeSize;

	// ファイルヘッダ
	StaticMesh::FileHeader fileHeader{};
	fileHeader.magicNumber = StaticMesh::MagicNumber;
	fileHeader.version = StaticMesh::Version;

	if (WriteFile(fileHandle, &fileHeader, TO_UI32(sizeof(StaticMesh::FileHeader)), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// インフォヘッダ
	StaticMesh::InfoHeader infoHeader{};
	infoHeader.materialCount = TO_UI32(materials.size());
	infoHeader.opSubsetCount = TO_UI32(m_Subsets.size());
	infoHeader.trSubsetCount = 0;
	infoHeader.verticesSize = m_VerticesSize;
	infoHeader.indicesSize = m_IndicesSize;
	infoHeader.indexType = m_IndexType;
	infoHeader.localCenter[0] = m_LocalCenter.x;
	infoHeader.localCenter[1] = m_LocalCenter.y;
	infoHeader.localCenter[2] = m_LocalCenter.z;
	infoHeader.localCenter[3] = 1.0f;
	infoHeader.halfExtent[0] = m_HalfExtent.x;
	infoHeader.halfExtent[1] = m_HalfExtent.y;
	infoHeader.halfExtent[2] = m_HalfExtent.z;
	infoHeader.halfExtent[3] = 1.0f;

	if (WriteFile(fileHandle, &infoHeader, TO_UI32(sizeof(StaticMesh::InfoHeader)), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// マテリアル

	if (WriteFile(fileHandle, materials.data(), TO_UI32(sizeof(Material::SerialData) * materials.size()), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// 不透明サブセット

	if (WriteFile(fileHandle, m_Subsets.data(), TO_UI32(sizeof(MeshSubset) * m_Subsets.size()), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// 半透明サブセット

	// バーテックス、インデックスバッファー

	uint8_t* pMemory;
	V3D_RESULT result = pHostVertexIndexBuffer->Map(0, 0, reinterpret_cast<void**>(&pMemory));
	if (result == V3D_OK)
	{
		if (WriteFile(fileHandle, pMemory + m_VertexOffset, TO_UI32(m_VerticesSize), &writeSize, nullptr) == FALSE)
		{
			SAFE_RELEASE(pHostVertexIndexBuffer);
			CloseHandle(fileHandle);
			return false;
		}

		if (WriteFile(fileHandle, pMemory + m_IndexOffset, TO_UI32(m_IndicesSize), &writeSize, nullptr) == FALSE)
		{
			SAFE_RELEASE(pHostVertexIndexBuffer);
			CloseHandle(fileHandle);
			return false;
		}

		result = pHostVertexIndexBuffer->Unmap();
	}

	if (result != V3D_OK)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	SAFE_RELEASE(pHostVertexIndexBuffer);

	CloseHandle(fileHandle);

	// ----------------------------------------------------------------------------------------------------

	return true;
}

/*****************************/
/* protected override - Mesh */
/*****************************/

V3D_RESULT StaticMesh::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager)
{
	Mesh::Initialize(pGraphicsManager, pTextureManager, pMaterialManager, pMeshManager);

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを取得
	// ----------------------------------------------------------------------------------------------------

	pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pPipelineLayout);

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT result = pMeshManager->CreateDescriptorSet(&m_pDescriptorSet, &m_UniformDynamicOffsetHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->GetBuffer(0, &m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void StaticMesh::Dispose()
{
	if (m_Subsets.empty() == false)
	{
		m_Subsets.clear();
		m_Subsets.shrink_to_fit();
	}

	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);

	if (m_pMeshManager != nullptr)
	{
		m_pMeshManager->ReleaseDescriptorSet(&m_UniformDynamicOffsetHandle);
	}

	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pPipelineLayout);

	Mesh::Dispose();
}

V3D_RESULT StaticMesh::Import(const wchar_t* pFilePath, const MeshImportDesc& importDesc)
{
	// ----------------------------------------------------------------------------------------------------
	// インポート
	// ----------------------------------------------------------------------------------------------------

	std::vector<Mesh::OptMaterial> srcMaterials;
	std::vector<Mesh::OptPolygon> srcPolygons;
	std::vector<Mesh::OptNode> srcNodes;

	V3D_RESULT result = Mesh::Import(pFilePath, importDesc, srcMaterials, srcPolygons, srcNodes);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// マテリアルを作成
	// ----------------------------------------------------------------------------------------------------

	result = Mesh::CreateMaterials(pFilePath, importDesc, srcMaterials);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// ソースを作成
	// ----------------------------------------------------------------------------------------------------

	std::vector<MeshVertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(srcPolygons.size() * 3);
	indices.reserve(srcPolygons.size() * 3);

	Mesh::CreateSource(importDesc, srcPolygons.begin(), srcPolygons.end(), vertices, indices, m_Subsets, &m_LocalCenter, &m_HalfExtent);

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスバッファーを作成
	// ----------------------------------------------------------------------------------------------------

	result = Mesh::CreateVertexIndexBuffer(vertices, indices, &m_pBuffer, &m_VertexOffset, &m_VerticesSize, &m_IndexOffset, &m_IndicesSize, &m_IndexType);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// キー
	// ----------------------------------------------------------------------------------------------------

	m_Key = reinterpret_cast<uint64_t>(m_pBuffer);

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

bool StaticMesh::Load(const wchar_t* pFilePath)
{
	// ----------------------------------------------------------------------------------------------------
	// 読み込む
	// ----------------------------------------------------------------------------------------------------

	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD readSize;

	// ファイルヘッダ
	StaticMesh::FileHeader fileHeader{};

	if (ReadFile(fileHandle, &fileHeader, TO_UI32(sizeof(StaticMesh::FileHeader)), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	if ((fileHeader.magicNumber != StaticMesh::MagicNumber) || (fileHeader.version != StaticMesh::Version))
	{
		CloseHandle(fileHandle);
		return false;
	}

	// インフォヘッダ
	StaticMesh::InfoHeader infoHeader{};

	if (ReadFile(fileHandle, &infoHeader, TO_UI32(sizeof(StaticMesh::InfoHeader)), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	m_VerticesSize = infoHeader.verticesSize;
	m_IndicesSize = infoHeader.indicesSize;
	m_IndexType = static_cast<V3D_INDEX_TYPE>(infoHeader.indexType);

	m_LocalCenter.x = infoHeader.localCenter[0];
	m_LocalCenter.y = infoHeader.localCenter[1];
	m_LocalCenter.z = infoHeader.localCenter[2];

	m_HalfExtent.x = infoHeader.halfExtent[0];
	m_HalfExtent.y = infoHeader.halfExtent[1];
	m_HalfExtent.z = infoHeader.halfExtent[2];

	// マテリアル
	std::vector<Material::SerialData> materials;
	materials.resize(infoHeader.materialCount);

	if (ReadFile(fileHandle, materials.data(), TO_UI32(sizeof(Material::SerialData) * materials.size()), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	// 不透明サブセット
	m_Subsets.resize(infoHeader.opSubsetCount);
	if (ReadFile(fileHandle, m_Subsets.data(), TO_UI32(sizeof(MeshSubset) * m_Subsets.size()), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	// 半透明サブセット
	;

	// バーテックス
	std::vector<uint8_t> vertices;
	vertices.resize(infoHeader.verticesSize);
	if (ReadFile(fileHandle, vertices.data(), TO_UI32(infoHeader.verticesSize), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	// インデックス
	std::vector<uint8_t> indices;
	indices.resize(infoHeader.indicesSize);
	if (ReadFile(fileHandle, indices.data(), TO_UI32(infoHeader.indicesSize), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	CloseHandle(fileHandle);

	// ----------------------------------------------------------------------------------------------------
	// 作成
	// ----------------------------------------------------------------------------------------------------

	// マテリアル
	if (CreateMaterials(pFilePath, materials) == false)
	{
		return false;
	}

#if 0
	MeshVertex* pVertex = reinterpret_cast<MeshVertex*>(vertices.data());
	MeshVertex* pVertexEnd = pVertex + (vertices.size() / sizeof(MeshVertex));
	while (pVertex != pVertexEnd)
	{
		pVertex->normal = -pVertex->normal;
		pVertex++;
	}
#endif

	// バーテックス、インデックスバッファーをアップロード
	if (UploadVertexIndexBuffer(vertices.size(), vertices.data(), indices.size(), indices.data(), &m_pBuffer, &m_VertexOffset, &m_IndexOffset) == false)
	{
		return false;
	}

	// ----------------------------------------------------------------------------------------------------
	// キー
	// ----------------------------------------------------------------------------------------------------

	m_Key = reinterpret_cast<uint64_t>(m_pBuffer);

	// ----------------------------------------------------------------------------------------------------

	return true;
}
