#include "V3DPipelineLayout.h"
#include "V3DDevice.h"
#include "V3DDescriptorSetLayout.h"

/******************************/
/* public - V3DPipelineLayout */
/******************************/

V3DPipelineLayout* V3DPipelineLayout::Create()
{
	return V3D_NEW_T(V3DPipelineLayout);
}

V3D_RESULT V3DPipelineLayout::Initialize(IV3DDevice* pDevice, uint32_t constantCount, V3DConstantDesc* pConstants, uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts, const wchar_t* pDebugName)
{
	V3D_ASSERT(pDevice != nullptr);

	m_pDevice = V3D_TO_ADD_REF(static_cast<V3DDevice*>(pDevice));

	// ----------------------------------------------------------------------------------------------------
	// コンスタント
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkPushConstantRange> vkConstantRanges;

	if (constantCount > 0)
	{
		vkConstantRanges.reserve(constantCount);
		m_Constants.reserve(constantCount);

		for (uint32_t i = 0; i < constantCount; i++)
		{
			const V3DConstantDesc& src = pConstants[i];
			VkPushConstantRange dst{};

			dst.stageFlags = ToVkShaderStageFlags(src.shaderStageFlags);
			dst.offset = src.offset;
			dst.size = src.size;

			vkConstantRanges.push_back(dst);
			m_Constants.push_back(src);
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウト
	// ----------------------------------------------------------------------------------------------------

	STLVector<VkDescriptorSetLayout> vkDescriptorSetLayouts;

	if (descriptorSetLayoutCount > 0)
	{
		vkDescriptorSetLayouts.reserve(descriptorSetLayoutCount);
		m_DescriptorSetLayouts.reserve(descriptorSetLayoutCount);

		for (uint32_t i = 0; i < descriptorSetLayoutCount; i++)
		{
			V3DDescriptorSetLayout* pInternalDescriptorSet = static_cast<V3DDescriptorSetLayout*>(ppDescriptorSetLayouts[i]);

			vkDescriptorSetLayouts.push_back(pInternalDescriptorSet->GetSource().descriptorSetLayout);
			m_DescriptorSetLayouts.push_back(V3D_TO_ADD_REF(pInternalDescriptorSet));
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// パイプラインキャッシュを作成
	// ----------------------------------------------------------------------------------------------------

	VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheCreateInfo.pNext = nullptr;
	pipelineCacheCreateInfo.flags = 0;
	pipelineCacheCreateInfo.initialDataSize = 0;
	pipelineCacheCreateInfo.pInitialData = nullptr;

	VkResult vkResult = vkCreatePipelineCache(m_pDevice->GetSource().device, &pipelineCacheCreateInfo, nullptr, &m_Source.pipelineCache);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipelineCache, V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(vkConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = vkConstantRanges.data();
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(vkDescriptorSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = vkDescriptorSetLayouts.data();

	vkResult = vkCreatePipelineLayout(m_pDevice->GetSource().device, &pipelineLayoutCreateInfo, nullptr, &m_Source.pipelineLayout);
	if (vkResult != VK_SUCCESS)
	{
		return ToV3DResult(vkResult);
	}

	V3D_ADD_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipelineLayout, V3D_SAFE_NAME(this, pDebugName));

	return V3D_OK;
}

const V3DPipelineLayout::Source& V3DPipelineLayout::GetSource() const
{
	return m_Source;
}

/****************************************/
/* public override - IV3DPipelineLayout */
/****************************************/

uint32_t V3DPipelineLayout::GetConstantCount() const
{
	return static_cast<uint32_t>(m_Constants.size());
}

const V3DConstantDesc& V3DPipelineLayout::GetConstantDesc(uint32_t constantIndex) const
{
	return m_Constants[constantIndex];
}

uint32_t V3DPipelineLayout::GetDescriptorSetCount() const
{
	return static_cast<uint32_t>(m_DescriptorSetLayouts.size());
}

void V3DPipelineLayout::GetDescriptorSetLayout(uint32_t descriptorSetIndex, IV3DDescriptorSetLayout** ppDescriptorLayout)
{
	(*ppDescriptorLayout) = V3D_TO_ADD_REF(m_DescriptorSetLayouts[descriptorSetIndex]);
}

/*************************************/
/* public override - IV3DDeviceChild */
/*************************************/

void V3DPipelineLayout::GetDevice(IV3DDevice** ppDevice)
{
	(*ppDevice) = V3D_TO_ADD_REF(m_pDevice);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DPipelineLayout::GetRefCount() const
{
	return m_RefCounter;
}

void V3DPipelineLayout::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DPipelineLayout::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DPipelineLayout);
	}
}

/*******************************/
/* private - V3DPipelineLayout */
/*******************************/

V3DPipelineLayout::V3DPipelineLayout() :
	m_RefCounter(1),
	m_pDevice(nullptr)
{
	m_Source.pipelineCache = VK_NULL_HANDLE;
	m_Source.pipelineLayout = VK_NULL_HANDLE;
}

V3DPipelineLayout::~V3DPipelineLayout()
{
	if (m_Source.pipelineCache != VK_NULL_HANDLE)
	{
		vkDestroyPipelineCache(m_pDevice->GetSource().device, m_Source.pipelineCache, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipelineCache);
	}

	if (m_Source.pipelineLayout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(m_pDevice->GetSource().device, m_Source.pipelineLayout, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pDevice->GetInternalInstancePtr(), m_Source.pipelineLayout);
	}

	if (m_DescriptorSetLayouts.empty() == false)
	{
		STLVector<IV3DDescriptorSetLayout*>::iterator it_begin = m_DescriptorSetLayouts.begin();
		STLVector<IV3DDescriptorSetLayout*>::iterator it_end = m_DescriptorSetLayouts.end();
		STLVector<IV3DDescriptorSetLayout*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			V3D_RELEASE((*it));
		}
	}

	V3D_RELEASE(m_pDevice);
}
