#include "V3DImage.h"
#include "V3DDevice.h"
#include "V3DResourceMemory.h"

/*********************/
/* public - V3DImage */
/*********************/

V3DImage* V3DImage::Create()
{
	return V3D_NEW_T(V3DImage);
}

V3D_RESULT V3DImage::Initialize(IV3DDevice* pDevice, const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	// ----------------------------------------------------------------------------------------------------

	const V3DDevice::Source& deviceSource = m_pDevice->GetSource();

	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext = nullptr;
	imageCreateInfo.flags = ToVkImageCreateFlags(imageDesc.usageFlags);
	imageCreateInfo.imageType = ToVkImageType(imageDesc.type);
	imageCreateInfo.format = ToVkFormat(imageDesc.format);
	imageCreateInfo.extent.width = imageDesc.width;
	imageCreateInfo.extent.height = imageDesc.height;
	imageCreateInfo.extent.depth = imageDesc.depth;
	imageCreateInfo.mipLevels = imageDesc.levelCount;
	imageCreateInfo.arrayLayers = imageDesc.layerCount;
	imageCreateInfo.samples = ToVkSampleCountFlagBits(imageDesc.samples);
	imageCreateInfo.tiling = ToVkImageTiling(imageDesc.tiling);
	imageCreateInfo.usage = ToVkImageUsageFlags(imageDesc.usageFlags);
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.pQueueFamilyIndices = nullptr;
	imageCreateInfo.initialLayout = ToVkImageLayout(initialLayout);

	VkResult vkResult = vkCreateImage(deviceSource.device, &imageCreateInfo, nullptr, &m_Source.image);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	// ----------------------------------------------------------------------------------------------------
	// 記述を設定
	// ----------------------------------------------------------------------------------------------------

	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(deviceSource.device, m_Source.image, &memReqs);

	m_Desc = imageDesc;

	m_ResourceDesc.memoryTypeBits = memReqs.memoryTypeBits;
	m_ResourceDesc.memorySize = memReqs.size;
	m_ResourceDesc.memoryAlignment = memReqs.alignment;

	// ----------------------------------------------------------------------------------------------------
	// イメージアスペクトを決定
	// ----------------------------------------------------------------------------------------------------

	if ((m_Desc.format == V3D_FORMAT_D16_UNORM) || (m_Desc.format == V3D_FORMAT_X8_D24_UNORM) || (m_Desc.format == V3D_FORMAT_D32_SFLOAT))
	{
		m_Source.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (m_Desc.format == V3D_FORMAT_S8_UINT)
	{
		m_Source.aspectFlags = VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else if ((m_Desc.format == V3D_FORMAT_D16_UNORM_S8_UINT) || (m_Desc.format == V3D_FORMAT_D24_UNORM_S8_UINT) || (m_Desc.format == V3D_FORMAT_D32_SFLOAT_S8_UINT))
	{
		m_Source.aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		m_Source.aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	// ----------------------------------------------------------------------------------------------------
	// サブリソースのレイアウトを取得
	// ----------------------------------------------------------------------------------------------------

	if (m_Desc.tiling == V3D_IMAGE_TILING_LINEAR)
	{
		VkImageSubresource vkImageSubresource{};
		vkImageSubresource.aspectMask = m_Source.aspectFlags;

		m_SubresourceLayouts.reserve(m_Desc.levelCount * m_Desc.layerCount);

		for (uint32_t i = 0; i < m_Desc.levelCount; i++)
		{
			vkImageSubresource.mipLevel = i;
			for (uint32_t j = 0; j < m_Desc.layerCount; j++)
			{
				vkImageSubresource.arrayLayer = j;

				VkSubresourceLayout vkSubresourceLayout;
				vkGetImageSubresourceLayout(deviceSource.device, m_Source.image, &vkImageSubresource, &vkSubresourceLayout);

				V3DImageSubresourceLayout subresourceLayout;
				subresourceLayout.offset = vkSubresourceLayout.offset;
				subresourceLayout.size = vkSubresourceLayout.size;
				subresourceLayout.rowPitch = vkSubresourceLayout.rowPitch;
				subresourceLayout.layerPitch = vkSubresourceLayout.arrayPitch;
				subresourceLayout.depthPitch = vkSubresourceLayout.depthPitch;

				m_SubresourceLayouts.push_back(subresourceLayout);
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

#ifdef _DEBUG

#ifdef _WIN64
	m_DebugImageAddr = reinterpret_cast<uint64_t>(m_Source.image);
#else //_WIN64
	m_DebugImageAddr = static_cast<uint32_t>(m_Source.image);
#endif //_WIN64

#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

/***********************************/
/* public override - IV3DImageBase */
/***********************************/

const IV3DImageBase::Source& V3DImage::GetSource() const
{
	return m_Source;
}

bool V3DImage::CheckBindMemory() const
{
	return (m_pMemory != nullptr);
}

V3D_RESULT V3DImage::BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset)
{
	V3D_ASSERT(pMemory != nullptr);

	m_pMemory = V3D_TO_ADD_REF(pMemory);
	m_Source.memoryOffset = memoryOffset;

#ifdef _DEBUG
	if (m_pMemory->Debug_CheckMemory(memoryOffset, m_ResourceDesc.memorySize) == false)
	{
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	VkResult vkResult = vkBindImageMemory(m_pDevice->GetSource().device, m_Source.image, m_pMemory->GetSource().deviceMemory, memoryOffset);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	return V3D_OK;
}

/*******************************/
/* public override - IV3DImage */
/*******************************/

const V3DImageDesc& V3DImage::GetDesc() const
{
	return m_Desc;
}

V3D_RESULT V3DImage::GetResourceMemory(IV3DResourceMemory** ppMemory)
{
	(*ppMemory) = V3D_TO_ADD_REF(m_pMemory);

	return V3D_OK;
}

const V3DImageSubresourceLayout& V3DImage::GetSubresourceLayout(uint32_t level, uint32_t layer) const
{
	if ((m_Desc.levelCount <= level) || (m_Desc.layerCount <= layer))
	{
		return V3DImage::DummySubresource;
	}

	return m_SubresourceLayouts[layer * level];
}

/**************************************/
/* public override - IV3DResourceDesc */
/**************************************/

const V3DResourceDesc& V3DImage::GetResourceDesc() const
{
	return m_ResourceDesc;
}

V3D_RESULT V3DImage::Map(uint64_t offset, uint64_t size, void** ppMemory)
{
	if ((m_ResourceDesc.memorySize <= offset) || ((size != V3D_WHOLE_SIZE) && (m_ResourceDesc.memorySize < (offset + size))) || (ppMemory == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	if (m_pMemory == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	return m_pMemory->Map(m_Source.memoryOffset + offset, (size == V3D_WHOLE_SIZE)? m_ResourceDesc.memorySize : size, ppMemory);
}

V3D_RESULT V3DImage::Unmap()
{
	return m_pMemory->Unmap();
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DImage::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DImage::GetRefCount() const
{
	return m_RefCounter;
}

void V3DImage::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DImage::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DImage);
	}
}

/**********************/
/* private - V3DImage */
/**********************/

V3DImage::V3DImage() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pMemory(nullptr),
	m_ResourceDesc({}),
	m_Desc({}),
	m_Source({})
{
	m_ResourceDesc.type = V3D_RESOURCE_TYPE_IMAGE;
	m_Source.image = VK_NULL_HANDLE;

#ifdef _DEBUG
	m_DebugImageAddr = 0;
#endif //_DEBUG
}

V3DImage::~V3DImage()
{
	if (m_Source.image != VK_NULL_HANDLE)
	{
		vkDestroyImage(m_pDevice->GetSource().device, m_Source.image, nullptr);
	}

	V3D_RELEASE(m_pMemory);
	V3D_RELEASE(m_pDevice);
}
