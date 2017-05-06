#include "Mesh.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "ObjParser.h"

Mesh::Mesh() :
	m_pGraphicsManager(nullptr),
	m_pTextureManager(nullptr),
	m_pMaterialManager(nullptr),
	m_pMeshManager(nullptr),
	m_Key(0),
	m_pPipelineLayout(nullptr),
	m_pBuffer(nullptr),
	m_VertexOffset(0),
	m_IndexOffset(0),
	m_IndexType(V3D_INDEX_TYPE_UINT16),
	m_pUniformBuffer(nullptr),
	m_UniformDynamicOffset(~0U),
	m_pDescriptorSet(nullptr)
{
}

Mesh::~Mesh()
{
	Dispose();
}

std::shared_ptr<Mesh> Mesh::Clone()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, m_pMeshManager);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	mesh->m_Key = m_Key;

	SAFE_ADD_REF(m_pBuffer);
	mesh->m_pBuffer = m_pBuffer;
	mesh->m_VertexOffset = m_VertexOffset;
	mesh->m_IndexOffset = m_IndexOffset;
	mesh->m_IndexType = m_IndexType;

	mesh->m_Subsets.assign(m_Subsets.begin(), m_Subsets.end());
	mesh->m_Materials.assign(m_Materials.begin(), m_Materials.end());

	m_pMeshManager->Add(m_Name.c_str(), mesh);

	return std::move(mesh);
}

uint64_t Mesh::GetKey() const
{
	return m_Key;
}

uint32_t Mesh::GetMaterialCount() const
{
	return static_cast<uint32_t>(m_Materials.size());
}

MaterialPtr Mesh::GetMaterial(uint32_t material)
{
	return m_Materials[material];
}

uint32_t Mesh::GetSubsetCount() const
{
	return static_cast<uint32_t>(m_Subsets.size());
}

const Mesh::Subset& Mesh::GetSubset(uint32_t subset) const
{
	return m_Subsets[subset];
}

const Matrix4x4& Mesh::GetWorldMatrix() const
{
	return m_Uniform.worldMat;
}

void Mesh::SetWorldMatrix(const Matrix4x4& worldMatrix)
{
	m_Uniform.worldMat = worldMatrix;
}

V3D_RESULT Mesh::Update(const Matrix4x4& viewProjMat)
{
	m_Uniform.worldViewProjMat = viewProjMat * m_Uniform.worldMat;

	void* pMemory;

	V3D_RESULT result = m_pUniformBuffer->Map(m_UniformDynamicOffset, sizeof(Mesh::Uniform), &pMemory);
	if (result != V3D_OK)
	{
		return result;
	}

	memcpy_s(pMemory, sizeof(Mesh::Uniform), &m_Uniform, sizeof(Mesh::Uniform));

	result = m_pUniformBuffer->Unmap();
	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

void Mesh::BindVertexIndexBuffer(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindVertexBuffers(0, 1, &m_pBuffer, &m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);
}

void Mesh::BindDescriptorSet(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 1, &m_UniformDynamicOffset);
}

void Mesh::Draw(IV3DCommandBuffer* pCommandBuffer)
{
	pCommandBuffer->BindVertexBuffers(0, 1, &m_pBuffer, &m_VertexOffset);
	pCommandBuffer->BindIndexBuffer(m_pBuffer, m_IndexOffset, m_IndexType);
	pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pPipelineLayout, 0, 1, &m_pDescriptorSet, 1, &m_UniformDynamicOffset);

	const Mesh::Subset* pSubset = m_Subsets.data();
	const Mesh::Subset* pSubsetEnd = pSubset + m_Subsets.size();

	uint64_t materialKey = 0;

	while (pSubset != pSubsetEnd)
	{
		MaterialPtr material = m_Materials[pSubset->material];
		uint64_t nextMaterialKey = material->GetKey();

		if (nextMaterialKey != materialKey)
		{
			material->Bind(pCommandBuffer);
			materialKey = nextMaterialKey;
		}

		pCommandBuffer->DrawIndexed(pSubset->indexCount, 1, pSubset->firstIndex, 0, 0);

		pSubset++;
	}
}

void Mesh::Dispose()
{
	if (m_Materials.empty() == false)
	{
		m_Materials.clear();
		m_Materials.shrink_to_fit();
	}

	if (m_Subsets.empty() == false)
	{
		m_Subsets.clear();
		m_Subsets.shrink_to_fit();
	}

	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);

	if ((m_pMeshManager != nullptr) && (m_UniformDynamicOffset != ~0U))
	{
		m_pMeshManager->ReleaseUniformDynamicOffset(m_UniformDynamicOffset);
	}

	SAFE_RELEASE(m_pBuffer);

	SAFE_RELEASE(m_pPipelineLayout);

	m_pMeshManager = nullptr;
	m_pMaterialManager = nullptr;
	m_pTextureManager = nullptr;
	m_pGraphicsManager = nullptr;
}

V3D_RESULT Mesh::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, MeshManager* pMeshManager)
{
	m_pGraphicsManager = pGraphicsManager;
	m_pTextureManager = pTextureManager;
	m_pMaterialManager = pMaterialManager;
	m_pMeshManager = pMeshManager;

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを取得
	// ----------------------------------------------------------------------------------------------------

	pGraphicsManager->GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pPipelineLayout);

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを取得
	// ----------------------------------------------------------------------------------------------------

	pMeshManager->GetDescriptorSet(&m_pDescriptorSet);

	V3D_RESULT result = m_pDescriptorSet->GetBuffer(0, &m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	pMeshManager->RetainUniformDynamicOffset(&m_UniformDynamicOffset);

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT Mesh::LoadFromFile(const wchar_t* pFilePath, const Mesh::LoadDesc& loadDesc)
{
	// ----------------------------------------------------------------------------------------------------
	// メッシュをバッファに読み込む
	// ----------------------------------------------------------------------------------------------------

	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(fileHandle);

	// ----------------------------------------------------------------------------------------------------
	// ディレクトリパスの抽出
	// ----------------------------------------------------------------------------------------------------

	std::wstring dirPath;

	int32_t filePathLength = static_cast<int32_t>(wcslen(pFilePath));
	int32_t dirPathPos = 0;
	for (int32_t i = filePathLength - 1; i >= 0; i--)
	{
		if ((pFilePath[i] == L'\\') || (pFilePath[i] == L'/'))
		{
			dirPathPos = i;
			break;
		}
	}

	if (dirPathPos > 0)
	{
		dirPath = pFilePath;
		dirPath.resize(dirPathPos + 1);
		dirPath.shrink_to_fit();
	}

	// ----------------------------------------------------------------------------------------------------
	// 展開
	// ----------------------------------------------------------------------------------------------------

	std::vector<Mesh::Vertex> srcVertices;
	std::vector<Mesh::SubsetOpt> srcSubsets;
	std::vector<Mesh::MaterialOpt> srcMaterials;

	V3D_RESULT result = LoadObj(dirPath.c_str(), fileSize, pBuffer, srcVertices, srcSubsets, srcMaterials);
	if (result != V3D_OK)
	{
		delete[] pBuffer;
		return result;
	}

	delete[] pBuffer;

	// ----------------------------------------------------------------------------------------------------
	// 回転
	// ----------------------------------------------------------------------------------------------------

	{
		std::vector<Mesh::Vertex>::iterator it_begin = srcVertices.begin();
		std::vector<Mesh::Vertex>::iterator it_end = srcVertices.end();
		std::vector<Mesh::Vertex>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			it->pos = loadDesc.rotation * it->pos;

			if (loadDesc.invertTex[0] == true) { it->uv.x = 1.0f - it->uv.x; }
			if (loadDesc.invertTex[1] == true) { it->uv.y = 1.0f - it->uv.y; }
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 法線、接線空間を求める
	// ----------------------------------------------------------------------------------------------------

	{
		size_t primitiveCount = srcVertices.size() / 3;

		size_t io0;
		size_t io1;
		size_t io2;

		if (loadDesc.flipFace == true)
		{
			io0 = 2;
			io1 = 1;
			io2 = 0;
		}
		else
		{
			io0 = 0;
			io1 = 1;
			io2 = 2;
		}

		for (size_t i = 0; i < primitiveCount; i++)
		{
			size_t i0 = i * 3 + io0;
			size_t i1 = i * 3 + io1;
			size_t i2 = i * 3 + io2;

			// 法線
			Vector3 ab;
			Vector3 bc;

			if (loadDesc.invertNormal == true)
			{
				ab = srcVertices[i0].pos - srcVertices[i1].pos;
				bc = srcVertices[i2].pos - srcVertices[i1].pos;
			}
			else
			{
				ab = srcVertices[i0].pos - srcVertices[i1].pos;
				bc = srcVertices[i1].pos - srcVertices[i2].pos;
			}

			Vector3 normal = Vector3::Cross(ab, bc).ToNormalize();

			srcVertices[i0].normal = normal;
			srcVertices[i1].normal = normal;
			srcVertices[i2].normal = normal;

			// 接線空間
			Mesh::Vertex* pVertices[3] =
			{
				&(srcVertices[i0]),
				&(srcVertices[i1]),
				&(srcVertices[i2]),
			};

			Vector3 p0 = pVertices[0]->pos;
			Vector3 p1 = pVertices[1]->pos;
			Vector3 p2 = pVertices[2]->pos;

			Vector2 t0 = pVertices[0]->uv;
			Vector2 t1 = pVertices[1]->uv;
			Vector2 t2 = pVertices[2]->uv;

			Vector3 cp0[3] =
			{
				Vector3(p0.x, t0.x, t0.y),
				Vector3(p0.y, t0.x, t0.y),
				Vector3(p0.z, t0.x, t0.y),
			};

			Vector3 cp1[3] =
			{
				Vector3(p1.x, t1.x, t1.y),
				Vector3(p1.y, t1.x, t1.y),
				Vector3(p1.z, t1.x, t1.y),
			};

			Vector3 cp2[3] =
			{
				Vector3(p2.x, t2.x, t2.y),
				Vector3(p2.y, t2.x, t2.y),
				Vector3(p2.z, t2.x, t2.y),
			};

			Vector3 v1;
			Vector3 v2;
			Vector3 abc;

			float u[3];
			float v[3];

			for (uint32_t i = 0; i < 3; i++)
			{
				v1 = cp1[i] - cp0[i];
				v2 = cp2[i] - cp1[i];

				abc = Vector3::Cross(v1, v2);

				if ((-FLOAT_EPSILON <= abc.x) && (FLOAT_EPSILON >= abc.x))
				{
					//縮退しているポリゴン
					u[i] = 1.0f;
					v[i] = 1.0f;
				}
				else
				{
					u[i] = (abc.y / abc.x);
					v[i] = (abc.z / abc.x);
				}
			}

			for (uint32_t i = 0; i < 3; i++)
			{
				pVertices[i]->tangent.x += u[0];
				pVertices[i]->tangent.y += u[1];
				pVertices[i]->tangent.z += u[2];
				pVertices[i]->tangent.Normalize();

				pVertices[i]->binormal.x += v[0];
				pVertices[i]->binormal.y += v[1];
				pVertices[i]->binormal.z += v[2];
				pVertices[i]->binormal.Normalize();
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 最適化
	// ----------------------------------------------------------------------------------------------------

	std::vector<Mesh::Vertex> vertices;
	std::vector<uint32_t> indices;

	Optimize(srcVertices, srcSubsets, vertices, indices, m_Subsets, loadDesc.flipFace, loadDesc.isSmoosing, cosf(loadDesc.smoosingAngle));

	// ----------------------------------------------------------------------------------------------------
	// インデックスタイプ
	// ----------------------------------------------------------------------------------------------------

	std::vector<uint16_t> indices_u16;

	if (indices.size() <= USHRT_MAX)
	{
		std::vector<uint32_t>::iterator it_begin = indices.begin();
		std::vector<uint32_t>::iterator it_end = indices.end();
		std::vector<uint32_t>::iterator it;

		indices_u16.reserve(indices.size());

		for (it = it_begin; it != it_end; ++it)
		{
			indices_u16.push_back(static_cast<uint16_t>(*it));
		}

		m_IndexType = V3D_INDEX_TYPE_UINT16;
	}
	else
	{
		size_t indexCount = indices.size();

		indices_u16.resize(indices.size() * 2);
		uint32_t* pIndices_u32 = reinterpret_cast<uint32_t*>(indices_u16.data());

		for (size_t i = 0; i < indexCount; i++)
		{
			pIndices_u32[i] = indices[i];
		}

		m_IndexType = V3D_INDEX_TYPE_UINT32;
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスバッファを作成
	// ----------------------------------------------------------------------------------------------------

	Array1<BufferSubresourceDesc, 2> tempBufferSubresources;
	tempBufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX;
	tempBufferSubresources[0].size = sizeof(Mesh::Vertex) * vertices.size();
	tempBufferSubresources[0].count = 1;
	tempBufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_INDEX;
	tempBufferSubresources[1].size = sizeof(uint16_t) * indices_u16.size();
	tempBufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> tempBufferMemoryLayouts;
	uint64_t tempBufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_HOST_VISIBLE,
		TO_UI32(tempBufferSubresources.size()),
		tempBufferSubresources.data(),
		tempBufferMemoryLayouts.data(),
		&tempBufferMemorySize);

	// テンポラリバッファ
	V3DBufferDesc tempBufferDesc{};
	tempBufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	tempBufferDesc.size = tempBufferMemorySize;

	IV3DBuffer* pTempBuffer;

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(tempBufferDesc, &pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, pTempBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	void* pMemory;
	result = pTempBuffer->Map(0, pTempBuffer->GetResourceDesc().memorySize, &pMemory);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

	// バーテックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[0].offset, tempBufferMemoryLayouts[0].stride, vertices.data(), tempBufferSubresources[0].size);

	// インデックスをコピー
	MemCopy(pAddr + tempBufferMemoryLayouts[1].offset, tempBufferMemoryLayouts[1].stride, indices_u16.data(), tempBufferSubresources[1].size);

	result = pTempBuffer->Unmap();
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	// 描画用バッファを作成
	Array1<BufferSubresourceDesc, 2> bufferSubresources;
	bufferSubresources[0].usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX;
	bufferSubresources[0].size = sizeof(Mesh::Vertex) * vertices.size();
	bufferSubresources[0].count = 1;
	bufferSubresources[1].usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX;
	bufferSubresources[1].size = sizeof(uint16_t) * indices_u16.size();
	bufferSubresources[1].count = 1;

	Array1<BufferMemoryLayout, 2> bufferMemoryLayouts;
	uint64_t bufferMemorySize;

	CalcBufferMemoryLayout(
		m_pGraphicsManager->GetDevicePtr(),
		V3D_MEMORY_PROPERTY_DEVICE_LOCAL,
		TO_UI32(bufferSubresources.size()),
		bufferSubresources.data(),
		bufferMemoryLayouts.data(),
		&bufferMemorySize);

	V3DBufferDesc bufferDesc{};
	bufferDesc.usageFlags = V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_VERTEX | V3D_BUFFER_USAGE_INDEX;
	bufferDesc.size = bufferMemorySize;

	result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(bufferDesc, &m_pBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, m_pBuffer);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(m_pBuffer);
		SAFE_RELEASE(pTempBuffer);
		return result;
	}

	// テンポラリバッファの内容を描画用バッファに転送
	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(m_pBuffer);
		SAFE_RELEASE(pTempBuffer);
		return V3D_ERROR_FAIL;
	}

	Array1<V3DCopyBufferRange, 2> copyBufferRanges;
	copyBufferRanges[0].dstOffset = bufferMemoryLayouts[0].offset;
	copyBufferRanges[0].srcOffset = tempBufferMemoryLayouts[0].offset;
	copyBufferRanges[0].size = tempBufferSubresources[0].size;
	copyBufferRanges[1].dstOffset = bufferMemoryLayouts[1].offset;
	copyBufferRanges[1].srcOffset = tempBufferMemoryLayouts[1].offset;
	copyBufferRanges[1].size = tempBufferSubresources[1].size;

	pCommandBuffer->CopyBuffer(m_pBuffer, pTempBuffer, TO_UI32(copyBufferRanges.size()), copyBufferRanges.data());

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(m_pBuffer);
		SAFE_RELEASE(pTempBuffer);
		return V3D_ERROR_FAIL;
	}

	SAFE_RELEASE(pTempBuffer);

	m_VertexOffset = bufferMemoryLayouts[0].offset;
	m_IndexOffset = bufferMemoryLayouts[1].offset;

	// ----------------------------------------------------------------------------------------------------
	// マテリアルを作成
	// ----------------------------------------------------------------------------------------------------

	if (srcMaterials.empty() == true)
	{
		Mesh::MaterialOpt materialOpt{};
		materialOpt.name = L"default";
		materialOpt.ambientColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.diffuseColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
		materialOpt.specularColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.emissiveColor.Set(0.0f, 0.0f, 0.0f);
		materialOpt.shininess = 1.0f;
		materialOpt.specularPower = 1.0f;

		srcMaterials.push_back(materialOpt);
	}

	{
		std::vector<Mesh::MaterialOpt>::iterator it_begin = srcMaterials.begin();
		std::vector<Mesh::MaterialOpt>::iterator it_end = srcMaterials.end();
		std::vector<Mesh::MaterialOpt>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			Mesh::MaterialOpt& srcMaterial = (*it);

			MaterialPtr material = m_pMaterialManager->Create(srcMaterial.name.c_str());

			material->SetDiffuseColor(srcMaterial.diffuseColor);

			if (srcMaterial.diffuseTexture.size() != 0)
			{
				// テクスチャのファイルパスを名前と拡張しに分離
				std::wstring textureName = srcMaterial.diffuseTexture;

				size_t separatePoint = ~0U;
				for (int32_t i = static_cast<int32_t>(srcMaterial.diffuseTexture.size()) - 1; i >= 0; i--)
				{
					if (srcMaterial.diffuseTexture[i] == L'.')
					{
						separatePoint = i;
						break;
					}
				}

				if (separatePoint == ~0U)
				{
					return V3D_ERROR_FAIL;
				}

				std::wstring textureExt;
				std::wstringstream textureFilePath;

				textureName = srcMaterial.diffuseTexture;
				textureName[separatePoint] = L'\0';
				textureName.resize(separatePoint);

				textureExt = &srcMaterial.diffuseTexture[separatePoint];

				// ディフューズテクスチャ
				material->SetDiffuseTexture(m_pTextureManager->Load(srcMaterial.diffuseTexture.c_str()));

				// スペキュラー
				material->SetSpecularPower(srcMaterial.specularPower);
				material->SetShininess(srcMaterial.shininess);

				// 法線テクスチャ
				textureFilePath << textureName << L"_n" << textureExt;
				material->SetNormalTexture(m_pTextureManager->Load(textureFilePath.str().c_str()));
			}

			result = material->Update();
			if (result != V3D_OK)
			{
				return result;
			}

			m_Materials.push_back(std::move(material));
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// キー
	// ----------------------------------------------------------------------------------------------------

	m_Key = reinterpret_cast<uint64_t>(m_pBuffer);

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void Mesh::Optimize(
	std::vector<Mesh::Vertex>& srcVertices, std::vector<Mesh::SubsetOpt>& srcSubsets,
	std::vector<Mesh::Vertex>& dstVertices, std::vector<uint32_t>& dstIndices, std::vector<Mesh::Subset>& dstSubsets,
	bool flipFace, bool isSmoosing, float smoosingCos)
{
	// ----------------------------------------------------------------------------------------------------
	// サブセットを更新
	// ----------------------------------------------------------------------------------------------------

	std::vector<Mesh::FaceOpt> faceOpts;

	{
		faceOpts.resize(srcVertices.size() / 3);

		Mesh::FaceOpt* pFace = faceOpts.data();

		std::vector<Mesh::SubsetOpt>::iterator it_subset_begin = srcSubsets.begin();
		std::vector<Mesh::SubsetOpt>::iterator it_subset_end = srcSubsets.end();
		std::vector<Mesh::SubsetOpt>::iterator it_subset;

		uint32_t faceCount = 0;

		for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
		{
			Mesh::SubsetOpt* pSubset = &(*it_subset);
			uint32_t primitiveCount = pSubset->indexCount / 3;
			uint32_t indexCount = 0;

			pSubset->firstFace = faceCount;
			pSubset->faceCount = 0;

			if (flipFace == true)
			{
				for (uint32_t i = 0; i < primitiveCount; i++)
				{
					pSubset->faceCount++;

					pFace->indices[2] = indexCount++;
					pFace->indices[1] = indexCount++;
					pFace->indices[0] = indexCount++;

					pFace++;
				}
			}
			else
			{
				for (uint32_t i = 0; i < primitiveCount; i++)
				{
					pSubset->faceCount++;

					pFace->indices[0] = indexCount++;
					pFace->indices[1] = indexCount++;
					pFace->indices[2] = indexCount++;

					pFace++;
				}
			}

			pSubset->vertices.reserve(pSubset->indexCount);
			pSubset->indices.reserve(pSubset->indexCount);

			for (uint32_t i = 0; i < pSubset->indexCount; i++)
			{
				pSubset->vertices.push_back(srcVertices[pSubset->firstIndex + i]);
				pSubset->indices.push_back(i);
			}

			faceCount += pSubset->faceCount;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// 可能ならば頂点を結合し、新しいインデックスを割り振る
	// ----------------------------------------------------------------------------------------------------

	{
		std::vector<Mesh::SubsetOpt>::iterator it_subset_begin = srcSubsets.begin();
		std::vector<Mesh::SubsetOpt>::iterator it_subset_end = srcSubsets.end();
		std::vector<Mesh::SubsetOpt>::iterator it_subset;

		std::vector<Mesh::Vertex*> combineVertices;

		for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
		{
			Mesh::SubsetOpt* pSubset = &(*it_subset);
			size_t vertexCount = pSubset->vertices.size();

			Mesh::FaceOpt* pFace = faceOpts.data() + pSubset->firstFace;
			Mesh::FaceOpt* pFaceEnd = pFace + pSubset->faceCount;

			while (pFace != pFaceEnd)
			{
				uint32_t* pIndex = &pFace->indices[0];
				uint32_t* pIndexEnd = pIndex + 3;

				while (pIndex != pIndexEnd)
				{
					if (pSubset->indices[*pIndex] == *pIndex)
					{
						// 新しいインデックスが割り振られていないので調べる
						uint32_t newIndex = static_cast<uint32_t>(pSubset->vertices.size());
						Mesh::Vertex* pSrcVertex = &(pSubset->vertices[*pIndex]);

						combineVertices.clear();
						combineVertices.push_back(pSrcVertex);

						// 結合相手の頂点を探す
						for (size_t i = 0; i < vertexCount; i++)
						{
							if (i == *pIndex)
							{
								// 自分だった
								continue;
							}

							Mesh::Vertex* pDstVertex = &(pSubset->vertices[i]);

							if ((pSrcVertex->pos == pDstVertex->pos) &&
								(pSrcVertex->uv == pDstVertex->uv))
							{
								if (isSmoosing == false)
								{
									if (pSrcVertex->normal == pDstVertex->normal)
									{
										//結合相手に新規インデックスを割り振り、結合する頂点を追加
										pSubset->indices[i] = newIndex;
										combineVertices.push_back(pDstVertex);
									}
								}
								else
								{
									float normalCos = Vector3::Dot(pSrcVertex->normal, pDstVertex->normal);

									if (smoosingCos <= normalCos)
									{
										// 結合相手に新規インデックスを割り振り、結合する頂点を追加
										pSubset->indices[i] = newIndex;
										combineVertices.push_back(pDstVertex);
									}
								}
							}
						}

						if (combineVertices.size() > 1)
						{
							// 自分と他のだれかの頂点の結合が要求されている
							Mesh::Vertex** ppCombineVertex = combineVertices.data();

							if (isSmoosing == true)
							{
								size_t combineVertexCount = combineVertices.size();

								Mesh::Vertex** ppVertex = ppCombineVertex;
								Mesh::Vertex** ppVertexEnd = ppVertex + combineVertexCount;

								Vector3 combineNormal;
								Vector3 combineTangent;
								Vector3 combineBinormal;

								while (ppVertex != ppVertexEnd)
								{
									combineNormal += (*ppVertex)->normal;
									combineTangent += (*ppVertex)->tangent;
									combineBinormal += (*ppVertex)->binormal;
									ppVertex++;
								}

								(*ppCombineVertex)->normal = combineNormal.ToNormalize();
								(*ppCombineVertex)->tangent = combineTangent.ToNormalize();
								(*ppCombineVertex)->binormal = combineBinormal.ToNormalize();
							}

							// 自分の新規インデックスを割り振る
							pSubset->indices[*pIndex] = newIndex;

							// 結合した頂点を追加
							pSubset->vertices.push_back(**ppCombineVertex);
						}
					}

					pIndex++;
				}

				pFace++;
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// バーテックス、インデックスのリストを作成
	// ----------------------------------------------------------------------------------------------------

	dstVertices.reserve(srcVertices.size());
	dstIndices.reserve(srcVertices.size());
	dstSubsets.reserve(srcSubsets.size());

	{
		uint32_t newIndex = 0;

		std::vector<Mesh::IndexOpt> indexOpts;

		std::vector<Mesh::SubsetOpt>::iterator it_subset_begin = srcSubsets.begin();
		std::vector<Mesh::SubsetOpt>::iterator it_subset_end = srcSubsets.end();
		std::vector<Mesh::SubsetOpt>::iterator it_subset;

		for (it_subset = it_subset_begin; it_subset != it_subset_end; ++it_subset)
		{
			Mesh::SubsetOpt* pSubset = &(*it_subset);

			indexOpts.clear();
			indexOpts.resize(pSubset->vertices.size());

			Mesh::FaceOpt* pFace = faceOpts.data() + pSubset->firstFace;
			Mesh::FaceOpt* pFaceEnd = pFace + pSubset->faceCount;

			uint32_t firstIndex = static_cast<uint32_t>(dstIndices.size());

			while (pFace != pFaceEnd)
			{
				uint32_t* pIndex = &pFace->indices[0];
				uint32_t* pIndexEnd = pIndex + 3;

				while (pIndex != pIndexEnd)
				{
					uint32_t oldIndex = pSubset->indices[*pIndex];
					Mesh::IndexOpt* pIndexOpt = &indexOpts[oldIndex];

					if (pIndexOpt->assigned == false)
					{
						const Mesh::Vertex& srcVertex = pSubset->vertices[oldIndex];
						Mesh::Vertex dstVertex;

						dstVertex.pos = srcVertex.pos;
						dstVertex.uv = srcVertex.uv;
						dstVertex.normal = srcVertex.normal;
						dstVertex.tangent = srcVertex.tangent;
						dstVertex.binormal = srcVertex.binormal;

						pIndexOpt->index = newIndex;
						pIndexOpt->assigned = true;

						dstVertices.push_back(dstVertex);
						dstIndices.push_back(newIndex);

						newIndex++;
					}
					else
					{
						dstIndices.push_back(pIndexOpt->index);
					}

					pIndex++;
				}

				pFace++;
			}

			Mesh::Subset dstSubset;
			dstSubset.material = pSubset->material;
			dstSubset.indexCount = static_cast<uint32_t>(dstIndices.size()) - firstIndex;
			dstSubset.firstIndex = firstIndex;
			dstSubsets.push_back(dstSubset);
		}
	}

#ifdef _DEBUG
	wchar_t mes[2014];
	::wsprintf(mes, L"vertex[%u] primitive[%u] subset[%u]\n", dstVertices.size(), dstIndices.size() / 3, dstSubsets.size());
	::OutputDebugString(mes);
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
}

V3D_RESULT Mesh::LoadObj(
	const wchar_t* pDirPath,
	uint64_t bufferSize, void* pBuffer,
	std::vector<Mesh::Vertex>& vertices,
	std::vector<Mesh::SubsetOpt>& subsets,
	std::vector<Mesh::MaterialOpt>& materials)
{

	// ----------------------------------------------------------------------------------------------------
	// obj ファイルの読み込み
	// ----------------------------------------------------------------------------------------------------

	{
#ifdef _WIN64
		ObjParser parser(bufferSize, pBuffer);
#else //_WIN64
		ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //_WIN64

		std::vector<Mesh::Vertex> tempVertices;
		tempVertices.reserve(4);

		std::vector<Vector3> positions;
		std::vector<Vector2> textures;
		std::vector<Vector3> normals;

		subsets.push_back(Mesh::SubsetOpt{});
		Mesh::SubsetOpt* pSubset = &subsets.back();

		for (;;)
		{
			ObjParser::ReadResult result = parser.Read();
			if (result.pToken == nullptr)
			{
				break;
			}
			else if (strcmp(result.pToken, "mtllib") == 0)
			{
				// マテリアルライブラリ
				std::wstring materialLib;
				MbToWc(parser.Read().pToken, materialLib);

				std::wstringstream mlStream;
				mlStream << pDirPath << materialLib.c_str();

				V3D_RESULT v3dResult = LoadObjMaterial(pDirPath, mlStream.str().c_str(), materials);
				if (v3dResult != V3D_OK)
				{
					return v3dResult;
				}
			}
			else if (strcmp(result.pToken, "o") == 0)
			{
				// オブジェクト
				parser.Read();
			}
			else if (strcmp(result.pToken, "v") == 0)
			{
				// 頂点座標
				float data[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

				result = ObjParser::ReadResult{};

				for (uint32_t i = 0; i < 4; i++)
				{
					result = parser.Read();
					if (result.pToken == nullptr)
					{
						return V3D_ERROR_FAIL;
					}

					data[i] = static_cast<float>(atof(result.pToken));

					if (result.separator == '\n')
					{
						break;
					}
				}

				float w = FLOAT_RECIPROCAL(data[3]);
				positions.push_back(Vector3(data[0] * w, data[1] * w, data[2] * w));
			}
			else if (strcmp(result.pToken, "vt") == 0)
			{
				// 頂点テクスチャ座標
				float data[3] = { 0.0f, 0.0f, 0.0f };

				result = ObjParser::ReadResult{};

				for (uint32_t i = 0; i < 3; i++)
				{
					result = parser.Read();
					if (result.pToken == nullptr)
					{
						return V3D_ERROR_FAIL;
					}

					data[i] = static_cast<float>(atof(result.pToken));

					if (result.separator == '\n')
					{
						break;
					}
				}

				textures.push_back(Vector2(data[0], data[1]));
			}
			else if (strcmp(result.pToken, "vn") == 0)
			{
				// 頂点法線
				float data[3] = { 0.0f, 0.0f, 0.0f };

				result = ObjParser::ReadResult{};

				for (uint32_t i = 0; i < 3; i++)
				{
					result = parser.Read();
					if (result.pToken == nullptr)
					{
						return V3D_ERROR_FAIL;
					}

					data[i] = static_cast<float>(atof(result.pToken));

					if (result.separator == '\n')
					{
						break;
					}
				}

				normals.push_back(Vector3(data[0], data[1], data[2]));
			}
			else if (strcmp(result.pToken, "usemtl") == 0)
			{
				// マテリアルを設定
				std::wstring materialName;
				MbToWc(parser.Read().pToken, materialName);

				uint32_t materialCount = static_cast<uint32_t>(materials.size());
				uint32_t materialIndex = ~0U;
				for (uint32_t i = 0; i < materialCount; i++)
				{
					if (materialName == materials[i].name)
					{
						materialIndex = i;
						break;
					}
				}

				if (materialIndex == ~0U)
				{
					return V3D_ERROR_FAIL;
				}

				if ((pSubset->indexCount > 0) && (pSubset->material != materialIndex))
				{
					uint32_t firstVertex = pSubset->firstIndex + pSubset->indexCount;

					subsets.push_back(Mesh::SubsetOpt{});
					pSubset = &subsets.back();

					pSubset->material = materialIndex;
					pSubset->firstIndex = firstVertex;
				}
			}
			else if (strcmp(result.pToken, "s") == 0)
			{
				// スムージング ( 1 or off )
				parser.Read();
			}
			else if (strcmp(result.pToken, "f") == 0)
			{
				// フェイス
				long long indices[3] = { -1, -1, -1 };
				bool faceLoop = true;

				tempVertices.clear();
				result = ObjParser::ReadResult{};

				while (faceLoop == true)
				{
					for (uint32_t i = 0; i < 3; i++)
					{
						result = parser.Read();
						if (result.pToken == nullptr)
						{
							return V3D_ERROR_FAIL;
						}
						else
						{
							if (strlen(result.pToken) > 0)
							{
								indices[i] = atoll(result.pToken);
							}

							if (result.separator == ' ')
							{
								break;
							}
							else if (result.separator == '\n')
							{
								faceLoop = false;
								break;
							}
						}
					}

					Mesh::Vertex vertex;

					if (indices[0] > 0) { vertex.pos = positions[static_cast<uint32_t>(indices[0] - 1)]; }
					if (indices[1] > 0) { vertex.uv = textures[static_cast<uint32_t>(indices[1] - 1)]; }
					if (indices[2] > 0) { vertex.normal = normals[static_cast<uint32_t>(indices[2] - 1)]; }

					tempVertices.push_back(vertex);
				}

				uint32_t vertexCount = static_cast<uint32_t>(tempVertices.size());

				if (vertexCount == 3)
				{
					vertices.push_back(tempVertices[0]);
					vertices.push_back(tempVertices[1]);
					vertices.push_back(tempVertices[2]);

					pSubset->indexCount += 3;
				}
				else if (vertexCount == 4)
				{
					vertices.push_back(tempVertices[0]);
					vertices.push_back(tempVertices[1]);
					vertices.push_back(tempVertices[2]);
					vertices.push_back(tempVertices[2]);
					vertices.push_back(tempVertices[3]);
					vertices.push_back(tempVertices[0]);

					pSubset->indexCount += 6;
				}
				else if (vertexCount > 4)
				{
					uint32_t pointCount = vertexCount - 3;
					uint32_t lastPoint = vertexCount - 1;

					for (uint32_t i = 0; i < pointCount; i++)
					{
						vertices.push_back(tempVertices[i + 0]);
						vertices.push_back(tempVertices[i + 1]);
						vertices.push_back(tempVertices[lastPoint]);
					}

					pSubset->indexCount += pointCount * 3;
				}
				else
				{
					return V3D_ERROR_FAIL;
				}
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT Mesh::LoadObjMaterial(const wchar_t* pDirPath, const wchar_t* pFilePath, std::vector<Mesh::MaterialOpt>& materials)
{
	HANDLE fileHandle = CreateFile(pFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		return V3D_ERROR_FAIL;
	}

	DWORD readSize = 0;
	DWORD fileSize = ::GetFileSize(fileHandle, nullptr);
	uint8_t* pBuffer = new uint8_t[fileSize];

	if (ReadFile(fileHandle, pBuffer, fileSize, &readSize, nullptr) == FALSE)
	{
		delete[] pBuffer;
		CloseHandle(fileHandle);
		return V3D_ERROR_FAIL;
	}

	CloseHandle(fileHandle);

	V3D_RESULT result = LoadObjMaterial(pDirPath, fileSize, pBuffer, materials);
	if (result != V3D_OK)
	{
		delete[] pBuffer;
		return V3D_ERROR_FAIL;
	}

	delete[] pBuffer;

	return V3D_OK;
}

V3D_RESULT Mesh::LoadObjMaterial(
	const wchar_t* pDirPath,
	uint64_t bufferSize, void* pBuffer,
	std::vector<Mesh::MaterialOpt>& materials)
{
#ifdef _WIN64
	ObjParser parser(bufferSize, pBuffer);
#else //_WIN64
	ObjParser parser(TO_UI32(bufferSize), pBuffer);
#endif //_WIN64

	Mesh::MaterialOpt* pMaterial = nullptr;

	for (;;)
	{
		ObjParser::ReadResult result = parser.Read();
		if (result.pToken == nullptr)
		{
			break;
		}
		else if (strcmp(result.pToken, "newmtl") == 0)
		{
			materials.push_back(Mesh::MaterialOpt{});
			pMaterial = &materials.back();
			MbToWc(parser.Read().pToken, pMaterial->name);
		}
		else if (strcmp(result.pToken, "Ns") == 0)
		{
			pMaterial->specularPower = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "Ka") == 0)
		{
			pMaterial->ambientColor.r = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->ambientColor.g = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->ambientColor.b = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "Kd") == 0)
		{
			pMaterial->diffuseColor.r = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->diffuseColor.g = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->diffuseColor.b = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "Ks") == 0)
		{
			pMaterial->specularColor.r = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->specularColor.g = static_cast<float>(atof(parser.Read().pToken));
			pMaterial->specularColor.b = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "Ni") == 0)
		{
			pMaterial->shininess = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "d") == 0)
		{
			pMaterial->diffuseColor.a = static_cast<float>(atof(parser.Read().pToken));
		}
		else if (strcmp(result.pToken, "Tr") == 0)
		{
			pMaterial->diffuseColor.a = max(0.0f, 1.0f - static_cast<float>(atof(parser.Read().pToken)));
		}
		else if (strcmp(result.pToken, "illum") == 0)
		{
			parser.Read();
		}
		else if (strcmp(result.pToken, "map_Kd") == 0)
		{
			MbToWc(parser.Read().pToken, pMaterial->diffuseTexture);
		}
	}

	return V3D_OK;
}
