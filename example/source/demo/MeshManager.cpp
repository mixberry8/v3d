#include "MeshManager.h"

MeshManager::MeshManager() :
	m_pGraphicsManager(nullptr),
	m_pTextureManager(nullptr),
	m_pMaterialManager(nullptr),
	m_pMemory(nullptr),
	m_pUniformBuffer(nullptr),
	m_pDescriptorSet(nullptr),
	m_InuseDynamicOffsetCount(0)
{
}

MeshManager::~MeshManager()
{
}

V3D_RESULT MeshManager::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, uint32_t maxMesh)
{
	m_pGraphicsManager = pGraphicsManager;
	m_pTextureManager = pTextureManager;
	m_pMaterialManager = pMaterialManager;
	m_MaxMesh = maxMesh;
	m_UnuseUniformDynamicOffsets.reserve(maxMesh);

	// ----------------------------------------------------------------------------------------------------
	// ユニフォームバッファを作成
	// ----------------------------------------------------------------------------------------------------

	BufferSubresourceDesc uniformBufferSubresource;
	uniformBufferSubresource.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	uniformBufferSubresource.size = sizeof(Mesh::Uniform);
	uniformBufferSubresource.count = maxMesh;

	BufferMemoryLayout uniformBufferMemoryLayout;
	uint64_t uniformBufferMemorySize;

	CalcBufferMemoryLayout(pGraphicsManager->GetDevicePtr(), V3D_MEMORY_PROPERTY_HOST_VISIBLE, 1, &uniformBufferSubresource, &uniformBufferMemoryLayout, &uniformBufferMemorySize);

	V3DBufferDesc uniformBufferDesc{};
	uniformBufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	uniformBufferDesc.size = uniformBufferMemorySize;

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(uniformBufferDesc, &m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	m_pUniformBuffer->GetResourceMemory(&m_pMemory);

	m_UniformStride = TO_UI32(uniformBufferMemoryLayout.stride);

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	result = pGraphicsManager->CreateDescriptorSet(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, GRAPHICS_DESCRIPTOR_SET_TYPE_MESH, &m_pDescriptorSet);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->SetBuffer(0, m_pUniformBuffer, 0, sizeof(Mesh::Uniform));
	if (result != V3D_OK)
	{
		return result;
	}

	m_pDescriptorSet->Update();

	return V3D_OK;
}

void MeshManager::Finalize()
{
	if (m_MeshMap.empty() == false)
	{
		MeshManager::MeshMap::iterator it_begin = m_MeshMap.begin();
		MeshManager::MeshMap::iterator it_end = m_MeshMap.end();
		MeshManager::MeshMap::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			it->second->Dispose();
		}

		m_MeshMap.clear();
	}

	SAFE_RELEASE(m_pDescriptorSet);
	SAFE_RELEASE(m_pUniformBuffer);
	SAFE_RELEASE(m_pMemory);

	m_pGraphicsManager = nullptr;
	m_pTextureManager = nullptr;
	m_pMaterialManager = nullptr;
}

V3D_RESULT MeshManager::BeginUpdate()
{
	return m_pMemory->BeginMap();
}

V3D_RESULT MeshManager::EndUpdate()
{
	return m_pMemory->EndMap();
}

MeshPtr MeshManager::Load(const wchar_t* pFilePath, const Mesh::LoadDesc& loadDesc)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	MeshManager::MeshMap::iterator it_mesh = m_MeshMap.find(pFilePath);
	if (it_mesh != m_MeshMap.end())
	{
		return it_mesh->second;
	}

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, this);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	result = mesh->LoadFromFile(pFilePath, loadDesc);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	m_NamingService.Add(pFilePath, mesh->m_Name);
	m_MeshMap[pFilePath] = mesh;

	return std::move(mesh);
}

void MeshManager::GetDescriptorSet(IV3DDescriptorSet** ppDescriptorSet)
{
	SAFE_ADD_REF(m_pDescriptorSet);
	*ppDescriptorSet = m_pDescriptorSet;
}

void MeshManager::RetainUniformDynamicOffset(uint32_t* pDynamicOffset)
{
	if (m_UnuseUniformDynamicOffsets.empty() == false)
	{
		*pDynamicOffset = m_UnuseUniformDynamicOffsets.back();
		m_UnuseUniformDynamicOffsets.pop_back();
	}
	else
	{
		*pDynamicOffset = m_InuseDynamicOffsetCount * m_UniformStride;
		m_InuseDynamicOffsetCount++;
	}
}

void MeshManager::ReleaseUniformDynamicOffset(uint32_t dynamicOffset)
{
	ASSERT(m_InuseDynamicOffsetCount > 0);

	m_UnuseUniformDynamicOffsets.push_back(dynamicOffset);
	m_InuseDynamicOffsetCount--;
}

void MeshManager::Add(const wchar_t* pName, MeshPtr mesh)
{
	m_NamingService.Add(pName, mesh->m_Name);

	ASSERT(m_MeshMap.find(mesh->m_Name) == m_MeshMap.end());
	m_MeshMap[mesh->m_Name] = mesh;
}
