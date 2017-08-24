#include "V3DQueue.h"
#include "V3DDevice.h"
#include "V3DFence.h"
#include "V3DSwapChain.h"

/****************************/
/* public - V3DQueue */
/****************************/

V3DQueue* V3DQueue::Create()
{
	return V3D_NEW_T(V3DQueue);
}

V3D_RESULT V3DQueue::Initialize(IV3DDevice* pDevice, uint32_t family, VkQueue queue, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = static_cast<V3DDevice*>(pDevice); // 循環参照になるので参照カウンタはインクリメントしない
	m_Family = family;
	m_Source.queue = queue;

	V3D_ADD_DEBUG_MEMORY_OBJECT(this, V3D_DEBUG_OBJECT_TYPE_QUEUE, V3D_SAFE_NAME(this, pDebugName));

	V3D_DEBUG_CODE(m_DebugName = V3D_SAFE_NAME(this, pDebugName));

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.queue, m_DebugName.c_str());

	return V3D_OK;
}

void V3DQueue::Dispose()
{
	if (m_Source.queue != VK_NULL_HANDLE)
	{
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.queue);
		m_Source.queue = VK_NULL_HANDLE;
	}
}

const V3DQueue::Source& V3DQueue::GetSource() const
{
	return m_Source;
}

/**************************************/
/* public override - IV3DQueue */
/**************************************/

uint32_t V3DQueue::GetFamily() const
{
	return m_Family;
}

V3D_RESULT V3DQueue::Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence)
{
#ifdef V3D_DEBUG
	if ((commandBufferCount == 0) || (ppCommandBuffers == nullptr))
	{
		V3D_LOG_S_PRINT_ERROR(Log_IV3DQueue_Submit << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(commandBufferCount, 0) << V3D_LOG_S_PTR(ppCommandBuffers));
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	for (uint32_t i = 0; i < commandBufferCount; i++)
	{
		if (ppCommandBuffers[i]->GetType() != V3D_COMMAND_BUFFER_TYPE_PRIMARY)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_NotPrimaryCommandBuffer, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}

		if (static_cast<V3DCommandBuffer*>(ppCommandBuffers[i])->GetDebug().isBegin == true)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_PrimaryCommandBufferNotEnd, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}
	}
#endif //V3D_DEBUG

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = commandBufferCount;
	submitInfo.pCommandBuffers = V3DQueue::Vulkan_CreateCommandBufferArray(m_Temp.commandBuffers, commandBufferCount, ppCommandBuffers);
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;

	VkFence fence = (pFence != nullptr) ? static_cast<V3DFence*>(pFence)->GetSource().fence : VK_NULL_HANDLE;
	VkResult vkResult = vkQueueSubmit(m_Source.queue, 1, &submitInfo, fence);
	
	return ToV3DResult(vkResult);
}

V3D_RESULT V3DQueue::Submit(uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores, IV3DFence* pFence)
{
#ifdef V3D_DEBUG
	if (((waitSemaphoreCount != 0) && ((ppWaitSemaphores == nullptr) || (pWaitDstStageMasks == nullptr))) || (commandBufferCount == 0) || (ppCommandBuffers == nullptr) || ((signalSemaphoreCount != 0) && (ppSignalSemaphores == nullptr)))
	{
		V3D_LOG_S_PRINT_ERROR(Log_IV3DQueue_Submit << Log_Error_InvalidArgument << V3D_LOG_S_NUM(waitSemaphoreCount) << V3D_LOG_S_PTR(ppWaitSemaphores) << V3D_LOG_S_PTR(pWaitDstStageMasks) << V3D_LOG_S_NUM_GREATER(commandBufferCount, 0) << V3D_LOG_S_PTR(ppCommandBuffers) << V3D_LOG_S_NUM(signalSemaphoreCount) << V3D_LOG_S_PTR(ppSignalSemaphores));
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	for (uint32_t i = 0; i < commandBufferCount; i++)
	{
		if (ppCommandBuffers[i]->GetType() != V3D_COMMAND_BUFFER_TYPE_PRIMARY)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_NotPrimaryCommandBuffer, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}

		if (static_cast<V3DCommandBuffer*>(ppCommandBuffers[i])->GetDebug().isBegin == true)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_PrimaryCommandBufferNotEnd, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}
	}
#endif //V3D_DEBUG

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphoreCount;
	submitInfo.pWaitSemaphores = V3DQueue::Vulkan_CreateSemaphoreArray(m_Temp.waitSemaphores, 0, waitSemaphoreCount, ppWaitSemaphores);;
	submitInfo.pWaitDstStageMask = V3DQueue::Vulkan_CreatePipelineStageFlags(m_Temp.waitDstStageMasks, 0, waitSemaphoreCount, pWaitDstStageMasks);
	submitInfo.commandBufferCount = commandBufferCount;
	submitInfo.pCommandBuffers = V3DQueue::Vulkan_CreateCommandBufferArray(m_Temp.commandBuffers, commandBufferCount, ppCommandBuffers);
	submitInfo.signalSemaphoreCount = signalSemaphoreCount;
	submitInfo.pSignalSemaphores = V3DQueue::Vulkan_CreateSemaphoreArray(m_Temp.signalSemaphores, 0, signalSemaphoreCount, ppSignalSemaphores);

	VkFence fence = (pFence != nullptr) ? static_cast<V3DFence*>(pFence)->GetSource().fence : VK_NULL_HANDLE;
	VkResult vkResult = vkQueueSubmit(m_Source.queue, 1, &submitInfo, fence);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DQueue::Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence)
{
#ifdef V3D_DEBUG
	if ((pSwapChain == nullptr) || (commandBufferCount == 0) || (ppCommandBuffers == nullptr))
	{
		V3D_LOG_S_PRINT_ERROR(Log_IV3DQueue_Submit << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pSwapChain) << V3D_LOG_S_NUM_GREATER(commandBufferCount, 0) << V3D_LOG_S_PTR(ppCommandBuffers));
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	for (uint32_t i = 0; i < commandBufferCount; i++)
	{
		if (ppCommandBuffers[i]->GetType() != V3D_COMMAND_BUFFER_TYPE_PRIMARY)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_NotPrimaryCommandBuffer, V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}

		if (static_cast<V3DCommandBuffer*>(ppCommandBuffers[i])->GetDebug().isBegin == true)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_PrimaryCommandBufferNotEnd, V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}
	}
#endif //V3D_DEBUG

	const V3DSwapChain::Source& swapChainSource = static_cast<V3DSwapChain*>(pSwapChain)->GetSource();

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &swapChainSource.pPresentCompleteSemaphores[swapChainSource.currentSyncIndex];
	submitInfo.pWaitDstStageMask = &swapChainSource.waitDstStageMask;
	submitInfo.commandBufferCount = commandBufferCount;
	submitInfo.pCommandBuffers = V3DQueue::Vulkan_CreateCommandBufferArray(m_Temp.commandBuffers, commandBufferCount, ppCommandBuffers);
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &swapChainSource.pRenderingCompleteSemaphores[swapChainSource.currentSyncIndex];

	VkFence fence = (pFence != nullptr) ? static_cast<V3DFence*>(pFence)->GetSource().fence : VK_NULL_HANDLE;
	VkResult vkResult = vkQueueSubmit(m_Source.queue, 1, &submitInfo, fence);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DQueue::Submit(IV3DSwapChain* pSwapChain, uint32_t waitSemaphoreCount, IV3DSemaphore** ppWaitSemaphores, const V3DFlags* pWaitDstStageMasks, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, uint32_t signalSemaphoreCount, IV3DSemaphore** ppSignalSemaphores, IV3DFence* pFence)
{
#ifdef V3D_DEBUG
	if (((waitSemaphoreCount != 0) && (ppWaitSemaphores == nullptr)) || (commandBufferCount == 0) || (ppCommandBuffers == nullptr) || ((signalSemaphoreCount != 0) && (ppSignalSemaphores == nullptr)))
	{
		V3D_LOG_S_PRINT_ERROR(Log_IV3DQueue_Submit << Log_Error_InvalidArgument << V3D_LOG_S_NUM_GREATER(waitSemaphoreCount, 0) << V3D_LOG_S_NUM(waitSemaphoreCount) << V3D_LOG_S_PTR(ppWaitSemaphores) << V3D_LOG_S_PTR(pWaitDstStageMasks) << V3D_LOG_S_NUM_GREATER(commandBufferCount, 0) << V3D_LOG_S_PTR(ppCommandBuffers) << V3D_LOG_S_NUM(signalSemaphoreCount) << V3D_LOG_S_PTR(ppSignalSemaphores));
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	for (uint32_t i = 0; i < commandBufferCount; i++)
	{
		if (ppCommandBuffers[i]->GetType() != V3D_COMMAND_BUFFER_TYPE_PRIMARY)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_NotPrimaryCommandBuffer, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}

		if (static_cast<V3DCommandBuffer*>(ppCommandBuffers[i])->GetDebug().isBegin == true)
		{
			V3D_LOG_PRINT_ERROR(Log_Error_PrimaryCommandBufferNotEnd, m_DebugName.c_str(), V3D_LOG_TYPE(ppCommandBuffers), i);
			return V3D_ERROR_INVALID_ARGUMENT;
		}
	}
#endif //V3D_DEBUG

	V3DSwapChain* pInternalSwapChain = static_cast<V3DSwapChain*>(pSwapChain);
	const V3DSwapChain::Source& swapChainSource = pInternalSwapChain->GetSource();

	VkSemaphore* pWaitSemaphores = V3DQueue::Vulkan_CreateSemaphoreArray(m_Temp.waitSemaphores, 1, waitSemaphoreCount, ppWaitSemaphores);
	pWaitSemaphores[0] = swapChainSource.pPresentCompleteSemaphores[swapChainSource.currentSyncIndex];

	uint32_t* pWaitStageMasks = V3DQueue::Vulkan_CreatePipelineStageFlags(m_Temp.waitDstStageMasks, 1, waitSemaphoreCount, pWaitDstStageMasks);
	pWaitStageMasks[0] = ToVkPipelineStageFlags(swapChainSource.waitDstStageMask);

	VkSemaphore* pSignalSemaphores = V3DQueue::Vulkan_CreateSemaphoreArray(m_Temp.signalSemaphores, 1, signalSemaphoreCount, ppSignalSemaphores);
	pSignalSemaphores[0] = swapChainSource.pRenderingCompleteSemaphores[swapChainSource.currentSyncIndex];

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = waitSemaphoreCount + 1;
	submitInfo.pWaitSemaphores = pWaitSemaphores;
	submitInfo.pWaitDstStageMask = pWaitStageMasks;
	submitInfo.commandBufferCount = commandBufferCount;
	submitInfo.pCommandBuffers = V3DQueue::Vulkan_CreateCommandBufferArray(m_Temp.commandBuffers, commandBufferCount, ppCommandBuffers);
	submitInfo.signalSemaphoreCount = signalSemaphoreCount + 1;
	submitInfo.pSignalSemaphores = pSignalSemaphores;

	VkFence fence = (pFence != nullptr) ? static_cast<V3DFence*>(pFence)->GetSource().fence : VK_NULL_HANDLE;

	VkResult vkResult = vkQueueSubmit(m_Source.queue, 1, &submitInfo, fence);

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DQueue::Present(IV3DSwapChain* pSwapChain)
{
#ifdef V3D_DEBUG
	if (pSwapChain == nullptr)
	{
		V3D_LOG_S_PRINT_ERROR(Log_IV3DQueue_Present << Log_Error_InvalidArgument << V3D_LOG_S_PTR(pSwapChain));
		return V3D_ERROR_INVALID_ARGUMENT;
	}
#endif //V3D_DEBUG

	V3DSwapChain* pInternalSwapChain = static_cast<V3DSwapChain*>(pSwapChain);
	const V3DSwapChain::Source& swapChainSource = pInternalSwapChain->GetSource();

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &swapChainSource.pRenderingCompleteSemaphores[swapChainSource.currentSyncIndex];
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChainSource.swapChain;
	presentInfo.pImageIndices = &swapChainSource.currentImageIndex;
	presentInfo.pResults = nullptr;

	V3D_RESULT result = V3D_OK;

	VkResult vkResult = vkQueuePresentKHR(m_Source.queue, &presentInfo);
	if (vkResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		result = pInternalSwapChain->RecreateSurfaceAndSwapChain();
	}
	else if (vkResult == VK_SUBOPTIMAL_KHR)
	{
		result = pInternalSwapChain->RecreateSwapChain();
	}
	else if (vkResult != VK_SUCCESS)
	{
		result = ToV3DResult(vkResult);
	}

	return result;
}

V3D_RESULT V3DQueue::WaitIdle()
{
	VkResult vkResult = vkQueueWaitIdle(m_Source.queue);
	return ToV3DResult(vkResult);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DQueue::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DQueue::GetRefCount() const
{
	return m_RefCounter;
}

void V3DQueue::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DQueue::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DQueue);
	}
}

/*****************************/
/* private - V3DQueue */
/*****************************/

V3DQueue::V3DQueue() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Source({}),
	m_Family(0)
{
	m_Source.queue = VK_NULL_HANDLE;
}

V3DQueue::~V3DQueue()
{
	V3D_REMOVE_DEBUG_MEMORY_OBJECT(this);
}
