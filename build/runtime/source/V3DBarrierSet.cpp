#include "V3DBarrierSet.h"
#include "V3DDevice.h"
#include "V3DBuffer.h"
#include "V3DBufferView.h"
#include "V3DImage.h"
#include "V3DImageView.h"

/**************************/
/* public - V3DBarrierSet */
/**************************/

V3DBarrierSet* V3DBarrierSet::Create()
{
	return V3D_NEW_T(V3DBarrierSet);
}

V3D_RESULT V3DBarrierSet::Initialize(IV3DDevice* pDevice, const V3DBarrierSetDesc& desc, const wchar_t* pDebugName)
{
	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	m_Source.srcStageMask = ToVkPipelineStageFlags(desc.pipeline.srcStageMask);
	m_Source.dstStageMask = ToVkPipelineStageFlags(desc.pipeline.dstStageMask);
	m_Source.dependencyFlags = ToVkDependencyFlags(desc.pipeline.dependencyFlags);

	m_Source.bufferMemoryBarrierCount = desc.bufferMemoryCount + desc.bufferViewMemoryCount;
	if (m_Source.bufferMemoryBarrierCount > 0)
	{
		m_Source.pBufferMemoryBarriers = static_cast<VkBufferMemoryBarrier*>(V3D_MALLOC(sizeof(VkBufferMemoryBarrier) * m_Source.bufferMemoryBarrierCount));
		if (m_Source.pBufferMemoryBarriers == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		VkBufferMemoryBarrier* pDst = m_Source.pBufferMemoryBarriers;

		if (desc.bufferMemoryCount > 0)
		{
			const V3DBufferMemoryBarrier* pSrc = desc.pBufferMemories;
			const V3DBufferMemoryBarrier* pSrcEnd = pSrc + desc.bufferMemoryCount;

			m_Buffers.reserve(desc.bufferMemoryCount);

			while (pSrc != pSrcEnd)
			{
				const V3DBuffer::Source& source = static_cast<V3DBuffer*>(pSrc->pBuffer)->GetSource();

				pDst->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
				pDst->pNext = nullptr;
				pDst->srcAccessMask = ToVkAccessFlags(pSrc->srcAccessMask);
				pDst->dstAccessMask = ToVkAccessFlags(pSrc->dstAccessMask);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->buffer = source.buffer;
				pDst->offset = pSrc->offset;
				pDst->size = (pSrc->size == 0)? ( source.memorySize - pSrc->offset) : pSrc->size;

				m_Buffers.push_back(V3D_TO_ADD_REF(pSrc->pBuffer));

				pSrc++;
				pDst++;
			}
		}

		if (desc.bufferViewMemoryCount > 0)
		{
			const V3DBufferViewMemoryBarrier* pSrc = desc.pBufferViewMemories;
			const V3DBufferViewMemoryBarrier* pSrcEnd = pSrc + desc.bufferViewMemoryCount;

			m_BufferViews.reserve(desc.bufferViewMemoryCount);

			while (pSrc != pSrcEnd)
			{
				const V3DBufferView::Source& source = static_cast<V3DBufferView*>(pSrc->pBufferView)->GetSource();

				pDst->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
				pDst->pNext = nullptr;
				pDst->srcAccessMask = ToVkAccessFlags(pSrc->srcAccessMask);
				pDst->dstAccessMask = ToVkAccessFlags(pSrc->dstAccessMask);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->buffer = source.buffer;
				pDst->offset = source.offset;
				pDst->size = source.size;

				m_BufferViews.push_back(V3D_TO_ADD_REF(pSrc->pBufferView));

				pSrc++;
				pDst++;
			}
		}
	}

	m_Source.imageMemoryBarrierCount = desc.imageMemoryCount + desc.imageViewMemoryCount;
	if (m_Source.imageMemoryBarrierCount > 0)
	{
		m_Source.pImageMemoryBarriers = static_cast<VkImageMemoryBarrier*>(V3D_MALLOC(sizeof(VkImageMemoryBarrier) * m_Source.imageMemoryBarrierCount));
		if (m_Source.pImageMemoryBarriers == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		VkImageMemoryBarrier* pDst = m_Source.pImageMemoryBarriers;

		if (desc.imageMemoryCount > 0)
		{
			const V3DImageMemoryBarrier* pSrc = desc.pImageMemories;
			const V3DImageMemoryBarrier* pSrcEnd = pSrc + desc.imageMemoryCount;

			m_Images.reserve(desc.imageMemoryCount);

			while (pSrc != pSrcEnd)
			{
				const V3DImage::Source& source = static_cast<V3DImage*>(pSrc->pImage)->GetSource();

				pDst->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				pDst->pNext = nullptr;
				pDst->srcAccessMask = ToVkAccessFlags(pSrc->srcAccessMask);
				pDst->dstAccessMask = ToVkAccessFlags(pSrc->dstAccessMask);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->oldLayout = ToVkImageLayout(pSrc->srcLayout);
				pDst->newLayout = ToVkImageLayout(pSrc->dstLayout);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->image = source.image;
				pDst->subresourceRange.aspectMask = source.aspectFlags;
				pDst->subresourceRange.baseMipLevel = pSrc->baseLevel;
				pDst->subresourceRange.levelCount = (pSrc->levelCount == 0) ? (source.levelCount - pSrc->baseLevel) : pSrc->levelCount;
				pDst->subresourceRange.baseArrayLayer = pSrc->baseLayer;
				pDst->subresourceRange.layerCount = (pSrc->layerCount == 0) ? (source.layerCount - pSrc->baseLayer) : pSrc->layerCount;

				m_Images.push_back(V3D_TO_ADD_REF(pSrc->pImage));

				pSrc++;
				pDst++;
			}
		}

		if (desc.imageViewMemoryCount > 0)
		{
			const V3DImageViewMemoryBarrier* pSrc = desc.pImageViewMemories;
			const V3DImageViewMemoryBarrier* pSrcEnd = pSrc + desc.imageViewMemoryCount;

			m_ImageViews.reserve(desc.imageViewMemoryCount);

			while (pSrc != pSrcEnd)
			{
				const V3DImageView::Source& source = static_cast<V3DImageView*>(pSrc->pImageView)->GetSource();

				pDst->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				pDst->pNext = nullptr;
				pDst->srcAccessMask = ToVkAccessFlags(pSrc->srcAccessMask);
				pDst->dstAccessMask = ToVkAccessFlags(pSrc->dstAccessMask);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->oldLayout = ToVkImageLayout(pSrc->srcLayout);
				pDst->newLayout = ToVkImageLayout(pSrc->dstLayout);
				pDst->srcQueueFamilyIndex = pSrc->srcQueueFamily;
				pDst->dstQueueFamilyIndex = pSrc->dstQueueFamily;
				pDst->image = source.image;
				pDst->subresourceRange = source.imageSubresourceRange;

				m_ImageViews.push_back(V3D_TO_ADD_REF(pSrc->pImageView));

				pSrc++;
				pDst++;
			}
		}
	}

	return V3D_OK;
}

const V3DBarrierSet::Source& V3DBarrierSet::GetSource() const
{
	return m_Source;
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DBarrierSet::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DBarrierSet::GetRefCount() const
{
	return m_RefCounter;
}

void V3DBarrierSet::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DBarrierSet::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DBarrierSet);
	}
}

/***************************/
/* private - V3DBarrierSet */
/***************************/

V3DBarrierSet::V3DBarrierSet() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Source({})
{
}

V3DBarrierSet::~V3DBarrierSet()
{
	if (m_Buffers.empty() == false)
	{
		auto it_begin = m_Buffers.begin();
		auto it_end = m_Buffers.end();

		for (auto it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	if (m_BufferViews.empty() == false)
	{
		auto it_begin = m_BufferViews.begin();
		auto it_end = m_BufferViews.end();

		for (auto it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	if (m_Images.empty() == false)
	{
		auto it_begin = m_Images.begin();
		auto it_end = m_Images.end();

		for (auto it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	if (m_ImageViews.empty() == false)
	{
		auto it_begin = m_ImageViews.begin();
		auto it_end = m_ImageViews.end();

		for (auto it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	if (m_Source.pBufferMemoryBarriers != nullptr)
	{
		V3D_FREE(m_Source.pBufferMemoryBarriers);
	}

	if (m_Source.pImageMemoryBarriers != nullptr)
	{
		V3D_FREE(m_Source.pImageMemoryBarriers);
	}

	V3D_RELEASE(m_pDevice);
}
