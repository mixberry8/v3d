#include "V3DQueryPool.h"
#include "V3DDevice.h"
#include "V3DAdapter.h"

/************************/
/* public V3DQueryPool */
/************************/

V3DQueryPool* V3DQueryPool::Create()
{
	return V3D_NEW_T(V3DQueryPool);
}

V3D_RESULT V3DQueryPool::Initialize(IV3DDevice* pDevice, const V3DQueryPoolDesc& desc)
{
	V3D_ASSERT(pDevice != nullptr);
	V3D_ASSERT(desc.queryCount > 0);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	VkQueryPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.queryType = ToVkQueryType(desc.queryType);
	createInfo.queryCount = desc.queryCount;

	if (desc.queryType == V3D_QUERY_TYPE_PIPELINE_STATISTICS)
	{
		createInfo.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT;
	}
	else
	{
		createInfo.pipelineStatistics = 0;
	}

	VkResult vkResult = vkCreateQueryPool(m_pDevice->GetSource().device, &createInfo, nullptr, &m_Source.queryPool);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	m_Desc = desc;

	if ((desc.queryType == V3D_QUERY_TYPE_OCCLUSION) && (m_pDevice->GetCaps().queryFlags & V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE))
	{
		m_Source.queryControlFlags = VK_QUERY_CONTROL_PRECISE_BIT;
	}
	else
	{
		m_Source.queryControlFlags = 0;
	}

	return V3D_OK;
}

/***********************************/
/* public override - IV3DImageBase */
/***********************************/

const V3DQueryPool::Source& V3DQueryPool::GetSource() const
{
	return m_Source;
}

/*******************************/
/* public override - IV3DImage */
/*******************************/

const V3DQueryPoolDesc& V3DQueryPool::GetDesc() const
{
	return m_Desc;
}

V3D_RESULT V3DQueryPool::GetResults(uint32_t firstQuery, uint32_t queryCount, uint32_t queryResultCount, uint64_t* pQueryResults, V3DFlags queryResultFlags)
{
#ifdef _DEBUG
	if ((m_Desc.queryCount <= firstQuery) || (m_Desc.queryCount < (firstQuery + queryCount)))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}
#endif //_DEBUG

	VkResult vkResult = vkGetQueryPoolResults(
		m_pDevice->GetSource().device,
		m_Source.queryPool,
		firstQuery, queryCount,
		sizeof(uint64_t) * queryResultCount, pQueryResults,
		sizeof(uint64_t),
		VK_QUERY_RESULT_64_BIT | ToVkQueryResultFlags(queryResultFlags));

	return ToV3DResult(vkResult);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DQueryPool::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DQueryPool::GetRefCount() const
{
	return m_RefCounter;
}

void V3DQueryPool::AddRef()
{
	++m_RefCounter;
}

void V3DQueryPool::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DQueryPool);
	}
}

/*************************/
/* private V3DQueryPool */
/*************************/

V3DQueryPool::V3DQueryPool() :
	m_RefCounter(1),
	m_pDevice(nullptr),
	m_Desc({}),
	m_Source({})
{
	m_Source.queryPool = VK_NULL_HANDLE;
}

V3DQueryPool::~V3DQueryPool()
{
	if (m_Source.queryPool != VK_NULL_HANDLE)
	{
		vkDestroyQueryPool(m_pDevice->GetSource().device, m_Source.queryPool, nullptr);
	}

	V3D_RELEASE(m_pDevice);
}
