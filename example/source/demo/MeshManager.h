#pragma once

#include "Mesh.h"
#include "NamingService.h"
#include "ResourceHeap.h"

class GraphicsManager;
class TextureManager;
class MaterialManager;

class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, uint32_t maxMesh);
	void Finalize();

	V3D_RESULT BeginUpdate();
	V3D_RESULT EndUpdate();

	MeshPtr Load(const wchar_t* pFilePath, const Mesh::LoadDesc& loadDesc);

private:
	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	MaterialManager* m_pMaterialManager;
	uint32_t m_MaxMesh;

	typedef std::map<std::wstring, MeshPtr> MeshMap;
	MeshMap m_MeshMap;

public:
	NamingService m_NamingService;
	ResourceHeap m_UniformBufferHeap;

	V3D_RESULT CreateUniformBuffer(IV3DBuffer** ppBuffer, IV3DBufferView** ppBufferView, ResourceHeap::Handle* pHandle);
	void ReleaseUniformBuffer(ResourceHeap::Handle handle);

	// ----------------------------------------------------------------------------------------------------

	void Add(const wchar_t* pName, MeshPtr mesh);

	friend class Mesh;
};
