#pragma once

#include "Material.h"
#include "DynamicOffsetHeap.h"

class GraphicsManager;
class TextureManager;

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, TextureManager* pTextureManager, uint32_t maxMaterial);
	void Finalize();

	MaterialPtr Create(const wchar_t* pFullName, const wchar_t* pName);

private:
	enum DUMMY_IMAGE_TYPE
	{
		DUMMY_IMAGE_TYPE_DIFFUSE = 0,
		DUMMY_IMAGE_TYPE_SPECULAR = 1,
		DUMMY_IMAGE_TYPE_MASK = 2,
		DUMMY_IMAGE_TYPE_NORMAL = 3,
	};
	static auto constexpr DUMMY_IMAGE_TYPE_COUNT = 4;

	static auto constexpr DescriptorPoolSize = 256;
	static auto constexpr SamplerFilterCount = 2;
	static auto constexpr SamplerAddressModeCount = 4;

	GraphicsManager* m_pGraphicsManager;
	TextureManager* m_pTextureManager;
	uint32_t m_MaxMaterial;

	IV3DResourceMemory* m_pUniformMemory;
	IV3DBuffer* m_pUniformBuffer;
	DynamicOffsetHeap m_UniformOffsetHeap;

	Array1<IV3DImageView*, MaterialManager::DUMMY_IMAGE_TYPE_COUNT> m_pDummyImageViews;
	Array2<IV3DSampler*, MaterialManager::SamplerFilterCount, MaterialManager::SamplerAddressModeCount> m_pSamplers;

	typedef std::map<std::wstring, MaterialPtr> MaterialMap;
	std::map<std::wstring, MaterialPtr> m_MaterialMap;

	MaterialPtr CreateFromSerial(const wchar_t* pFullName, const Material::SerialData* pData);

	V3D_RESULT CreateDummyImage(IV3DImage** ppImage);

	// ----------------------------------------------------------------------------------------------------

	V3D_RESULT CreateDescriptorSet(V3D_FILTER filter, V3D_ADDRESS_MODE addressMode, IV3DDescriptorSet** ppDescriptorSet, DynamicOffsetHeap::Handle* pHandle);
	void ReleaseDescriptorSet(DynamicOffsetHeap::Handle* pHandle);

	IV3DImageView* GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE type);
	IV3DSampler* GetSamplerPtr(V3D_FILTER filter, V3D_ADDRESS_MODE addressMode);

	friend class Material;
	friend class Mesh;
};
