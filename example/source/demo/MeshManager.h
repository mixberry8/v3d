#pragma once

#include "StaticMesh.h"
#include "SkeletalMesh.h"
#include "NamingService.h"
#include "DynamicOffsetHeap.h"

class GraphicsManager;
class TextureManager;
class MaterialManager;

class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, MaterialManager* pMaterialManager, uint32_t maxUniform);
	void Finalize();

	V3D_RESULT BeginUpdate();
	V3D_RESULT EndUpdate();

	StaticMeshPtr ImportStatic(const wchar_t* pFilePath, const MeshImportDesc& importDesc);
	SkeletalMeshPtr ImportSkeletal(const wchar_t* pFilePath, const MeshImportDesc& importDesc);

	StaticMeshPtr LoadStatic(const wchar_t* pFilePath);
	SkeletalMeshPtr LoadSkeletal(const wchar_t* pFilePath);

private:
	typedef std::map<std::wstring, std::shared_ptr<Mesh>> MeshMap;

	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	MaterialManager* m_pMaterialManager;

	NamingService m_NamingService;
	MeshMap m_MeshMap;

	IV3DResourceMemory* m_pUniformMemory;
	IV3DBuffer* m_pUniformBuffer;
	DynamicOffsetHeap m_UniformOffsetHeap;
	IV3DDescriptorSet* m_pDescriptorSet;

	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT CreateDescriptorSet(IV3DDescriptorSet** ppDescriptorSet, DynamicOffsetHeap::Handle* pHandle);
	void ReleaseDescriptorSet(DynamicOffsetHeap::Handle* pHandle);

	void Add(const wchar_t* pName, std::shared_ptr<Mesh> mesh);

	friend class StaticMesh;
	friend class SkeletalMesh;
};
