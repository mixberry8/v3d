#include "MeshManager.h"

MeshManager::MeshManager() :
	m_pGraphicsManager(nullptr),
	m_pTextureManager(nullptr),
	m_pMaterialManager(nullptr),
	m_pUniformMemory(nullptr),
	m_pUniformBuffer(nullptr),
	m_pDescriptorSet(nullptr)
{
}

MeshManager::~MeshManager()
{
}

V3D_RESULT MeshManager::Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, uint32_t maxUniform)
{
	m_pGraphicsManager = pGraphicsManager;
	m_pTextureManager = pTextureManager;
	m_pMaterialManager = pMaterialManager;

	// ----------------------------------------------------------------------------------------------------
	// ユニフォームバッファを作成
	// ----------------------------------------------------------------------------------------------------

	BufferSubresourceDesc uniformBufferSubresource;
	uniformBufferSubresource.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	uniformBufferSubresource.size = sizeof(StaticMesh::Uniform);
	uniformBufferSubresource.count = maxUniform;

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

	V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
	result = m_pGraphicsManager->GetDevicePtr()->CheckResourceMemoryProperty(memoryPropertyFlags, m_pUniformBuffer);
	if (result != V3D_OK)
	{
		memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(memoryPropertyFlags, m_pUniformBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	m_pUniformBuffer->GetResourceMemory(&m_pUniformMemory);

	m_UniformOffsetHeap.Initialize(maxUniform, TO_UI32(uniformBufferMemoryLayout.stride));

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	result = pGraphicsManager->CreateDescriptorSet(GRAPHICS_PIPELINE_TYPE_GEOMETRY, GRAPHICS_DESCRIPTOR_SET_TYPE_MESH, &m_pDescriptorSet);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pDescriptorSet->SetBuffer(0, m_pUniformBuffer, 0, sizeof(StaticMesh::Uniform));
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
	SAFE_RELEASE(m_pUniformMemory);

	m_pGraphicsManager = nullptr;
	m_pTextureManager = nullptr;
	m_pMaterialManager = nullptr;
}

V3D_RESULT MeshManager::BeginUpdate()
{
	return m_pUniformMemory->BeginMap();
}

V3D_RESULT MeshManager::EndUpdate()
{
	return m_pUniformMemory->EndMap();
}

StaticMeshPtr MeshManager::ImportStatic(const wchar_t* pFilePath, const MeshImportDesc& importDesc)
{
	std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>();

	MeshManager::MeshMap::iterator it_mesh = m_MeshMap.find(pFilePath);
	if (it_mesh != m_MeshMap.end())
	{
		return std::static_pointer_cast<StaticMesh>(it_mesh->second);
	}

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, this);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	std::wstring filePath;
	CreateFilePath(pFilePath, filePath);

	result = mesh->Import(filePath.c_str(), importDesc);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	m_NamingService.Add(pFilePath, mesh->m_Name);
	m_MeshMap[pFilePath] = mesh;

	return std::move(mesh);
}

SkeletalMeshPtr MeshManager::ImportSkeletal(const wchar_t* pFilePath, const MeshImportDesc& importDesc)
{
	SkeletalMeshPtr mesh = std::make_shared<SkeletalMesh>();

	MeshManager::MeshMap::iterator it_mesh = m_MeshMap.find(pFilePath);
	if (it_mesh != m_MeshMap.end())
	{
		return std::static_pointer_cast<SkeletalMesh>(it_mesh->second);
	}

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, this);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	std::wstring filePath;
	CreateFilePath(pFilePath, filePath);

	result = mesh->Import(filePath.c_str(), importDesc);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	m_NamingService.Add(pFilePath, mesh->m_Name);
	m_MeshMap[pFilePath] = mesh;

	return std::move(mesh);
}

StaticMeshPtr MeshManager::LoadStatic(const wchar_t* pFilePath)
{
	StaticMeshPtr mesh = std::make_shared<StaticMesh>();

	MeshManager::MeshMap::iterator it_mesh = m_MeshMap.find(pFilePath);
	if (it_mesh != m_MeshMap.end())
	{
		return std::static_pointer_cast<StaticMesh>(it_mesh->second);
	}

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, this);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	std::wstring filePath;
	CreateFilePath(pFilePath, filePath);

	if (mesh->Load(filePath.c_str()) == false)
	{
		return nullptr;
	}

	m_NamingService.Add(pFilePath, mesh->m_Name);
	m_MeshMap[pFilePath] = mesh;

	return std::move(mesh);
}

SkeletalMeshPtr MeshManager::LoadSkeletal(const wchar_t* pFilePath)
{
	SkeletalMeshPtr mesh = std::make_shared<SkeletalMesh>();

	MeshManager::MeshMap::iterator it_mesh = m_MeshMap.find(pFilePath);
	if (it_mesh != m_MeshMap.end())
	{
		return std::static_pointer_cast<SkeletalMesh>(it_mesh->second);
	}

	V3D_RESULT result = mesh->Initialize(m_pGraphicsManager, m_pTextureManager, m_pMaterialManager, this);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	std::wstring filePath;
	CreateFilePath(pFilePath, filePath);

	if(mesh->Load(filePath.c_str()) == false)
	{
		return nullptr;
	}

	m_NamingService.Add(pFilePath, mesh->m_Name);
	m_MeshMap[pFilePath] = mesh;

	return std::move(mesh);
}

V3D_RESULT MeshManager::CreateDescriptorSet(IV3DDescriptorSet** ppDescriptorSet, DynamicOffsetHeap::Handle* pHandle)
{
	if (m_UniformOffsetHeap.Retain(pHandle) == false)
	{
		return V3D_ERROR_FAIL;
	}

	SAFE_ADD_REF(m_pDescriptorSet);
	*ppDescriptorSet = m_pDescriptorSet;

	return V3D_OK;
}

void MeshManager::ReleaseDescriptorSet(DynamicOffsetHeap::Handle* pHandle)
{
	m_UniformOffsetHeap.Release(pHandle);
}

void MeshManager::Add(const wchar_t* pName, std::shared_ptr<Mesh> mesh)
{
	m_NamingService.Add(pName, mesh->m_Name);

	ASSERT(m_MeshMap.find(mesh->m_Name) == m_MeshMap.end());
	m_MeshMap[mesh->m_Name] = mesh;
}
