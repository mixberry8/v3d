#pragma once

#include "V3DCommandBuffer.h"
#include "V3DSemaphore.h"

class V3DDevice;

class V3DQueue : public IV3DQueue
{
public:
	struct Source
	{
		VkQueue queue;
	};

	static V3DQueue* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, uint32_t family, VkQueue queue, const wchar_t* pDebugName);
	void Dispose();

	const V3DQueue::Source& GetSource() const;

	/*******************************/
	/* override - IV3DQueue */
	/*******************************/
	virtual uint32_t GetFamily() const override;
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) override;
	virtual V3D_RESULT Submit(uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores, IV3DFence* pFence) override;
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) override;
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores, IV3DFence* pFence) override;
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) override;
	virtual V3D_RESULT WaitIdle() override;

	/******************************/
	/* override - IV3DDeviceChild */
	/******************************/
	virtual void GetDevice(IV3DDevice** ppDevice) override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	struct Temp
	{
		STLVector<VkSemaphore> waitSemaphores;
		STLVector<VkPipelineStageFlags> waitDstStageMasks;
		STLVector<VkCommandBuffer> commandBuffers;
		STLVector<VkSemaphore> signalSemaphores;
	};

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	uint32_t m_Family;
	V3DQueue::Source m_Source;
	V3DQueue::Temp m_Temp;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DQueue();
	virtual ~V3DQueue();

	static inline VkPipelineStageFlags* Vulkan_CreatePipelineStageFlags(STLVector<VkPipelineStageFlags>& dst, uint32_t offset, uint32_t count, const V3DFlags* pFlags)
	{
		uint32_t totalCount = offset + count;
		if (totalCount == 0)
		{
			return false;
		}

		dst.resize(totalCount);

		const V3DFlags* pSrc = pFlags;
		const V3DFlags* pSrcEnd = pSrc + count;

		VkPipelineStageFlags* pDstBegin = dst.data();
		VkPipelineStageFlags* pDst = pDstBegin + offset;

		while (pSrc != pSrcEnd)
		{
			*pDst++ = ToVkPipelineStageFlags(*pSrc++);
		}

		return pDstBegin;
	}

	static inline VkSemaphore* Vulkan_CreateSemaphoreArray(STLVector<VkSemaphore>& dst, uint32_t offset, uint32_t count, IV3DSemaphore** ppSemaphores)
	{
		uint32_t totalCount = offset + count;
		if (totalCount == 0)
		{
			return nullptr;
		}

		dst.resize(offset + count);
		
		V3DSemaphore** ppSrc = reinterpret_cast<V3DSemaphore**>(ppSemaphores);
		V3DSemaphore** ppSrcEnd = ppSrc + count;

		VkSemaphore* pDstBegin = dst.data();
		VkSemaphore* pDst = pDstBegin + offset;

		while (ppSrc != ppSrcEnd)
		{
			*pDst++ = (*ppSrc++)->GetSource().semaphore;
		}

		return pDstBegin;
	}

	static inline VkCommandBuffer* Vulkan_CreateCommandBufferArray(STLVector<VkCommandBuffer>& dst, uint32_t count, IV3DCommandBuffer** ppCommandBuffers)
	{
		V3DCommandBuffer** ppSrc = reinterpret_cast<V3DCommandBuffer**>(ppCommandBuffers);
		V3DCommandBuffer** ppSrcEnd = ppSrc + count;

		dst.resize(count);
		VkCommandBuffer* pDstBegin = dst.data();
		VkCommandBuffer* pDst = pDstBegin;

		while (ppSrc != ppSrcEnd)
		{
			*pDst++ = (*ppSrc++)->GetSource().commandBuffer;
		}

		return pDstBegin;
	}

	V3D_DECLARE_ALLOCATOR
};