#pragma once

#include "Material.h"

class GraphicsManager;

class MaterialManager
{
public:
	MaterialManager();
	~MaterialManager();

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager, uint32_t maxMaterial);
	void Finalize();

	MaterialPtr Create(const wchar_t* pName);

private:
	enum DUMMY_IMAGE_TYPE
	{
		DUMMY_IMAGE_TYPE_DIFFUSE = 0,
		DUMMY_IMAGE_TYPE_NORMAL = 1,
	};
	static auto constexpr DUMMY_IMAGE_TYPE_COUNT = 2;

	static auto constexpr DescriptorPoolSize = 256;
	static auto constexpr SamplerFilterCount = 2;
	static auto constexpr SamplerAddressModeCount = 4;

	GraphicsManager* m_pGraphicsManager;
	uint32_t m_MaxMaterial;

	IV3DResourceMemory* m_pUniformMemory;
	uint32_t m_UniformStride;
	IV3DBuffer* m_pUniformBuffer;
	std::vector<uint32_t> m_UnuseUniformDynamicOffsets;
	uint32_t m_InuseDynamicOffsetCount;

	Array1<IV3DImageView*, MaterialManager::DUMMY_IMAGE_TYPE_COUNT> m_pDummyImageViews;
	Array2<IV3DSampler*, MaterialManager::SamplerFilterCount, MaterialManager::SamplerAddressModeCount> m_pSamplers;

	typedef std::map<std::wstring, MaterialPtr> MaterialMap;
	std::map<std::wstring, MaterialPtr> m_MaterialMap;

	V3D_RESULT CreateDummyImage(IV3DImage** ppImage);

	IV3DImageView* GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE type);
	IV3DSampler* GetSamplerPtr(V3D_FILTER filter, V3D_ADDRESS_MODE addressMode);

	// ----------------------------------------------------------------------------------------------------

	void GetUniformBuffer(IV3DBuffer** ppBuffer);
	void RetainUniformDynamicOffset(uint32_t* pDynamicOffset);
	void ReleaseUniformDynamicOffset(uint32_t dynamicOffset);

	friend class Material;
};
