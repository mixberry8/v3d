#include "SkeletalMesh.h"
#include "MeshManager.h"
#include "Material.h"

/*****************************/
/* public - SkeletalMeshNode */
/*****************************/

SkeletalMeshNode::SkeletalMeshNode() :
	m_ParentIndex(~0U),
	m_pPipelineLayout(nullptr),
	m_Uniform({}),
	m_pUniformBuffer(nullptr),
	m_pDescriptorSet(nullptr)
{
}

SkeletalMeshNode::~SkeletalMeshNode()
{
	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);
	SAFE_RELEASE(m_pPipelineLayout);
}

const wchar_t* SkeletalMeshNode::GetName() const
{
	return m_Name.c_str();
}

IV3DDescriptorSet* SkeletalMeshNode::GetDescriptorSetPtr()
{
	return m_pDescriptorSet;
}

uint32_t SkeletalMeshNode::GetDynamicOffset() const
{
	return m_UniformDynamicOffsetHandle.GetOffset();
}

const Matrix4x4& SkeletalMeshNode::GetLocalMatrix() const
{
	return m_LocalMatrix;
}

void SkeletalMeshNode::SetLocalMatrix(const Matrix4x4& localMatrix)
{
	m_LocalMatrix = localMatrix;
}

const Matrix4x4& SkeletalMeshNode::GetWorldMatrix() const
{
	return m_Uniform.worldMat;
}

const MeshBounds& SkeletalMeshNode::GetBounds() const
{
	return m_Bounds;
}

uint32_t SkeletalMeshNode::GetSubsetCount() const
{
	return TO_UI32(m_Subsets.size());
}

const MeshSubset& SkeletalMeshNode::GetSubset(uint32_t subset) const
{
	return m_Subsets[subset];
}

void SkeletalMeshNode::BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer)
{
	uint32_t dynamicOffset = m_UniformDynamicOffsetHandle.GetOffset();

	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 1, &dynamicOffset);
}

V3D_RESULT SkeletalMeshNode::Update(const Matrix4x4& parentWorldMatrix)
{
	m_Uniform.worldMat = m_LocalMatrix * parentWorldMatrix;

	if (m_pUniformBuffer != nullptr)
	{
		// ユニフォームバッファーを更新
		void* pMemory;
		V3D_RESULT result = m_pUniformBuffer->Map(m_UniformDynamicOffsetHandle.GetOffset(), sizeof(SkeletalMeshNode::Uniform), &pMemory);
		if(result == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(SkeletalMeshNode::Uniform), &m_Uniform, sizeof(SkeletalMeshNode::Uniform));
			result = m_pUniformBuffer->Unmap();
		}

		if (result != V3D_OK)
		{
			return result;
		}

		// 境界を更新
		Vector3 center = m_Uniform.worldMat * m_LocalCenter;
		Vector3 halfExtent = m_HalfExtent;

		m_Uniform.worldMat.TransformSR(1, &halfExtent);

		m_Bounds.aabb.minimum = center - halfExtent;
		m_Bounds.aabb.maximum = center + halfExtent;
		m_Bounds.aabb.UpdateCenterAndPoints();

		m_Bounds.sphere.center = center;
		m_Bounds.sphere.radius = halfExtent.GetLength();
	}

	std::vector<std::shared_ptr<SkeletalMeshNode>>::iterator it_child_begin = m_Childs.begin();
	std::vector<std::shared_ptr<SkeletalMeshNode>>::iterator it_child_end = m_Childs.end();
	std::vector<std::shared_ptr<SkeletalMeshNode>>::iterator it_child;

	for (it_child = it_child_begin; it_child != it_child_end; ++it_child)
	{
		V3D_RESULT result = (*it_child)->Update(m_Uniform.worldMat);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	return V3D_OK;
}

/*************************/
/* public - SkeletalMesh */
/*************************/

SkeletalMesh::SkeletalMesh() :
m_pPipelineLayout(nullptr),
m_pBuffer(nullptr),
m_VertexOffset(0),
m_VerticesSize(0),
m_IndexOffset(0),
m_IndicesSize(0),
m_IndexType(V3D_INDEX_TYPE_UINT16)
{
}

SkeletalMesh::~SkeletalMesh()
{
	SAFE_RELEASE(m_pPipelineLayout);
}

SkeletalMeshNodePtr SkeletalMesh::GetRootNode()
{
	return m_Nodes[0];
}

uint32_t SkeletalMesh::GetNodeCount() const
{
	return TO_UI32(m_Nodes.size());
}

SkeletalMeshNodePtr SkeletalMesh::GetNode(uint32_t node)
{
	return m_Nodes[node];
}

uint32_t SkeletalMesh::GetMeshNodeCount() const
{
	return TO_UI32(m_MeshNodes.size());
}

SkeletalMeshNodePtr SkeletalMesh::GetMeshNode(uint32_t meshNode)
{
	return m_MeshNodes[meshNode];
}

const Matrix4x4& SkeletalMesh::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

void SkeletalMesh::SetWorldMatrix(const Matrix4x4& worldMatrix)
{
	m_WorldMatrix = worldMatrix;
}

void SkeletalMesh::BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindVertexBuffers(0, 1, &m_pBuffer, &m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);
}

void SkeletalMesh::Draw(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindVertexBuffers(0, 1, &m_pBuffer, &m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);

	std::vector<SkeletalMeshNodePtr>::iterator it_node_begin = m_MeshNodes.begin();
	std::vector<SkeletalMeshNodePtr>::iterator it_node_end = m_MeshNodes.end();
	std::vector<SkeletalMeshNodePtr>::iterator it_node;

	for (it_node = it_node_begin; it_node != it_node_end; ++it_node)
	{
		SkeletalMeshNodePtr node = (*it_node);
		node->BindDescriptorSet(pCommandBuffer);

		uint32_t subsetCount = node->GetSubsetCount();
		for (uint32_t i = 0; i < subsetCount; i++)
		{
			const MeshSubset& subset = node->GetSubset(i);

			MaterialPtr material = GetMaterial(subset.material);
			material->Bind(pCommandBuffer, RENDER_TYPE_GEOMETRY);

			pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
		}
	}
}

std::shared_ptr<SkeletalMesh> SkeletalMesh::Clone()
{
	std::shared_ptr<SkeletalMesh> mesh = std::make_shared<SkeletalMesh>();

	if (mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, m_pMeshManager) != V3D_OK)
	{
		return nullptr;
	}

	std::vector<SkeletalMeshNodePtr>::iterator it_begin = m_Nodes.begin();
	std::vector<SkeletalMeshNodePtr>::iterator it_end = m_Nodes.end();
	std::vector<SkeletalMeshNodePtr>::iterator it;

	mesh->m_Key = m_Key;
	mesh->m_Nodes.reserve(m_Nodes.size());
	mesh->m_MeshNodes.reserve(m_MeshNodes.size());

	for (it = it_begin; it != it_end; ++it)
	{
		SkeletalMeshNodePtr srcNode = (*it);
		SkeletalMeshNodePtr dstNode = std::make_shared<SkeletalMeshNode>();

		dstNode->m_Name = srcNode->m_Name;
		dstNode->m_ParentIndex = srcNode->m_ParentIndex;

		if (srcNode->m_ParentIndex != ~0U)
		{
			dstNode->m_Parent = mesh->m_Nodes[srcNode->m_ParentIndex];

			SkeletalMeshNodePtr parentNode = dstNode->m_Parent.lock();
			parentNode->m_Childs.push_back(dstNode);

		}

		if (srcNode->m_Subsets.empty() == false)
		{
			dstNode->m_Subsets.assign(srcNode->m_Subsets.begin(), srcNode->m_Subsets.end());
		}

		dstNode->m_LocalMatrix = srcNode->m_LocalMatrix;

		dstNode->m_LocalCenter = srcNode->m_LocalCenter;
		dstNode->m_HalfExtent = srcNode->m_HalfExtent;
		dstNode->m_Bounds = srcNode->m_Bounds;

		SAFE_ADD_REF(srcNode->m_pPipelineLayout);
		dstNode->m_pPipelineLayout = srcNode->m_pPipelineLayout;

		dstNode->m_Uniform = srcNode->m_Uniform;

		if (srcNode->m_pDescriptorSet != nullptr)
		{
			if (m_pMeshManager->CreateDescriptorSet(&dstNode->m_pDescriptorSet, &dstNode->m_UniformDynamicOffsetHandle) != V3D_OK)
			{
				return nullptr;
			}

			if (dstNode->m_pDescriptorSet->GetBuffer(0, &dstNode->m_pUniformBuffer) != V3D_OK)
			{
				return nullptr;
			}

			mesh->m_MeshNodes.push_back(dstNode);
		}

		mesh->m_Nodes.push_back(dstNode);
	}

	SAFE_ADD_REF(m_pBuffer);
	mesh->m_pBuffer = m_pBuffer;
	mesh->m_VertexOffset = m_VertexOffset;
	mesh->m_IndexOffset = m_IndexOffset;
	mesh->m_IndexType = m_IndexType;

	CloneMaterials(mesh.get());

	m_pMeshManager->Add(m_Name.c_str(), mesh);

	return mesh;
}

/**************************/
/* public override - Mesh */
/**************************/

MESH_TYPE SkeletalMesh::GetType() const
{
	return MESH_TYPE_SKELETAL;
}

IV3DBuffer* SkeletalMesh::GetVertexIndexBuffer()
{
	return m_pBuffer;
}

uint64_t SkeletalMesh::GetVertexOffset() const
{
	return m_VertexOffset;
}

uint64_t SkeletalMesh::GetIndexOffset() const
{
	return m_IndexOffset;
}

V3D_INDEX_TYPE SkeletalMesh::GetIndexType() const
{
	return m_IndexType;
}

V3D_RESULT SkeletalMesh::Update()
{
	V3D_RESULT result = m_Nodes[0]->Update(m_WorldMatrix);
	if (result != V3D_OK)
	{
		return result;
	}

	std::vector<SkeletalMeshNodePtr>::iterator it_node_begin = m_MeshNodes.begin();
	std::vector<SkeletalMeshNodePtr>::iterator it_node_end = m_MeshNodes.end();
	std::vector<SkeletalMeshNodePtr>::iterator it_node;

	Vector3 minimum(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	Vector3 maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (it_node = it_node_begin; it_node != it_node_end; ++it_node)
	{
		const MeshBounds& bounds = (*it_node)->GetBounds();

		minimum = Vector3::Minimum(minimum, bounds.aabb.minimum);
		maximum = Vector3::Maximum(maximum, bounds.aabb.maximum);
	}

	m_Bounds.aabb.minimum = minimum;
	m_Bounds.aabb.maximum = maximum;
	m_Bounds.aabb.UpdateCenterAndPoints();

	m_Bounds.sphere.radius = (m_Bounds.aabb.minimum - m_Bounds.aabb.center).GetLength();
	m_Bounds.sphere.center = m_Bounds.aabb.center;

	return V3D_OK;
}

bool SkeletalMesh::Save(const wchar_t* pFilePath)
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
	// サブセットリスト、ノードリストを作成
	// ----------------------------------------------------------------------------------------------------

	std::vector<SkeletalMesh::NodeInfo> nodeInfos;
	std::vector<MeshSubset> opSubsets;

	std::vector<SkeletalMeshNodePtr>::iterator it_node_begin = m_Nodes.begin();
	std::vector<SkeletalMeshNodePtr>::iterator it_node_end = m_Nodes.end();
	std::vector<SkeletalMeshNodePtr>::iterator it_node;

	opSubsets.reserve(m_Nodes.size() * 4);
	nodeInfos.reserve(m_Nodes.size());

	for (it_node = it_node_begin; it_node != it_node_end; ++it_node)
	{
		SkeletalMeshNodePtr node = (*it_node);
		SkeletalMesh::NodeInfo nodeInfo{};

		if ((sizeof(nodeInfo.name) >> 1) <= node->m_Name.size())
		{
			return false;
		}

		const Vector3& localCenter = node->m_LocalCenter;
		const Vector3& halfExtent = node->m_HalfExtent;

		wcscpy_s(nodeInfo.name, node->m_Name.c_str());
		nodeInfo.parentNode = node->m_ParentIndex;
		nodeInfo.opSubsetCount = TO_UI32(node->m_Subsets.size());
		nodeInfo.opFirstSubset = TO_UI32(opSubsets.size());
		nodeInfo.trSubsetCount = 0;
		nodeInfo.trFirstSubset = 0;
		nodeInfo.localCenter.x = localCenter.x;
		nodeInfo.localCenter.y = localCenter.y;
		nodeInfo.localCenter.z = localCenter.z;
		nodeInfo.localCenter.w = 1.0f;
		nodeInfo.halfExtent.x = halfExtent.x;
		nodeInfo.halfExtent.y = halfExtent.y;
		nodeInfo.halfExtent.z = halfExtent.z;
		nodeInfo.halfExtent.w = 1.0f;

		if (node->m_Subsets.size() > 0)
		{
			const std::vector<MeshSubset>& subsets = node->m_Subsets;

			std::vector<MeshSubset>::const_iterator it_subset_begin = subsets.begin();
			std::vector<MeshSubset>::const_iterator it_subset_end = subsets.end();
			std::vector<MeshSubset>::const_iterator it_subset;

			for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
			{
				opSubsets.push_back(*it_subset);
			}
		}

		nodeInfos.push_back(nodeInfo);
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスをホストメモリにロード
	// ----------------------------------------------------------------------------------------------------

	IV3DBuffer* pHostVertexIndexBuffer;

	if (Mesh::LoadVertexIndexBuffer(m_pBuffer, &pHostVertexIndexBuffer) == false)
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
	SkeletalMesh::FileHeader fileHeader{};
	fileHeader.magicNumber = SkeletalMesh::MagicNumber;
	fileHeader.version = SkeletalMesh::Version;

	if (WriteFile(fileHandle, &fileHeader, TO_UI32(sizeof(SkeletalMesh::FileHeader)), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// インフォヘッダ
	SkeletalMesh::InfoHeader infoHeader{};
	infoHeader.materialCount = TO_UI32(materials.size());
	infoHeader.opSubsetCount = TO_UI32(opSubsets.size());
	infoHeader.trSubsetCount = 0;
	infoHeader.nodeCount = TO_UI32(nodeInfos.size());
	infoHeader.verticesSize = m_VerticesSize;
	infoHeader.indicesSize = m_IndicesSize;
	infoHeader.indexType = m_IndexType;

	if (WriteFile(fileHandle, &infoHeader, TO_UI32(sizeof(SkeletalMesh::InfoHeader)), &writeSize, nullptr) == FALSE)
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

	if (WriteFile(fileHandle, opSubsets.data(), TO_UI32(sizeof(MeshSubset) * opSubsets.size()), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	// 半透明サブセット

	// ノード

	if (WriteFile(fileHandle, nodeInfos.data(), TO_UI32(sizeof(SkeletalMesh::NodeInfo) * nodeInfos.size()), &writeSize, nullptr) == FALSE)
	{
		SAFE_RELEASE(pHostVertexIndexBuffer);
		CloseHandle(fileHandle);
		return false;
	}

	uint8_t* pMemory;
	V3D_RESULT result = pHostVertexIndexBuffer->Map(0, V3D_WHOLE_SIZE, reinterpret_cast<void**>(&pMemory));
	if (result == V3D_OK)
	{
		// バーテックス
		if (WriteFile(fileHandle, pMemory + m_VertexOffset, TO_UI32(m_VerticesSize), &writeSize, nullptr) == FALSE)
		{
			SAFE_RELEASE(pHostVertexIndexBuffer);
			CloseHandle(fileHandle);
			return false;
		}

		// インデックス
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

V3D_RESULT SkeletalMesh::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager)
{
	Mesh::Initialize(pGraphicsManager, pTextureManager, pMaterialManager, pMeshManager);

	m_pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pPipelineLayout);

	return V3D_OK;
}

void SkeletalMesh::Dispose()
{
	if (m_MeshNodes.empty() == false)
	{
		m_MeshNodes.clear();
	}

	if (m_Nodes.empty() == false)
	{
		std::vector<SkeletalMeshNodePtr>::iterator it_begin = m_Nodes.begin();
		std::vector<SkeletalMeshNodePtr>::iterator it_end = m_Nodes.end();
		std::vector<SkeletalMeshNodePtr>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			m_pMeshManager->ReleaseDescriptorSet(&(*it)->m_UniformDynamicOffsetHandle);
		}

		m_Nodes.clear();
	}

	SAFE_RELEASE(m_pBuffer);

	Mesh::Dispose();
}

V3D_RESULT SkeletalMesh::Import(const wchar_t* pFilePath, const MeshImportDesc& importDesc)
{
	// ----------------------------------------------------------------------------------------------------
	// ロード
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
	// ノードを作成
	// ----------------------------------------------------------------------------------------------------

	std::vector<MeshVertex> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(srcPolygons.size() * 3);
	indices.reserve(srcPolygons.size() * 3);

	size_t nodeCount = srcNodes.size();

	Mesh::OptNode* pSrcNode = &srcNodes[0];
	Mesh::OptNode* pSrcNodeEnd = pSrcNode + nodeCount;

	m_Nodes.reserve(nodeCount);

	uint32_t nodeIndex = 1;

	while (pSrcNode != pSrcNodeEnd)
	{
		SkeletalMeshNodePtr node = std::make_shared<SkeletalMeshNode>();

		node->m_Name = pSrcNode->name;
		node->m_ParentIndex = pSrcNode->parentNode;
		node->m_LocalMatrix = pSrcNode->localMatrix;

		std::wstringstream stringStream;
		stringStream << L"node(" << nodeIndex << L") : name[" << node->m_Name << L"] ";
		nodeIndex++;

		if (pSrcNode->parentNode != ~0U)
		{
			// 子ノードリストに追加
			node->m_Parent = m_Nodes[pSrcNode->parentNode];

			SkeletalMeshNodePtr parentNode = node->m_Parent.lock();
			parentNode->m_Childs.push_back(node);
		}

		if (pSrcNode->polygonCount > 0)
		{
			// ソースを作成
			std::vector<OptPolygon>::iterator it_polygon_begin = srcPolygons.begin() + pSrcNode->firstPolygon;
			std::vector<OptPolygon>::iterator it_polygon_end = it_polygon_begin + pSrcNode->polygonCount;
			Mesh::CreateSource(importDesc, it_polygon_begin, it_polygon_end, vertices, indices, node->m_Subsets, &node->m_LocalCenter, &node->m_HalfExtent);

			// パイプラインレアウトを取得
			SAFE_ADD_REF(m_pPipelineLayout);
			node->m_pPipelineLayout = m_pPipelineLayout;

			// デスクリプタセット作成
			result = m_pMeshManager->CreateDescriptorSet(&node->m_pDescriptorSet, &node->m_UniformDynamicOffsetHandle);
			if (result != V3D_OK)
			{
				return result;
			}

			// デスクリプタセットからユニフォームバッファを取得
			result = node->m_pDescriptorSet->GetBuffer(0, &node->m_pUniformBuffer);
			if (result != V3D_OK)
			{
				return result;
			}

			m_MeshNodes.push_back(node);
		}

		m_Nodes.push_back(node);

		pSrcNode++;
	}

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

bool SkeletalMesh::Load(const wchar_t* pFilePath)
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
	SkeletalMesh::FileHeader fileHeader{};
	if (ReadFile(fileHandle, &fileHeader, TO_UI32(sizeof(SkeletalMesh::FileHeader)), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	if ((fileHeader.magicNumber != SkeletalMesh::MagicNumber) || (fileHeader.version != SkeletalMesh::Version))
	{
		CloseHandle(fileHandle);
		return false;
	}

	// インフォヘッダ
	SkeletalMesh::InfoHeader infoHeader{};
	if (ReadFile(fileHandle, &infoHeader, TO_UI32(sizeof(SkeletalMesh::InfoHeader)), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	m_VerticesSize = infoHeader.verticesSize;
	m_IndicesSize = infoHeader.indicesSize;
	m_IndexType = static_cast<V3D_INDEX_TYPE>(infoHeader.indexType);

	// マテリアル
	std::vector<Material::SerialData> materials;
	materials.resize(infoHeader.materialCount);

	if (ReadFile(fileHandle, materials.data(), TO_UI32(sizeof(Material::SerialData) * materials.size()), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	// 不透明サブセット
	std::vector<MeshSubset> opSubsets;
	opSubsets.resize(infoHeader.opSubsetCount);

	if (ReadFile(fileHandle, opSubsets.data(), TO_UI32(sizeof(MeshSubset) * opSubsets.size()), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

	// 半透明サブセット
	;

	// ノード
	std::vector<SkeletalMesh::NodeInfo> nodeInfos;
	nodeInfos.resize(infoHeader.nodeCount);

	if (ReadFile(fileHandle, nodeInfos.data(), TO_UI32(sizeof(SkeletalMesh::NodeInfo) * nodeInfos.size()), &readSize, nullptr) == FALSE)
	{
		CloseHandle(fileHandle);
		return false;
	}

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

	// ノード、サブセット
	std::vector<SkeletalMesh::NodeInfo>::const_iterator it_node_begin = nodeInfos.begin();
	std::vector<SkeletalMesh::NodeInfo>::const_iterator it_node_end = nodeInfos.end();
	std::vector<SkeletalMesh::NodeInfo>::const_iterator it_node;

	m_Nodes.reserve(infoHeader.nodeCount);

	for (it_node = it_node_begin; it_node != it_node_end; ++it_node)
	{
		const SkeletalMesh::NodeInfo& srcNode = *it_node;
		SkeletalMeshNodePtr dstNode = std::make_shared<SkeletalMeshNode>();

		dstNode->m_Name = srcNode.name;
		dstNode->m_ParentIndex = srcNode.parentNode;

		if (srcNode.parentNode != ~0U)
		{
			SkeletalMeshNodePtr parentNode = m_Nodes[srcNode.parentNode];
			parentNode->m_Childs.push_back(dstNode);
			dstNode->m_Parent = parentNode;
		}

		if (srcNode.opSubsetCount > 0)
		{
			std::vector<MeshSubset>::iterator it_begin = opSubsets.begin() + srcNode.opFirstSubset;
			std::vector<MeshSubset>::iterator it_end = it_begin + srcNode.opSubsetCount;

			dstNode->m_Subsets.assign(it_begin, it_end);
		}

		if (srcNode.trSubsetCount > 0)
		{
			;
		}

		dstNode->m_LocalCenter.x = srcNode.localCenter.x;
		dstNode->m_LocalCenter.y = srcNode.localCenter.y;
		dstNode->m_LocalCenter.z = srcNode.localCenter.z;

		dstNode->m_HalfExtent.x = srcNode.halfExtent.x;
		dstNode->m_HalfExtent.y = srcNode.halfExtent.y;
		dstNode->m_HalfExtent.z = srcNode.halfExtent.z;

		dstNode->m_LocalMatrix = srcNode.localMatrix;

		if ((srcNode.opSubsetCount > 0) || srcNode.trSubsetCount > 0)
		{
			// パイプラインレアウトを取得
			m_pGraphicsManager->GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_GEOMETRY, &dstNode->m_pPipelineLayout);

			// デスクリプタセット作成
			V3D_RESULT result = m_pMeshManager->CreateDescriptorSet(&dstNode->m_pDescriptorSet, &dstNode->m_UniformDynamicOffsetHandle);
			if (result != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットからユニフォームバッファを取得
			result = dstNode->m_pDescriptorSet->GetBuffer(0, &dstNode->m_pUniformBuffer);
			if (result != V3D_OK)
			{
				return false;
			}

			m_MeshNodes.push_back(dstNode);
		}

		m_Nodes.push_back(dstNode);
	}

	// バーテックス、インデックスバッファー
	if (StoreVertexIndexBuffer(vertices.size(), vertices.data(), indices.size(), indices.data(), &m_pBuffer, &m_VertexOffset, &m_IndexOffset) == false)
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
