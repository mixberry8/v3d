#include "V3DImageView.h"
#include "V3DDevice.h"
#include "IV3DImageBase.h"

/*************************/
/* public - V3DImageView */
/*************************/

V3DImageView* V3DImageView::Create()
{
	return V3D_NEW_T(V3DImageView);
}

V3D_RESULT V3DImageView::Initialize(IV3DDevice* pDevice, IV3DImage* pImage, const V3DImageViewDesc& desc, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(pImage != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pImage = V3D_TO_ADD_REF(static_cast<IV3DImageBase*>(pImage));

	// ----------------------------------------------------------------------------------------------------

	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = m_pImage->GetSource().image;
	createInfo.viewType = ToVkImageViewType(desc.type);
	createInfo.format = ToVkFormat(m_pImage->GetDesc().format);
	createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	createInfo.subresourceRange.aspectMask = m_pImage->GetSource().aspectFlags;
	createInfo.subresourceRange.baseMipLevel = desc.baseLevel;
	createInfo.subresourceRange.levelCount = desc.levelCount;
	createInfo.subresourceRange.baseArrayLayer = desc.baseLayer;
	createInfo.subresourceRange.layerCount = desc.layerCount;

	VkDevice vkDevice = m_pDevice->GetSource().device;

	VkResult vkResult = vkCreateImageView(vkDevice, &createInfo, nullptr, &m_Source.imageView);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.imageView, V3D_DEBUG_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------

	m_Desc = desc;

	const V3DImageDesc& imageDesc = m_pImage->GetDesc();

	m_SubresourceSize.width = V3D_MAX(1, imageDesc.width / (desc.baseLevel + 1));
	m_SubresourceSize.height = V3D_MAX(1, imageDesc.height / (desc.baseLevel + 1));
	m_SubresourceSize.depth = imageDesc.depth;

	m_Source.image = createInfo.image;
	m_Source.imageSubresourceRange = createInfo.subresourceRange;
	m_Source.extent.width = m_SubresourceSize.width;
	m_Source.extent.height = m_SubresourceSize.height;
	m_Source.extent.depth = m_SubresourceSize.depth;

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

IV3DImageBase* V3DImageView::GetInternalImagePtr() const
{
	return m_pImage;
}

const V3DImageView::Source& V3DImageView::GetSource() const
{
	return m_Source;
}

/***********************************/
/* public override - IV3DImageView */
/***********************************/

const V3DImageViewDesc& V3DImageView::GetDesc() const
{
	return m_Desc;
}

void V3DImageView::GetImage(IV3DImage** ppImage)
{
	(*ppImage) = V3D_TO_ADD_REF(m_pImage);

}

const V3DSize3D& V3DImageView::GetImageSubresourceSize() const
{
	return m_SubresourceSize;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DImageView::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DImageView::GetRefCount() const
{
	return m_RefCounter;
}

void V3DImageView::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DImageView::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DImageView);
	}
}

/**************************/
/* private - V3DImageView */
/**************************/

V3DImageView::V3DImageView() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_pImage(nullptr),
	m_Desc({}),
	m_SubresourceSize({}),
	m_Source({})
{
	m_Source.imageView = VK_NULL_HANDLE;

#ifdef _DEBUG
	m_DebugImageAddr = 0;
#endif //_DEBUG
}

V3DImageView::~V3DImageView()
{
	if (m_Source.imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(m_pDevice->GetSource().device, m_Source.imageView, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.imageView);
	}

	V3D_RELEASE(m_pImage);
	V3D_RELEASE(m_pDevice);
}
