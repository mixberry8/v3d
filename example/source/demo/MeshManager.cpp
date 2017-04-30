#include "MeshManager.h"

MeshManager::MeshManager() :
	m_pGraphicsManager(nullptr),
	m_pTextureManager(nullptr),
	m_pMaterialManager(nullptr)
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

	m_UniformBufferHeap.Dispose();

	m_pGraphicsManager = nullptr;
	m_pTextureManager = nullptr;
	m_pMaterialManager = nullptr;
}

V3D_RESULT MeshManager::BeginUpdate()
{
	return m_UniformBufferHeap.GetMemory()->BeginMap();
}

V3D_RESULT MeshManager::EndUpdate()
{
	return m_UniformBufferHeap.GetMemory()->EndMap();
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

V3D_RESULT MeshManager::CreateUniformBuffer(IV3DBuffer** ppBuffer, IV3DBufferView** ppBufferView, ResourceHeap::Handle* pHandle)
{
	V3DBufferSubresourceDesc uniformSubresources[1];
	uniformSubresources[0].usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	uniformSubresources[0].size = sizeof(Mesh::Uniform);

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(_countof(uniformSubresources), uniformSubresources, ppBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	if (m_UniformBufferHeap.IsInitialized() == false)
	{
		V3DFlags memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE | V3D_MEMORY_PROPERTY_HOST_COHERENT;
		if (m_pGraphicsManager->GetDevicePtr()->CheckResourceMemoryProperty(memoryPropertyFlags, 1, reinterpret_cast<IV3DResource**>(ppBuffer)) != V3D_OK)
		{
			memoryPropertyFlags = V3D_MEMORY_PROPERTY_HOST_VISIBLE;
		}

		result = m_UniformBufferHeap.Initialize(m_pGraphicsManager->GetDevicePtr(), memoryPropertyFlags, (*ppBuffer)->GetResourceDesc().memorySize * m_MaxMesh);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	result = m_UniformBufferHeap.Bind((*ppBuffer), pHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->CreateBufferView((*ppBuffer), 0, V3D_FORMAT_UNDEFINED, ppBufferView);
	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

void MeshManager::ReleaseUniformBuffer(ResourceHeap::Handle handle)
{
	m_UniformBufferHeap.Unbind(handle);
}

void MeshManager::Add(const wchar_t* pName, MeshPtr mesh)
{
	m_NamingService.Add(pName, mesh->m_Name);

	ASSERT(m_MeshMap.find(mesh->m_Name) == m_MeshMap.end());
	m_MeshMap[mesh->m_Name] = mesh;
}
