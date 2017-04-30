#include "MaterialManager.h"
#include "GraphicsManager.h"

MaterialManager::MaterialManager() :
	m_pGraphicsManager(nullptr),
	m_MaxMaterial(0)
{
	for (uint32_t i = 0; i < MaterialManager::DUMMY_IMAGE_TYPE_COUNT; i++)
	{
		m_pDummyImageViews[i] = nullptr;
	}

	for (uint32_t i = 0; i < MaterialManager::SamplerFilterCount; i++)
	{
		for (uint32_t j = 0; j < MaterialManager::SamplerAddressModeCount; j++)
		{
			m_pSamplers[i][j] = nullptr;
		}
	}
}

MaterialManager::~MaterialManager()
{
}

V3D_RESULT MaterialManager::Initialize(GraphicsManager* pGraphicsManager, uint32_t maxMaterial)
{
	m_pGraphicsManager = pGraphicsManager;
	m_MaxMaterial = maxMaterial;

	// ------------------------------------------------------------------------------------------
	// ダミーイメージを作成
	// ------------------------------------------------------------------------------------------

	V3DImageDesc imageDesc;
	imageDesc.type = V3D_IMAGE_TYPE_2D;
	imageDesc.format = V3D_FORMAT_R8G8B8A8_UNORM;
	imageDesc.width = 1;
	imageDesc.height = 1;
	imageDesc.depth = 1;
	imageDesc.levelCount = 1;
	imageDesc.layerCount = 1;
	imageDesc.samples = V3D_SAMPLE_COUNT_1;
	imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
	imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED;

	IV3DImage* pDummyAlbedoImage;
	V3D_RESULT result = CreateDummyImage(&pDummyAlbedoImage);
	if (result != V3D_OK)
	{
		return result;
	}

	IV3DImage* pDummyNormalImage;
	result = CreateDummyImage(&pDummyNormalImage);
	if (result != V3D_OK)
	{
		return result;
	}

	IV3DCommandBuffer* pCommandBuffer = m_pGraphicsManager->ResetCommandBuffer();
	if (pCommandBuffer == nullptr)
	{
		SAFE_RELEASE(pDummyNormalImage);
		SAFE_RELEASE(pDummyAlbedoImage);
		return V3D_ERROR_FAIL;
	}

	V3DBarrierImageDesc barrier{};
	barrier.dependencyFlags = 0;
	barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

	barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
	barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
	barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
	pCommandBuffer->BarrierImage(pDummyAlbedoImage, barrier);
	pCommandBuffer->BarrierImage(pDummyNormalImage, barrier);

	V3DClearValue clearValue{};

	clearValue.color.float32[0] = 1.0f;
	clearValue.color.float32[1] = 1.0f;
	clearValue.color.float32[2] = 1.0f;
	clearValue.color.float32[3] = 1.0f;
	pCommandBuffer->ClearImage(pDummyAlbedoImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, clearValue);

	clearValue.color.float32[0] = 1.0f;
	clearValue.color.float32[1] = 0.0f;
	clearValue.color.float32[2] = 1.0f;
	clearValue.color.float32[3] = 0.0f;
	pCommandBuffer->ClearImage(pDummyNormalImage, V3D_IMAGE_LAYOUT_TRANSFER_DST, clearValue);

	barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
	barrier.dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
	barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
	barrier.dstAccessMask = V3D_ACCESS_SHADER_READ;
	barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
	barrier.dstLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
	pCommandBuffer->BarrierImage(pDummyAlbedoImage, barrier);
	pCommandBuffer->BarrierImage(pDummyNormalImage, barrier);

	if (m_pGraphicsManager->FlushCommandBuffer() == false)
	{
		SAFE_RELEASE(pDummyNormalImage);
		SAFE_RELEASE(pDummyAlbedoImage);
		return V3D_ERROR_FAIL;
	}

	V3DImageViewDesc imageViewDesc;
	imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
	imageViewDesc.baseLevel = 0;
	imageViewDesc.levelCount = 1;
	imageViewDesc.baseLayer = 0;
	imageViewDesc.layerCount = 1;

	result = m_pGraphicsManager->GetDevicePtr()->CreateImageView(pDummyAlbedoImage, imageViewDesc, &m_pDummyImageViews[MaterialManager::DUMMY_IMAGE_TYPE_DIFFUSE]);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pDummyNormalImage);
		SAFE_RELEASE(pDummyAlbedoImage);
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->CreateImageView(pDummyNormalImage, imageViewDesc, &m_pDummyImageViews[MaterialManager::DUMMY_IMAGE_TYPE_NORMAL]);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pDummyNormalImage);
		SAFE_RELEASE(pDummyAlbedoImage);
		return result;
	}

	SAFE_RELEASE(pDummyNormalImage);
	SAFE_RELEASE(pDummyAlbedoImage);

	// ------------------------------------------------------------------------------------------
	// サンプラーを作成
	// ------------------------------------------------------------------------------------------

	const V3D_FILTER filterTable[MaterialManager::SamplerFilterCount] =
	{
		V3D_FILTER_NEAREST,
		V3D_FILTER_LINEAR,
	};

	const V3D_ADDRESS_MODE addressModeTable[MaterialManager::SamplerAddressModeCount] =
	{
		V3D_ADDRESS_MODE_REPEAT,
		V3D_ADDRESS_MODE_MIRRORED_REPEAT,
		V3D_ADDRESS_MODE_CLAMP_TO_EDGE,
		V3D_ADDRESS_MODE_CLAMP_TO_BORDER,
	};

	for (uint32_t i = 0; i < MaterialManager::SamplerFilterCount; i++)
	{
		for (uint32_t j = 0; j < MaterialManager::SamplerAddressModeCount; j++)
		{
			V3DSamplerDesc samplerDesc{};
			samplerDesc.magFilter = filterTable[i];
			samplerDesc.minFilter = filterTable[i];
			samplerDesc.mipmapMode = (filterTable[i] == V3D_FILTER_NEAREST) ? V3D_MIPMAP_MODE_NEAREST : V3D_MIPMAP_MODE_LINEAR;
			samplerDesc.addressModeU = addressModeTable[j];
			samplerDesc.addressModeV = addressModeTable[j];
			samplerDesc.addressModeW = addressModeTable[j];
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 1.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			result = m_pGraphicsManager->GetDevicePtr()->CreateSampler(samplerDesc, &m_pSamplers[i][j]);
			if (result != V3D_OK)
			{
				return result;
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void MaterialManager::Finalize()
{
	if (m_MaterialMap.empty() == false)
	{
		MaterialManager::MaterialMap::iterator it_begin = m_MaterialMap.begin();
		MaterialManager::MaterialMap::iterator it_end = m_MaterialMap.end();
		MaterialManager::MaterialMap::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			it->second->Dispose();
		}
	}

	for (uint32_t i = 0; i < MaterialManager::SamplerFilterCount; i++)
	{
		for (uint32_t j = 0; j < MaterialManager::SamplerAddressModeCount; j++)
		{
			SAFE_RELEASE(m_pSamplers[i][j]);
		}
	}

	for (uint32_t i = 0; i < MaterialManager::DUMMY_IMAGE_TYPE_COUNT; i++)
	{
		SAFE_RELEASE(m_pDummyImageViews[i]);
	}

	m_UniformBufferHeap.Dispose();

	m_pGraphicsManager = nullptr;
}

MaterialPtr MaterialManager::Create(const wchar_t* pName)
{
	MaterialManager::MaterialMap::iterator it = m_MaterialMap.find(pName);
	if (it != m_MaterialMap.end())
	{
		return it->second;
	}

	MaterialPtr material = std::make_shared<Material>();

	V3D_RESULT result = material->Initialize(m_pGraphicsManager, this, pName);
	if (result != V3D_OK)
	{
		return nullptr;
	}

	m_MaterialMap[pName] = material;

	return std::move(material);
}

V3D_RESULT MaterialManager::CreateDummyImage(IV3DImage** ppImage)
{
	V3DImageDesc imageDesc;
	imageDesc.type = V3D_IMAGE_TYPE_2D;
	imageDesc.format = V3D_FORMAT_R8G8B8A8_UNORM;
	imageDesc.width = 1;
	imageDesc.height = 1;
	imageDesc.depth = 1;
	imageDesc.levelCount = 1;
	imageDesc.layerCount = 1;
	imageDesc.samples = V3D_SAMPLE_COUNT_1;
	imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;
	imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED;

	IV3DImage* pDummyImage;
	
	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateImage(imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &pDummyImage);
	if (result != V3D_OK)
	{
		return result;
	}

	result = m_pGraphicsManager->GetDevicePtr()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, pDummyImage);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pDummyImage);
		return result;
	}

	(*ppImage) = pDummyImage;

	return V3D_OK;
}

IV3DImageView* MaterialManager::GetDummyImageViewPtr(MaterialManager::DUMMY_IMAGE_TYPE type)
{
	return m_pDummyImageViews[type];
}

IV3DSampler* MaterialManager::GetSamplerPtr(V3D_FILTER filter, V3D_ADDRESS_MODE addressMode)
{
	return m_pSamplers[filter][addressMode];
}

V3D_RESULT MaterialManager::CreateUniformBuffer(IV3DBuffer** ppBuffer, IV3DBufferView** ppBufferView, ResourceHeap::Handle* pHandle)
{
	V3DBufferSubresourceDesc subresources[1];
	subresources[0].usageFlags = V3D_BUFFER_USAGE_UNIFORM;
	subresources[0].size = sizeof(Material::Uniform);

	V3D_RESULT result = m_pGraphicsManager->GetDevicePtr()->CreateBuffer(_countof(subresources), subresources, ppBuffer);
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

		result = m_UniformBufferHeap.Initialize(m_pGraphicsManager->GetDevicePtr(), memoryPropertyFlags, (*ppBuffer)->GetResourceDesc().memorySize * m_MaxMaterial);
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

void MaterialManager::ReleaseUniformBuffer(ResourceHeap::Handle handle)
{
	m_UniformBufferHeap.Unbind(handle);
}
