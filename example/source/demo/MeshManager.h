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
	IV3DResourceMemory* m_pMemory;
	IV3DBuffer* m_pUniformBuffer;
	IV3DDescriptorSet* m_pDescriptorSet;
	uint32_t m_UniformStride;
	std::vector<uint32_t> m_UnuseUniformDynamicOffsets;
	uint32_t m_InuseDynamicOffsetCount;

	// ----------------------------------------------------------------------------------------------------

	void GetDescriptorSet(IV3DDescriptorSet** ppDescriptorSet);
	void RetainUniformDynamicOffset(uint32_t* pDynamicOffset);
	void ReleaseUniformDynamicOffset(uint32_t dynamicOffset);

	void Add(const wchar_t* pName, MeshPtr mesh);

	friend class Mesh;
};
