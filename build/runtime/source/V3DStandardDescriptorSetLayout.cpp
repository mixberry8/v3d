#include "V3DStandardDescriptorSetLayout.h"
#include "V3DDevice.h"
#include "ScopedLock.h"
#include <algorithm>

struct V3DStandardDescriptorSetLayout::HandleT
{
	uint32_t maxCount;
	uint32_t count;

	VkDescriptorPool pool;
	STLVector<VkDescriptorSet> sets;

#ifdef _DEBUG
	void* pDebugPtr;
#endif //_DEBUG

	V3D_DECLARE_ALLOCATOR
};

/*******************************************/
/* public - V3DStandardDescriptorSetLayout */
/*******************************************/

V3DStandardDescriptorSetLayout* V3DStandardDescriptorSetLayout::Create()
{
	return V3D_NEW_T(V3DStandardDescriptorSetLayout);
}

V3D_RESULT V3DStandardDescriptorSetLayout::Initialize(V3DDevice* pDevice, uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep, const wchar_t* pDebugName)
{
	V3D_ASSERT(poolSize > 0);

	V3D_RESULT result = V3DBaseDescriptorSetLayout::Initialize(pDevice, descriptorCount, descriptors, false, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタプールのサイズを求める
	// ----------------------------------------------------------------------------------------------------

	uint32_t maxBinding = 0;

	m_PoolSizes.resize(VK_DESCRIPTOR_TYPE_RANGE_SIZE);
	memcpy_s(m_PoolSizes.data(), sizeof(V3DStandardDescriptorSetLayout::DescriptorPoolSizeInitList),
		V3DStandardDescriptorSetLayout::DescriptorPoolSizeInitList, sizeof(V3DStandardDescriptorSetLayout::DescriptorPoolSizeInitList));

	V3DFlags vPoolSizeFlags = 0;
	uint32_t vPoolSizeCount = 0;

	for (uint32_t i = 0; i < descriptorCount; i++)
	{
		const V3DDescriptorDesc& src = descriptors[i];

		maxBinding = V3D_MAX(maxBinding, src.binding);

		VkDescriptorType vkDescType = ToVkDescriptorType(src.type);

		m_PoolSizes[vkDescType].descriptorCount++;

		V3D_ASSERT(32 > vkDescType);
		uint32_t poolSizeBit = 1 << vkDescType;
		if ((vPoolSizeFlags & poolSizeBit) == 0)
		{
			vPoolSizeCount++;
			vPoolSizeFlags |= poolSizeBit;
		}
	}

	std::sort(m_PoolSizes.begin(), m_PoolSizes.end(), [](const VkDescriptorPoolSize& lh, const VkDescriptorPoolSize& rh) { return lh.descriptorCount > rh.descriptorCount; });
	m_PoolSizes.resize(vPoolSizeCount);
	m_PoolSizes.shrink_to_fit();

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタプールを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorPoolCreateInfo& poolCreateInfo = m_PoolCreateInfo;
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pNext = nullptr;
	poolCreateInfo.flags = 0;
	poolCreateInfo.maxSets = poolSize;
	poolCreateInfo.poolSizeCount = vPoolSizeCount;
	poolCreateInfo.pPoolSizes = m_PoolSizes.data();

	V3DStandardDescriptorSetLayout::Handle dummyHandle;
	result = AddHandle(&dummyHandle);
	if (result != V3D_OK)
	{
		return result;
	}

	m_PoolCreateInfo.maxSets = poolResizeStep;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3D_RESULT V3DStandardDescriptorSetLayout::Vulkan_CreateDescriptorSet(V3DStandardDescriptorSetLayout::Handle* pHandle, VkDescriptorSet* pDescriptorSet)
{
	ScopedLock lock(m_CriticalSection);

	// ----------------------------------------------------------------------------------------------------
	// ハンドルを取得
	// ----------------------------------------------------------------------------------------------------

	V3DStandardDescriptorSetLayout::HandleT* pHandleT = m_Handles.back();

	if (pHandleT->count == pHandleT->maxCount)
	{
		V3D_ASSERT(m_Statistics.setCount == m_Statistics.setCapacity);

		V3D_RESULT result = AddHandle(&pHandleT);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットを作成
	// ----------------------------------------------------------------------------------------------------

	VkDescriptorSet descriptorSet;

	if (pHandleT->sets.empty() == false)
	{
		descriptorSet = pHandleT->sets.back();
		pHandleT->sets.pop_back();
	}
	else
	{
		VkDescriptorSetAllocateInfo descSetAllocInfo;
		descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descSetAllocInfo.pNext = nullptr;
		descSetAllocInfo.descriptorPool = pHandleT->pool;
		descSetAllocInfo.descriptorSetCount = 1;
		descSetAllocInfo.pSetLayouts = &m_Source.descriptorSetLayout;

		VkResult vkResult = vkAllocateDescriptorSets(m_pDevice->GetSource().device, &descSetAllocInfo, &descriptorSet);
		if (vkResult != VK_SUCCESS)
		{
			return ToV3DResult(vkResult);
		}
	}

	pHandleT->count++;
	m_Statistics.setCount++;

	if ((pHandleT->count == pHandleT->maxCount) && (m_Statistics.setCount < m_Statistics.setCapacity))
	{
		// いっぱいになったのでソート
		std::sort(m_Handles.begin(), m_Handles.end(), [](const V3DStandardDescriptorSetLayout::HandleT* lh, const V3DStandardDescriptorSetLayout::HandleT* rh) { return lh->count > rh->count; });
	}

	*pHandle = pHandleT;
	*pDescriptorSet = descriptorSet;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void V3DStandardDescriptorSetLayout::Vulkan_DestroyDescriptorSet(V3DStandardDescriptorSetLayout::Handle handle, VkDescriptorSet descriptorSet)
{
	V3D_ASSERT(handle != nullptr);
	V3D_ASSERT(descriptorSet != VK_NULL_HANDLE);

	V3D_ASSERT(handle->pDebugPtr == this);
	V3D_ASSERT(handle->count > 0);
	V3D_ASSERT(m_Statistics.setCount > 0);

	ScopedLock lock(m_CriticalSection);

	handle->sets.push_back(descriptorSet);
	handle->count--;

	if ((handle->count == 0) || (m_Statistics.setCount == m_Statistics.setCapacity))
	{
		// からっぽになった、前回までいっぱいだったが空きができたのでソート
		std::sort(m_Handles.begin(), m_Handles.end(), [](const V3DStandardDescriptorSetLayout::HandleT* lh, const V3DStandardDescriptorSetLayout::HandleT* rh) { return lh->count > rh->count; });
	}

	m_Statistics.setCount--;
}

/*********************************************/
/* public override - IV3DDescriptorSetLayout */
/*********************************************/

V3D_DESCRIPTOR_SET_TYPE V3DStandardDescriptorSetLayout::GetType() const
{
	return V3D_DESCRIPTOR_SET_TYPE_STANDARD;
}

const V3DDescriptorSetLayoutStatistics& V3DStandardDescriptorSetLayout::GetStatistics() const
{
	return m_Statistics;
}

/************************************/
/* private - V3DStandardDescriptorSetLayout */
/************************************/

V3DStandardDescriptorSetLayout::V3DStandardDescriptorSetLayout() :
	m_Statistics({}),
	m_PoolCreateInfo({})
{
}

V3DStandardDescriptorSetLayout::~V3DStandardDescriptorSetLayout()
{
	if (m_Handles.empty() == false)
	{
		VkDevice vkDevice = m_pDevice->GetSource().device;

		STLVector<V3DStandardDescriptorSetLayout::HandleT*>::iterator it_begin = m_Handles.begin();
		STLVector<V3DStandardDescriptorSetLayout::HandleT*>::iterator it_end = m_Handles.end();
		STLVector<V3DStandardDescriptorSetLayout::HandleT*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3DStandardDescriptorSetLayout::HandleT* pHandle = (*it);
			vkDestroyDescriptorPool(vkDevice, pHandle->pool, nullptr);
			V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), pHandle->pool);
			V3D_DELETE_T(pHandle, HandleT);
		}
	}
}

V3D_RESULT V3DStandardDescriptorSetLayout::AddHandle(V3DStandardDescriptorSetLayout::HandleT** ppHandle)
{
	VkDescriptorPool vkDescriptorPool;

	VkResult vkResult = vkCreateDescriptorPool(m_pDevice->GetSource().device, &m_PoolCreateInfo, nullptr, &vkDescriptorPool);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), vkDescriptorPool, m_DebugName.c_str());

	V3DStandardDescriptorSetLayout::HandleT* pHandle = V3D_NEW_T(V3DStandardDescriptorSetLayout::HandleT);
	if (pHandle == nullptr)
	{
		vkDestroyDescriptorPool(m_pDevice->GetSource().device, vkDescriptorPool, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), vkDescriptorPool);
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

#ifdef _DEBUG
	pHandle->pDebugPtr = this;
#endif //_DEBUG
	pHandle->pool = vkDescriptorPool;
	pHandle->maxCount = m_PoolCreateInfo.maxSets;
	pHandle->count = 0;

	m_Handles.push_back(pHandle);
	m_Statistics.setCapacity += pHandle->maxCount;
	m_Statistics.poolCount++;

	(*ppHandle) = pHandle;

	return V3D_OK;
}
