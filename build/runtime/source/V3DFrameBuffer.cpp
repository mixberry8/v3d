#include "V3DFrameBuffer.h"
#include "V3DDevice.h"
#include "V3DRenderPass.h"
#include "V3DImageView.h"
#ifdef _DEBUG
#include "V3DImage.h"
#endif //_SEBUG

/***************************/
/* public - V3DFrameBuffer */
/***************************/

V3DFrameBuffer* V3DFrameBuffer::Create()
{
	return V3D_NEW_T(V3DFrameBuffer);
}

V3D_RESULT V3DFrameBuffer::Initialize(IV3DDevice* pDevice, IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, const wchar_t* pDebugName)
{
	V3D_ASSERT(attachmentCount > 0);
	V3D_ASSERT(ppAttachments != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));
	m_pRenderPass = V3D_TO_ADD_REF(static_cast<V3DRenderPass*>(pRenderPass));

	// ----------------------------------------------------------------------------------------------------
	// フレームバッファを作成
	// ----------------------------------------------------------------------------------------------------

	const V3DRenderPass::Source& renderPassSource = m_pRenderPass->GetSource();

#ifdef _DEBUG
	if (renderPassSource.debug.attachments.size() != attachmentCount)
	{
		V3D_LOG_ERROR(Log_Error_MismatchFrameBufferAttachmentCount, V3D_SAFE_NAME(this, pDebugName), m_pRenderPass->GetDebugName());
		return V3D_ERROR_FAIL;
	}

	bool debugSizeError = false;
	uint32_t debugErrorCount = 0;
#endif //_DEBUG

	STLVector<VkImageView> vkImageViews;
	vkImageViews.reserve(attachmentCount);

	m_ImageViews.reserve(attachmentCount);

	const V3DImageView::Source& imageViewSource = static_cast<V3DImageView*>(ppAttachments[0])->GetSource();

	for (uint32_t i = 0; i < attachmentCount; i++)
	{
		V3DImageView* pImageView = static_cast<V3DImageView*>(ppAttachments[i]);
		const V3DImageView::Source& compImageViewSource = pImageView->GetSource();

#ifdef _DEBUG
		if ((imageViewSource.extent.width != compImageViewSource.extent.width) ||
			(imageViewSource.extent.height != compImageViewSource.extent.height) ||
			(imageViewSource.extent.depth != compImageViewSource.extent.depth) ||
			(imageViewSource.imageSubresourceRange.layerCount != compImageViewSource.imageSubresourceRange.layerCount))
		{
			if (debugSizeError == false)
			{
				V3D_LOG_ERROR(Log_Error_MismatchFrameBufferAttachmentSize, V3D_SAFE_NAME(this, pDebugName));
				debugSizeError = true;
				debugErrorCount++;
			}
		}

		const V3DImageDesc& debugImageDesc = pImageView->GetInternalImagePtr()->GetDesc();
		if ((renderPassSource.debug.attachments[i].format != debugImageDesc.format) ||
			(renderPassSource.debug.attachments[i].samples != debugImageDesc.samples))
		{
			V3D_LOG_ERROR(Log_Error_MismatchFrameBufferAttachmentFormatOrSamples, V3D_SAFE_NAME(this, pDebugName), i);
			debugErrorCount++;
		}
#endif //_DEBUG

		vkImageViews.push_back(compImageViewSource.imageView);

		m_ImageViews.push_back(V3D_TO_ADD_REF(pImageView));
	}

#ifdef _DEBUG
	if (debugSizeError == true)
	{
		for (uint32_t i = 0; i < attachmentCount; i++)
		{
			V3DImageView* pImageView = static_cast<V3DImageView*>(ppAttachments[i]);
			const V3DImageView::Source& imageViewSource = pImageView->GetSource();

			V3D_LOG_ERROR(L"atacchment[%u] : width[%u] height[%u] layerCount[%u]", i, imageViewSource.extent.width, imageViewSource.extent.height, imageViewSource.imageSubresourceRange.layerCount);
		}
	}

	if (debugErrorCount > 0)
	{
		return V3D_ERROR_FAIL;
	}
#endif //_DEBUG

	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.renderPass = renderPassSource.renderPass;
	createInfo.attachmentCount = static_cast<uint32_t>(vkImageViews.size());
	createInfo.pAttachments = vkImageViews.data();
	createInfo.width = imageViewSource.extent.width;
	createInfo.height = imageViewSource.extent.height;
	createInfo.layers = imageViewSource.imageSubresourceRange.layerCount;

	VkResult vkResult = vkCreateFramebuffer(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.framebuffer);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Desc.attachmentWidth = imageViewSource.extent.width;
	m_Desc.attachmentHeight = imageViewSource.extent.height;
	m_Desc.attachmentDepth = imageViewSource.extent.depth;
	m_Desc.attachmentLayerCount = imageViewSource.imageSubresourceRange.layerCount;

	m_Source.extent.width = imageViewSource.extent.width;
	m_Source.extent.height = imageViewSource.extent.height;
	m_Source.layerCount = imageViewSource.imageSubresourceRange.layerCount;

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.framebuffer, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DFrameBuffer::Source& V3DFrameBuffer::GetSource() const
{
	return m_Source;
}

/***************************************************/
/* public override - IV3DFrameBuffer */
/***************************************************/

const V3DFrameBufferDesc& V3DFrameBuffer::GetDesc() const
{
	return m_Desc;
}

void V3DFrameBuffer::GetRenderPass(IV3DRenderPass** ppRenderPass)
{
	(*ppRenderPass) = V3D_TO_ADD_REF(m_pRenderPass);
}

uint32_t V3DFrameBuffer::GetAttachmentCount() const
{
	return static_cast<uint32_t>(m_ImageViews.size());
}

void V3DFrameBuffer::GetAttachment(uint32_t attachmentIndex, IV3DImageView** ppImage)
{
	(*ppImage) = V3D_TO_ADD_REF(m_ImageViews[attachmentIndex]);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DFrameBuffer::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DFrameBuffer::GetRefCount() const
{
	return m_RefCounter;
}

void V3DFrameBuffer::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DFrameBuffer::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DFrameBuffer);
	}
}

/****************************/
/* private - V3DFrameBuffer */
/****************************/

V3DFrameBuffer::V3DFrameBuffer() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Desc({}),
	m_pRenderPass(nullptr)
{
	m_Source.framebuffer = VK_NULL_HANDLE;
	m_Source.extent.width = 0;
	m_Source.extent.height = 0;
}

V3DFrameBuffer::~V3DFrameBuffer()
{
	if (m_Source.framebuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(m_pDevice->GetSource().device, m_Source.framebuffer, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.framebuffer);
	}

	if (m_ImageViews.empty() == false)
	{
		STLVector<IV3DImageView*>::iterator it_begin = m_ImageViews.begin();
		STLVector<IV3DImageView*>::iterator it_end = m_ImageViews.end();
		STLVector<IV3DImageView*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	V3D_RELEASE(m_pRenderPass);
	V3D_RELEASE(m_pDevice);
}
