#include "V3DDevice.h"
#include "V3DInstance.h"
#include "V3DAdapter.h"
#include "V3DFence.h"
#include "V3DQueryPool.h"
#include "V3DQueue.h"
#include "V3DCommandPool.h"
#include "V3DCommandBuffer.h"
#include "V3DSwapChain.h"
#include "V3DShaderModule.h"
#include "V3DResourceMemory.h"
#include "V3DBuffer.h"
#include "V3DImage.h"
#include "V3DBufferView.h"
#include "V3DImageView.h"
#include "V3DSampler.h"
#include "V3DRenderPass.h"
#include "V3DFrameBuffer.h"
#include "V3DStandardDescriptorSetLayout.h"
#include "V3DPushDescriptorSetLayout.h"
#include "V3DStandardDescriptorSet.h"
#include "V3DPushDescriptorSet.h"
#include "V3DPipelineLayout.h"
#include "V3DGraphicsPipeline.h"
#include "V3DComputePipeline.h"

/**********************/
/* public - V3DDevice */
/**********************/

V3DDevice* V3DDevice::Create()
{
	return V3D_NEW_T(V3DDevice);
}

V3D_RESULT V3DDevice::Initialize(V3DInstance* pInstance, IV3DAdapter* pAdapter, const wchar_t* pDebugName)
{
	V3D_ASSERT(pAdapter != nullptr);

	m_pInstance = static_cast<V3DInstance*>(V3D_TO_ADD_REF(pInstance));
	m_pAdapter = static_cast<V3DAdapter*>(V3D_TO_ADD_REF(pAdapter));

	const V3DAdapter::Source& adapterSource = m_pAdapter->GetSource();

	// ----------------------------------------------------------------------------------------------------
	// レイヤーの列挙
	// ----------------------------------------------------------------------------------------------------

	// いらなくなったらしい
/*
	uint32_t vkLayerCount;
	VkResult vkResult = vkEnumerateDeviceLayerProperties(adapterSource.physicalDevice, &vkLayerCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<VkLayerProperties> vkLayerProps;
	vkLayerProps.resize(vkLayerCount);
	vkResult = vkEnumerateDeviceLayerProperties(adapterSource.physicalDevice, &vkLayerCount, vkLayerProps.data());
	if (vkResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<const char*> vkEnableLayers;
	switch (m_pInstance->GetLayerType())
	{
	case V3D_LAYER_VALIDATION:
		if (std::find_if(vkLayerProps.begin(), vkLayerProps.end(), V3DFindLayer(V3D_LAYER_LUNARG_standard_validation)) != vkLayerProps.end())
		{
			vkEnableLayers.push_back(V3D_LAYER_LUNARG_standard_validation);
		}
		break;

	case V3D_LAYER_RENDERDOC:
		if (std::find_if(vkLayerProps.begin(), vkLayerProps.end(), V3DFindLayer(V3D_LAYER_RENDERDOC_Capture)) != vkLayerProps.end())
		{
			vkEnableLayers.push_back(V3D_LAYER_RENDERDOC_Capture);
		}
	}
*/
	// ----------------------------------------------------------------------------------------------------
	// エクステンションの列挙
	// ----------------------------------------------------------------------------------------------------

	uint32_t vExtensionCount;
	VkResult vResult = vkEnumerateDeviceExtensionProperties(adapterSource.physicalDevice, nullptr, &vExtensionCount, nullptr);
	if (vResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<VkExtensionProperties> vExtensionProps;
	vExtensionProps.resize(vExtensionCount);
	vResult = vkEnumerateDeviceExtensionProperties(adapterSource.physicalDevice, nullptr, &vExtensionCount, vExtensionProps.data());
	if (vResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	if (std::find_if(vExtensionProps.begin(), vExtensionProps.end(), V3DFindExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) == vExtensionProps.end())
	{
		return V3D_ERROR_FAIL;
	}

	const char* VK_KHX_NAME = "VK_KHX";
	const char* VK_NVX_NAME = "VK_NVX";

	STLVector<const char*> vEnableExtensions;
	vEnableExtensions.reserve(vExtensionCount);

	for (uint32_t i = 0; i < vExtensionCount; i++)
	{
		const char* pExtensionName = vExtensionProps[i].extensionName;

		// 実験的なものは除外する
		if ((strncmp(pExtensionName, VK_KHX_NAME, strlen(VK_KHX_NAME)) != 0) &&
			(strncmp(pExtensionName, VK_NVX_NAME, strlen(VK_NVX_NAME)) != 0))
		{
			vEnableExtensions.push_back(pExtensionName);
		}
	}

	V3DFlags extensionFlags = 0;

	if (std::find_if(vExtensionProps.begin(), vExtensionProps.end(), V3DFindExtension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)) != vExtensionProps.end())
	{
		extensionFlags |= V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS;
	}

	V3D_LAYER layer = m_pInstance->GetLayer();
	if ((layer == V3D_LAYER_NSIGHT) || (layer == V3D_LAYER_RENDERDOC))
	{
		if (std::find_if(vExtensionProps.begin(), vExtensionProps.end(), V3DFindExtension(VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) != vExtensionProps.end())
		{
			extensionFlags |= V3D_DEVICE_EXTENSION_DEBUG_MARKER;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイス情報を取得
	// ----------------------------------------------------------------------------------------------------

	VkPhysicalDevicePushDescriptorPropertiesKHR devicePushDescProp{};

	// 特性を取得
	PFN_vkGetPhysicalDeviceProperties2KHR pGetPhysicalDeviceProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(vkGetInstanceProcAddr(m_pInstance->GetSource().instance, "vkGetPhysicalDeviceProperties2KHR"));
	if (pGetPhysicalDeviceProperties2KHR != nullptr)
	{
		VkPhysicalDeviceProperties2KHR devicePropsExt{};

		devicePushDescProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PUSH_DESCRIPTOR_PROPERTIES_KHR;
		devicePushDescProp.pNext = nullptr;

		devicePropsExt.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
		devicePropsExt.pNext = &devicePushDescProp;
		pGetPhysicalDeviceProperties2KHR(adapterSource.physicalDevice, &devicePropsExt);

		m_Source.deviceProps = devicePropsExt.properties;
	}
	else
	{
		vkGetPhysicalDeviceProperties(adapterSource.physicalDevice, &m_Source.deviceProps);
	}

	// 機能を取得
	vkGetPhysicalDeviceFeatures(adapterSource.physicalDevice, &m_Source.deviceFeatures);

	// ----------------------------------------------------------------------------------------------------
	// キューファミリーの取得
	// キューの作成情報リストを作成
	// ----------------------------------------------------------------------------------------------------

	uint32_t vQueueFamilyPropCount;
	vkGetPhysicalDeviceQueueFamilyProperties(adapterSource.physicalDevice, &vQueueFamilyPropCount, nullptr);

	STLVector<VkQueueFamilyProperties> vQueueFamilyProps;
	vQueueFamilyProps.resize(vQueueFamilyPropCount);
	vkGetPhysicalDeviceQueueFamilyProperties(adapterSource.physicalDevice, &vQueueFamilyPropCount, vQueueFamilyProps.data());

	uint32_t totalQueueCount = 0;
	for (uint32_t i = 0; i < vQueueFamilyPropCount; i++)
	{
		const VkQueueFamilyProperties& vProps = vQueueFamilyProps[i];
		totalQueueCount += vProps.queueCount;
	}

	STLVector<float> vkQueuePriorities;
	vkQueuePriorities.resize(totalQueueCount, 0.0f);

	STLVector<VkDeviceQueueCreateInfo> vQueueCreateInfos;
	vQueueCreateInfos.reserve(vQueueFamilyPropCount);

	uint32_t firstQueuePriority = 0;

	m_QueueFamilies.reserve(vQueueFamilyPropCount);
	for (uint32_t i = 0; i < vQueueFamilyPropCount; i++)
	{
		const VkQueueFamilyProperties& vProps = vQueueFamilyProps[i];

		V3DDevice::QueueFamily queueFamily;
		queueFamily.desc.queueFlags = ToV3DQueueFlags(vProps.queueFlags);
		queueFamily.desc.queueCount = vProps.queueCount;
		m_QueueFamilies.push_back(queueFamily);

		VkDeviceQueueCreateInfo vQueueCreateInfo;
		vQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		vQueueCreateInfo.pNext = nullptr;
		vQueueCreateInfo.flags = 0;
		vQueueCreateInfo.queueCount = vProps.queueCount;
		vQueueCreateInfo.queueFamilyIndex = i;
		vQueueCreateInfo.pQueuePriorities = &vkQueuePriorities[firstQueuePriority];
		vQueueCreateInfos.push_back(vQueueCreateInfo);

		firstQueuePriority += vProps.queueCount;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスを作成
	// ----------------------------------------------------------------------------------------------------

	VkDeviceCreateInfo devInfo;
	devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devInfo.pNext = nullptr;
	devInfo.flags = 0;
	devInfo.queueCreateInfoCount = static_cast<uint32_t>(vQueueCreateInfos.size());
	devInfo.pQueueCreateInfos = vQueueCreateInfos.data();
	devInfo.enabledLayerCount = 0;// static_cast<uint32_t>(vkEnableLayers.size());
	devInfo.ppEnabledLayerNames = nullptr;// vkEnableLayers.data();
	devInfo.enabledExtensionCount = static_cast<uint32_t>(vEnableExtensions.size());
	devInfo.ppEnabledExtensionNames = vEnableExtensions.data();
	devInfo.pEnabledFeatures = &m_Source.deviceFeatures;

	vResult = vkCreateDevice(adapterSource.physicalDevice, &devInfo, nullptr, &m_Source.device);
	if (vResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	V3D_ADD_DEBUG_OBJECT(m_pInstance, m_Source.device, V3D_SAFE_NAME(this, pDebugName));

	// ----------------------------------------------------------------------------------------------------
	// 拡張機能を取得
	// ----------------------------------------------------------------------------------------------------

	if (extensionFlags & V3D_DEVICE_EXTENSION_DEBUG_MARKER)
	{
		m_pDebugMarkerSetObjectNameFunction = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(m_Source.device, "vkDebugMarkerSetObjectNameEXT"));
	}

	// ----------------------------------------------------------------------------------------------------
	// キューを取得
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < vQueueFamilyPropCount; i++)
	{
		V3DDevice::QueueFamily& queueFamily = m_QueueFamilies[i];
		queueFamily.queues.reserve(queueFamily.desc.queueCount);

		for (uint32_t j = 0; j < queueFamily.desc.queueCount; j++)
		{
			VkQueue vSrcQueue;
			vkGetDeviceQueue(m_Source.device, i, j, &vSrcQueue);

			V3DQueue* pQueue = V3DQueue::Create();
			if (pQueue == nullptr)
			{
				return V3D_ERROR_OUT_OF_HOST_MEMORY;
			}

#ifdef _DEBUG
			STLStringStreamW debugStringStream;
			debugStringStream << V3D_SAFE_NAME(this, pDebugName) << L"_Queue_F" << i << L"_I" << j;

			STLStringW debugString = debugStringStream.str();

			V3D_RESULT result = pQueue->Initialize(this, i, vSrcQueue, debugString.c_str());
#else //_DEBUG
			V3D_RESULT result = pQueue->Initialize(this, i, vSrcQueue, nullptr);
#endif //_DEBUG

			if (result != V3D_OK)
			{
				pQueue->Release();
				return result;
			}

			queueFamily.queues.push_back(pQueue);
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスの能力
	// ----------------------------------------------------------------------------------------------------

	const VkPhysicalDeviceFeatures vPDFeatures = m_Source.deviceFeatures;
	const VkPhysicalDeviceLimits& vPDLimits = m_Source.deviceProps.limits;

	if (vPDFeatures.fullDrawIndexUint32 == VK_TRUE) { m_Caps.flags |= V3D_CAP_FULL_DRAW_INDEX_UINT32; }
	if (vPDFeatures.samplerAnisotropy == VK_TRUE) { m_Caps.flags |= V3D_CAP_SAMPLER_ANISOTROPY; }

	if (vPDFeatures.occlusionQueryPrecise == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE; }
	if (vPDFeatures.pipelineStatisticsQuery == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY; }
	if (vPDLimits.timestampComputeAndGraphics == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_TIMESTAMP_QUERY_GRAPHICS_AND_COMPUTE; }
	if (vPDFeatures.inheritedQueries == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_INHERITED_QUERIES; }

	if (vPDFeatures.imageCubeArray == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_CUBE_ARRAY; }
	if (vPDFeatures.textureCompressionETC2 == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_ETC2; }
	if (vPDFeatures.textureCompressionASTC_LDR == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_ASTC_LDR; }
	if (vPDFeatures.textureCompressionBC == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_BC; }

	if (vPDFeatures.geometryShader == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_GEOMETRY; }
	if (vPDFeatures.tessellationShader == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_TESSELLATION; }
	if (vPDFeatures.robustBufferAccess == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS; }
	if (vPDFeatures.vertexPipelineStoresAndAtomics == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_VERTEX_PIPELINE_STORES_AND_ATOMICS; }
	if (vPDFeatures.fragmentStoresAndAtomics == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_FRAGMENT_STORES_AND_ATOMICS; }
	if (vPDFeatures.largePoints == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_LARGE_POINTS; }
	if (vPDFeatures.shaderTessellationAndGeometryPointSize == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_TESSELLATION_AND_GEOMETRY_POINTSIZE; }
	if (vPDFeatures.shaderImageGatherExtended == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_IMAGE_GATHER_EXTENDED; }
	if (vPDFeatures.shaderStorageImageExtendedFormats == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_EXTENDED_FORMATS; }
	if (vPDFeatures.shaderStorageImageMultisample == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_MULTISAMPLE; }
	if (vPDFeatures.shaderStorageImageReadWithoutFormat == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_READ_WITH_OUT_FORMAT; }
	if (vPDFeatures.shaderStorageImageWriteWithoutFormat == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_WRITE_WITH_OUT_FORMAT; }
	if (vPDFeatures.shaderUniformBufferArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING; }
	if (vPDFeatures.shaderSampledImageArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING; }
	if (vPDFeatures.shaderStorageBufferArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING; }
	if (vPDFeatures.shaderStorageImageArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING; }
	if (vPDFeatures.shaderClipDistance == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_CLIP_DISTANCE; }
	if (vPDFeatures.shaderCullDistance == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_CULL_DISTANCE; }
	if (vPDFeatures.shaderFloat64 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_FLOAT64; }
	if (vPDFeatures.shaderInt64 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_INT64; }
	if (vPDFeatures.shaderInt16 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_INT16; }
	if (vPDFeatures.shaderResourceResidency == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_RESOURCE_RESIDENCY; }
	if (vPDFeatures.shaderResourceMinLod == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_RESOURCE_MIN_LOD; }

	if (vPDFeatures.multiViewport == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_MULTI_VIEWPORT; }
	if (vPDFeatures.fillModeNonSolid == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID; }
	if (vPDFeatures.depthClamp == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_DEPTH_CLAMP; }
	if (vPDFeatures.depthBiasClamp == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP; }

	if (vPDFeatures.variableMultisampleRate == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_VARIABLE_RATE; }
	if (vPDFeatures.sampleRateShading == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING; }
	if (vPDFeatures.alphaToOne == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE; }

	if (vPDFeatures.depthBounds == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS; }

	if (vPDFeatures.independentBlend == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_INDEPENDENT; }
	if (vPDFeatures.dualSrcBlend == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_DUAL_SRC; }
	if (vPDFeatures.logicOp == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_LOGIC_OP; }

	m_Caps.extensionFlags = extensionFlags;

	m_Caps.maxImageDimension1D = vPDLimits.maxImageDimension1D;
	m_Caps.maxImageDimension2D = vPDLimits.maxImageDimension2D;
	m_Caps.maxImageDimension3D = vPDLimits.maxImageDimension3D;
	m_Caps.maxImageDimensionCube = vPDLimits.maxImageDimensionCube;
	m_Caps.maxImageLayers = vPDLimits.maxImageArrayLayers;
	m_Caps.maxTexelBufferElements = vPDLimits.maxTexelBufferElements;
	m_Caps.maxUniformBufferSize = vPDLimits.maxUniformBufferRange;
	m_Caps.maxStorageBufferSize = vPDLimits.maxStorageBufferRange;
	m_Caps.maxPushConstantsSize = vPDLimits.maxPushConstantsSize;
	m_Caps.maxPushDescriptors = V3D_MIN(devicePushDescProp.maxPushDescriptors, V3D_PUSH_DESCRIPTOR_MAX);
	m_Caps.maxResourceMemoryCount = vPDLimits.maxMemoryAllocationCount;
	m_Caps.maxSamplerCreateCount = vPDLimits.maxSamplerAllocationCount;
	m_Caps.bufferImageGranularity = vPDLimits.bufferImageGranularity;
	m_Caps.maxBoundDescriptorSets = vPDLimits.maxBoundDescriptorSets;
	m_Caps.maxPerStageDescriptorSamplers = vPDLimits.maxPerStageDescriptorSamplers;
	m_Caps.maxPerStageDescriptorUniformBuffers = vPDLimits.maxPerStageDescriptorUniformBuffers;
	m_Caps.maxPerStageDescriptorStorageBuffers = vPDLimits.maxPerStageDescriptorStorageBuffers;
	m_Caps.maxPerStageDescriptorSampledImages = vPDLimits.maxPerStageDescriptorSampledImages;
	m_Caps.maxPerStageDescriptorStorageImages = vPDLimits.maxPerStageDescriptorStorageImages;
	m_Caps.maxPerStageDescriptorInputAttachments = vPDLimits.maxPerStageDescriptorInputAttachments;
	m_Caps.maxPerStageResources = vPDLimits.maxPerStageResources;
	m_Caps.maxDescriptorSetSamplers = vPDLimits.maxDescriptorSetSamplers;
	m_Caps.maxDescriptorSetUniformBuffers = vPDLimits.maxDescriptorSetUniformBuffers;
	m_Caps.maxDescriptorSetUniformBuffersDynamic = vPDLimits.maxDescriptorSetUniformBuffersDynamic;
	m_Caps.maxDescriptorSetStorageBuffers = vPDLimits.maxDescriptorSetStorageBuffers;
	m_Caps.maxDescriptorSetStorageBuffersDynamic = vPDLimits.maxDescriptorSetStorageBuffersDynamic;
	m_Caps.maxDescriptorSetSampledImages = vPDLimits.maxDescriptorSetSampledImages;
	m_Caps.maxDescriptorSetStorageImages = vPDLimits.maxDescriptorSetStorageImages;
	m_Caps.maxDescriptorSetInputAttachments = vPDLimits.maxDescriptorSetInputAttachments;
	m_Caps.maxVertexInputElements = vPDLimits.maxVertexInputAttributes;
	m_Caps.maxVertexInputBindings = vPDLimits.maxVertexInputBindings;
	m_Caps.maxVertexInputElementOffset = vPDLimits.maxVertexInputAttributeOffset;
	m_Caps.maxVertexInputBindingStride = vPDLimits.maxVertexInputBindingStride;
	m_Caps.maxVertexOutputComponents = vPDLimits.maxVertexOutputComponents;
	m_Caps.maxTessellationGenerationLevel = vPDLimits.maxTessellationGenerationLevel;
	m_Caps.maxTessellationPatchSize = vPDLimits.maxTessellationPatchSize;
	m_Caps.maxTessellationControlPerVertexInputComponents = vPDLimits.maxTessellationControlPerVertexInputComponents;
	m_Caps.maxTessellationControlPerVertexOutputComponents = vPDLimits.maxTessellationControlPerVertexOutputComponents;
	m_Caps.maxTessellationControlPerPatchOutputComponents = vPDLimits.maxTessellationControlPerPatchOutputComponents;
	m_Caps.maxTessellationControlTotalOutputComponents = vPDLimits.maxTessellationControlTotalOutputComponents;
	m_Caps.maxTessellationEvaluationInputComponents = vPDLimits.maxTessellationEvaluationInputComponents;
	m_Caps.maxTessellationEvaluationOutputComponents = vPDLimits.maxTessellationEvaluationOutputComponents;
	m_Caps.maxGeometryShaderInvocations = vPDLimits.maxGeometryShaderInvocations;
	m_Caps.maxGeometryInputComponents = vPDLimits.maxGeometryInputComponents;
	m_Caps.maxGeometryOutputComponents = vPDLimits.maxGeometryOutputComponents;
	m_Caps.maxGeometryOutputVertices = vPDLimits.maxGeometryOutputVertices;
	m_Caps.maxGeometryTotalOutputComponents = vPDLimits.maxGeometryTotalOutputComponents;
	m_Caps.maxFragmentInputComponents = vPDLimits.maxFragmentInputComponents;
	m_Caps.maxFragmentOutputAttachments = vPDLimits.maxFragmentOutputAttachments;
	m_Caps.maxFragmentDualSrcAttachments = vPDLimits.maxFragmentDualSrcAttachments;
	m_Caps.maxFragmentCombinedOutputResources = vPDLimits.maxFragmentCombinedOutputResources;
	m_Caps.maxComputeSharedMemorySize = vPDLimits.maxComputeSharedMemorySize;
	m_Caps.maxComputeDispatchGroupCount.x = vPDLimits.maxComputeWorkGroupCount[0];
	m_Caps.maxComputeDispatchGroupCount.y = vPDLimits.maxComputeWorkGroupCount[1];
	m_Caps.maxComputeDispatchGroupCount.z = vPDLimits.maxComputeWorkGroupCount[2];
	m_Caps.maxComputeDispatchGroupInvocations = vPDLimits.maxComputeWorkGroupInvocations;
	m_Caps.maxComputeDispatchGroupSize.x = vPDLimits.maxComputeWorkGroupSize[0];
	m_Caps.maxComputeDispatchGroupSize.y = vPDLimits.maxComputeWorkGroupSize[1];
	m_Caps.maxComputeDispatchGroupSize.z = vPDLimits.maxComputeWorkGroupSize[2];
	m_Caps.subPixelPrecisionBits = vPDLimits.subPixelPrecisionBits;
	m_Caps.subTexelPrecisionBits = vPDLimits.subTexelPrecisionBits;
	m_Caps.mipmapPrecisionBits = vPDLimits.mipmapPrecisionBits;
	m_Caps.maxDrawIndexedIndexValue = vPDLimits.maxDrawIndexedIndexValue;
	m_Caps.maxSamplerLodBias = vPDLimits.maxSamplerLodBias;
	m_Caps.maxSamplerAnisotropy = vPDLimits.maxSamplerAnisotropy;
	m_Caps.maxViewports = vPDLimits.maxViewports;
	m_Caps.maxViewportDimension.width = vPDLimits.maxViewportDimensions[0];
	m_Caps.maxViewportDimension.height = vPDLimits.maxViewportDimensions[1];
	m_Caps.minViewportBounds = vPDLimits.viewportBoundsRange[0];
	m_Caps.maxViewportBounds = vPDLimits.viewportBoundsRange[1];
	m_Caps.viewportSubPixelBits = vPDLimits.viewportSubPixelBits;
	m_Caps.minMemoryMapAlignment = vPDLimits.minMemoryMapAlignment;
	m_Caps.minTexelBufferOffsetAlignment = vPDLimits.minTexelBufferOffsetAlignment;
	m_Caps.minUniformBufferOffsetAlignment = vPDLimits.minUniformBufferOffsetAlignment;
	m_Caps.minStorageBufferOffsetAlignment = vPDLimits.minStorageBufferOffsetAlignment;
	m_Caps.minTexelOffset = vPDLimits.minTexelOffset;
	m_Caps.maxTexelOffset = vPDLimits.maxTexelOffset;
	m_Caps.minTexelGatherOffset = vPDLimits.minTexelGatherOffset;
	m_Caps.maxTexelGatherOffset = vPDLimits.maxTexelGatherOffset;
	m_Caps.minInterpolationOffset = vPDLimits.minInterpolationOffset;
	m_Caps.maxInterpolationOffset = vPDLimits.maxInterpolationOffset;
	m_Caps.subPixelInterpolationOffsetBits = vPDLimits.subPixelInterpolationOffsetBits;
	m_Caps.maxFrameBufferWidth = vPDLimits.maxFramebufferWidth;
	m_Caps.maxFrameBufferHeight = vPDLimits.maxFramebufferHeight;
	m_Caps.maxFrameBufferLayers = vPDLimits.maxFramebufferLayers;
	m_Caps.framebufferColorSampleCounts = ToVkSampleCountFlags(vPDLimits.framebufferColorSampleCounts);
	m_Caps.framebufferDepthSampleCounts = ToVkSampleCountFlags(vPDLimits.framebufferDepthSampleCounts);
	m_Caps.framebufferStencilSampleCounts = ToVkSampleCountFlags(vPDLimits.framebufferStencilSampleCounts);
	m_Caps.framebufferNoAttachmentsSampleCounts = ToVkSampleCountFlags(vPDLimits.framebufferNoAttachmentsSampleCounts);
	m_Caps.sampledImageColorSampleCounts = ToVkSampleCountFlags(vPDLimits.sampledImageColorSampleCounts);
	m_Caps.sampledImageIntegerSampleCounts = ToVkSampleCountFlags(vPDLimits.sampledImageIntegerSampleCounts);
	m_Caps.sampledImageDepthSampleCounts = ToVkSampleCountFlags(vPDLimits.sampledImageDepthSampleCounts);
	m_Caps.sampledImageStencilSampleCounts = ToVkSampleCountFlags(vPDLimits.sampledImageStencilSampleCounts);
	m_Caps.storageImageSampleCounts = ToVkSampleCountFlags(vPDLimits.storageImageSampleCounts);
	m_Caps.maxColorAttachments = vPDLimits.maxColorAttachments;
	m_Caps.maxSampleMaskWords = vPDLimits.maxSampleMaskWords;
	m_Caps.timestampPeriod = vPDLimits.timestampPeriod;
	m_Caps.maxClipDistances = vPDLimits.maxClipDistances;
	m_Caps.maxCullDistances = vPDLimits.maxCullDistances;
	m_Caps.maxCombinedClipAndCullDistances = vPDLimits.maxCombinedClipAndCullDistances;
	m_Caps.minPointSize = vPDLimits.pointSizeRange[0];
	m_Caps.maxPointSize = vPDLimits.pointSizeRange[1];
	m_Caps.pointSizeGranularity = vPDLimits.pointSizeGranularity;
	m_Caps.discreteQueuePriorities = vPDLimits.discreteQueuePriorities;
	m_Caps.standardSampleLocations = ToV3DBool(vPDLimits.standardSampleLocations);
	m_Caps.optimalBufferCopyOffsetAlignment = vPDLimits.optimalBufferCopyOffsetAlignment;
	m_Caps.nonCoherentAtomSize = vPDLimits.nonCoherentAtomSize;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

V3DInstance* V3DDevice::GetInternalInstancePtr() const
{
	return m_pInstance;
}

V3DAdapter* V3DDevice::GetInternalAdapterPtr() const
{
	return m_pAdapter;
}

const V3DDevice::Source& V3DDevice::GetSource() const
{
	return m_Source;
}

void V3DDevice::NotifyReleaseSampler()
{
	V3D_ASSERT(m_Statistics.samplerCount > 0);
	--m_Statistics.samplerCount;
}

void V3DDevice::NotifyReleaseResourceMemory()
{
	V3D_ASSERT(m_Statistics.resourceMemoryCount > 0);
	--m_Statistics.resourceMemoryCount;
}

void V3DDevice::Vulkan_SetDebugMarkerObjectName(VkDebugReportObjectTypeEXT objectType, void* pObject, const wchar_t* pName)
{
	Vulkan_SetDebugMarkerObjectName(objectType, reinterpret_cast<uint64_t>(pObject), pName);
}

void V3DDevice::Vulkan_SetDebugMarkerObjectName(VkDebugReportObjectTypeEXT objectType, uint64_t object, const wchar_t* pName)
{
	if (m_pDebugMarkerSetObjectNameFunction == nullptr)
	{
		return;
	}

	V3D_ASSERT(pName != nullptr);

	STLStringA debugName;
	ToMultibyteString(pName, debugName);

	VkDebugMarkerObjectNameInfoEXT nameInfo;
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
	nameInfo.pNext = nullptr;
	nameInfo.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
	nameInfo.object = object;
	nameInfo.pObjectName = debugName.c_str();

	m_pDebugMarkerSetObjectNameFunction(m_Source.device, &nameInfo);
}

/********************************/
/* public override - IV3DDevice */
/********************************/

void V3DDevice::GetAdapter(IV3DAdapter** ppAdapter)
{
	(*ppAdapter) = V3D_TO_ADD_REF(m_pAdapter);
}

uint32_t V3DDevice::GetQueueFamilyCount() const
{
	return static_cast<uint32_t>(m_QueueFamilies.size());
}

const V3DDeviceQueueFamilyDesc& V3DDevice::GetQueueFamilyDesc(uint32_t family) const
{
	return m_QueueFamilies[family].desc;
}

void V3DDevice::GetQueue(uint32_t family, uint32_t queue, IV3DQueue** ppQueue)
{
	IV3DQueue* pQueue = m_QueueFamilies[family].queues[queue];
	*ppQueue = V3D_TO_ADD_REF(pQueue);
}

const V3DDeviceCaps& V3DDevice::GetCaps() const
{
	return m_Caps;
}

const V3DDeviceStatistics& V3DDevice::GetStatistics() const
{
	return m_Statistics;
}

V3D_RESULT V3DDevice::GetImageFormatDesc(V3D_FORMAT format, V3D_IMAGE_TYPE type, V3D_IMAGE_TILING tiling, V3DFlags usageFlags, V3DImageFormatDesc* pDesc)
{
	VkImageFormatProperties vkProperties;

	VkResult vkResult = vkGetPhysicalDeviceImageFormatProperties(
		m_pAdapter->GetSource().physicalDevice,
		ToVkFormat(format), ToVkImageType(type), ToVkImageTiling(tiling), ToVkImageUsageFlags(usageFlags), 0,
		&vkProperties);

	if ((vkResult == VK_SUCCESS) && (pDesc != nullptr))
	{
		pDesc->maxWidth = vkProperties.maxExtent.width;
		pDesc->maxHeight = vkProperties.maxExtent.height;
		pDesc->maxDepth = vkProperties.maxExtent.depth;
		pDesc->maxLevels = vkProperties.maxMipLevels;
		pDesc->maxLayers = vkProperties.maxArrayLayers;
		pDesc->sampleCountFlags = ToVkSampleCountFlags(vkProperties.sampleCounts);
	}

	return ToV3DResult(vkResult);
}

V3D_RESULT V3DDevice::CheckBufferFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const
{
	VkFormatProperties vkFormatProps;
	vkGetPhysicalDeviceFormatProperties(m_pAdapter->GetSource().physicalDevice, ToVkFormat(format), &vkFormatProps);

	VkFormatFeatureFlags vkFeatureFlags = ToVkBufferFormatFeatureFlags(featureFlags);
	if ((vkFormatProps.bufferFeatures & vkFeatureFlags) != vkFeatureFlags)
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	return V3D_OK;
}

V3D_RESULT V3DDevice::CheckImageFormatFeature(V3D_FORMAT format, V3DFlags featureFlags) const
{
	VkFormatProperties vkFormatProps;
	vkGetPhysicalDeviceFormatProperties(m_pAdapter->GetSource().physicalDevice, ToVkFormat(format), &vkFormatProps);

	VkFormatFeatureFlags vkFeatureFlags = ToVkImageFormatFeatureFlags(featureFlags);

	if ((vkFormatProps.linearTilingFeatures & vkFeatureFlags) != vkFeatureFlags)
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	if ((vkFormatProps.optimalTilingFeatures & vkFeatureFlags) != vkFeatureFlags)
	{
		return V3D_ERROR_NOT_SUPPORTED;
	}

	return V3D_OK;
}

V3D_RESULT V3DDevice::CheckImageFormatFeature(V3D_FORMAT format, V3D_IMAGE_TILING tiling, V3DFlags featureFlags) const
{
	VkFormatProperties vkFormatProps;
	vkGetPhysicalDeviceFormatProperties(m_pAdapter->GetSource().physicalDevice, ToVkFormat(format), &vkFormatProps);

	VkFormatFeatureFlags vkFeatureFlags = ToVkImageFormatFeatureFlags(featureFlags);

	if (tiling == V3D_IMAGE_TILING_LINEAR)
	{
		if ((vkFormatProps.linearTilingFeatures & vkFeatureFlags) != vkFeatureFlags)
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}
	else
	{
		if ((vkFormatProps.optimalTilingFeatures & vkFeatureFlags) != vkFeatureFlags)
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}
	}

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateCommandPool(const V3DCommandPoolDesc& desc, IV3DCommandPool** ppCommandPool, const wchar_t* pDebugName)
{
	V3DCommandPool* pCommandPool = V3DCommandPool::Create();
	if (pCommandPool == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pCommandPool->Initialize(this, desc, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pCommandPool);
		return result;
	}

	(*ppCommandPool) = pCommandPool;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateCommandBuffer(IV3DCommandPool* pCommandPool, V3D_COMMAND_BUFFER_TYPE commandBufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName)
{
	if (pCommandPool == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DCommandBuffer* pCommandBuffer = V3DCommandBuffer::Create();
	if (pCommandBuffer == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pCommandBuffer->Initialize(this, pCommandPool, commandBufferType, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pCommandBuffer);
		return result;
	}

	(*ppCommandBuffer) = pCommandBuffer;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateCommandBuffer(const V3DCommandPoolDesc& poolDesc, V3D_COMMAND_BUFFER_TYPE bufferType, IV3DCommandBuffer** ppCommandBuffer, const wchar_t* pDebugName)
{
	V3DCommandPool* pCommandPool = V3DCommandPool::Create();
	if (pCommandPool == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pCommandPool->Initialize(this, poolDesc, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pCommandPool);
		return result;
	}

	V3DCommandBuffer* pCommandBuffer = V3DCommandBuffer::Create();
	if (pCommandBuffer == nullptr)
	{
		V3D_RELEASE(pCommandPool);
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	result = pCommandBuffer->Initialize(this, pCommandPool, bufferType, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pCommandBuffer);
		V3D_RELEASE(pCommandPool);
		return result;
	}

	V3D_RELEASE(pCommandPool);

	(*ppCommandBuffer) = pCommandBuffer;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateSwapChain(const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, IV3DSwapChain** ppSwapChain, const wchar_t* pDebugName)
{
	if ((swapChainDesc.windowHandle == nullptr) ||
		(swapChainDesc.imageWidth == 0) || (swapChainDesc.imageHeight == 0) || (swapChainDesc.imageCount < 2) ||
		(swapChainDesc.queueWaitDstStageMask == 0))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DSwapChain* pSwapChain = V3DSwapChain::Create();
	if (pSwapChain == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pSwapChain->Initialize(this, swapChainDesc, swapChainCallbacks, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pSwapChain);
		return result;
	}

	(*ppSwapChain) = pSwapChain;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateFence(IV3DFence** ppFence, const wchar_t* pDebugName)
{
	V3DFence* pFence = V3DFence::Create();
	if (pFence == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pFence->Initialize(this, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pFence);
		return result;
	}

	(*ppFence) = pFence;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateQueryPool(const V3DQueryPoolDesc& desc, IV3DQueryPool** ppQueryPool, const wchar_t* pDebugName)
{
	if (desc.queryCount == 0)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DQueryPool* pQueryPool = V3DQueryPool::Create();
	if (pQueryPool == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pQueryPool->Initialize(this, desc, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pQueryPool);
		return result;
	}

	(*ppQueryPool) = pQueryPool;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateSampler(const V3DSamplerDesc& desc, IV3DSampler** ppSampler, const wchar_t* pDebugName)
{
	V3DSampler* pSampler = V3DSampler::Create();
	if (pSampler == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pSampler->Initialize(this, desc, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppSampler) = pSampler;

	// 統計
	++m_Statistics.samplerCount;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateShaderModule(size_t codeSize, const void* pCode, IV3DShaderModule** ppShaderModule, const wchar_t* pDebugName)
{
	if ((codeSize == 0) || ((codeSize % 4) != 0) || (pCode == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DShaderModule* pShaderModule = V3DShaderModule::Create();
	if (pShaderModule == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pShaderModule->Initialize(this, codeSize, pCode, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pShaderModule);
		return result;
	}

	(*ppShaderModule) = pShaderModule;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, IV3DResource* pResource) const
{
	if ((memoryPropertyFlags == 0) || (pResource == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const VkPhysicalDeviceMemoryProperties& vkMemoryProps = m_pAdapter->GetSource().physicalDeviceMemoryProps;
	VkMemoryPropertyFlags vkMemoryPropFlags = ToVkMemoryPropertyFlags(memoryPropertyFlags);

	for (uint32_t i = 0; i < vkMemoryProps.memoryTypeCount; ++i)
	{
		if ((vkMemoryProps.memoryTypes[i].propertyFlags & vkMemoryPropFlags) == vkMemoryPropFlags)
		{
			uint32_t memoryTypeBits = 1 << i;

			if ((pResource->GetResourceDesc().memoryTypeBits & memoryTypeBits) == memoryTypeBits)
			{
				return V3D_OK;
			}
		}
	}

	return V3D_ERROR_NOT_SUPPORTED;
}

V3D_RESULT V3DDevice::CheckResourceMemoryProperty(V3DFlags memoryPropertyFlags, uint32_t resourceCount, IV3DResource** ppResources) const
{
	if ((memoryPropertyFlags == 0) || (resourceCount == 0) || (ppResources == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const VkPhysicalDeviceMemoryProperties& vkMemoryProps = m_pAdapter->GetSource().physicalDeviceMemoryProps;
	VkMemoryPropertyFlags vkMemoryPropFlags = ToVkMemoryPropertyFlags(memoryPropertyFlags);

	for (uint32_t i = 0; i < vkMemoryProps.memoryTypeCount; ++i)
	{
		if ((vkMemoryProps.memoryTypes[i].propertyFlags & vkMemoryPropFlags) == vkMemoryPropFlags)
		{
			uint32_t memoryTypeBits = 1 << i;
			uint32_t matchCount = 0;

			for (uint32_t j = 0; j < resourceCount; j++)
			{
				if (ppResources[j]->GetResourceDesc().memoryTypeBits & memoryTypeBits)
				{
					matchCount++;
				}
				else
				{
					break;
				}
			}

			if (resourceCount == matchCount)
			{
				return V3D_OK;
			}
		}
	}

	return V3D_ERROR_NOT_SUPPORTED;
}

V3D_RESULT V3DDevice::AllocateResourceMemory(V3DFlags propertyFlags, uint64_t size, IV3DResourceMemory** ppResourceMemory, const wchar_t* pDebugName)
{
	if ((propertyFlags == 0) || (size == 0))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DResourceMemory* pResourceMemory = V3DResourceMemory::Create();
	if (pResourceMemory == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pResourceMemory->Initialize(this, propertyFlags, size, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppResourceMemory) = pResourceMemory;

	// 統計
	++m_Statistics.resourceMemoryCount;

	return V3D_OK;
}

V3D_RESULT V3DDevice::BindResourceMemory(IV3DResourceMemory* pResourceMemory, IV3DResource* pResource, uint64_t memoryOffset)
{
	if ((pResourceMemory == nullptr) || (pResource == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DResourceMemory* pInternalResourceMemory = static_cast<V3DResourceMemory*>(pResourceMemory);
	V3D_RESULT result = V3D_ERROR_FAIL;

	switch (pResource->GetResourceDesc().type)
	{
	case V3D_RESOURCE_TYPE_BUFFER:
		result = static_cast<V3DBuffer*>(pResource)->BindMemory(pInternalResourceMemory, memoryOffset);
		break;
	case V3D_RESOURCE_TYPE_IMAGE:
		result = static_cast<IV3DImageBase*>(pResource)->BindMemory(pInternalResourceMemory, memoryOffset);
		break;
	}

	return result;
}

V3D_RESULT V3DDevice::AllocateResourceMemoryAndBind(V3DFlags propertyFlags, IV3DResource* pResource, const wchar_t* pDebugName)
{
	if ((propertyFlags == 0) || (pResource == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DResourceMemory* pResourceMemory = V3DResourceMemory::Create();
	if (pResourceMemory == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	const V3DResourceDesc& resourceDesc = pResource->GetResourceDesc();

	V3D_RESULT result = pResourceMemory->Initialize(this, propertyFlags, resourceDesc.memorySize, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pResourceMemory);
		return result;
	}

	switch (resourceDesc.type)
	{
	case V3D_RESOURCE_TYPE_BUFFER:
		result = static_cast<V3DBuffer*>(pResource)->BindMemory(pResourceMemory, 0);
		break;
	case V3D_RESOURCE_TYPE_IMAGE:
		result = static_cast<IV3DImageBase*>(pResource)->BindMemory(pResourceMemory, 0);
		break;
	}

	V3D_RELEASE(pResourceMemory);

	if (result == V3D_OK)
	{
		// 統計
		++m_Statistics.resourceMemoryCount;
	}

	return result;
}

V3D_RESULT V3DDevice::AllocateResourceMemoryAndBind(V3DFlags propertyFlags, uint32_t resourceCount, IV3DResource** ppResources, const wchar_t* pDebugName)
{
	if ((propertyFlags == 0) || (resourceCount == 0) || (ppResources == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DResourceMemory* pResourceMemory = V3DResourceMemory::Create();
	if (pResourceMemory == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pResourceMemory->Initialize(this, propertyFlags, resourceCount, ppResources, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pResourceMemory);
		return result;
	}

	V3D_RELEASE(pResourceMemory);

	if (result == V3D_OK)
	{
		// 統計
		++m_Statistics.resourceMemoryCount;
	}

	return result;
}

V3D_RESULT V3DDevice::CreateBuffer(const V3DBufferDesc& desc, IV3DBuffer** ppBuffer, const wchar_t* pDebugName)
{
	if ((desc.usageFlags == 0) || (desc.size == 0))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DBuffer* pBuffer = V3DBuffer::Create();
	if (pBuffer == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pBuffer->Initialize(this, desc, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppBuffer) = pBuffer;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateImage(const V3DImageDesc& imageDesc, V3D_IMAGE_LAYOUT initialLayout, IV3DImage** ppImage, const wchar_t* pDebugName)
{
	if ((initialLayout != V3D_IMAGE_LAYOUT_UNDEFINED) && (initialLayout != V3D_IMAGE_LAYOUT_PREINITIALIZED))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DImage* pImage = V3DImage::Create();
	if (pImage == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pImage->Initialize(this, imageDesc, initialLayout, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pImage);
		return result;
	}

	(*ppImage) = pImage;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateBufferView(IV3DBuffer* pBuffer, const V3DBufferViewDesc& desc, IV3DBufferView** ppBufferView, const wchar_t* pDebugName)
{
	if ((pBuffer == nullptr) ||	(desc.format == V3D_FORMAT_UNDEFINED))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DBufferDesc& bufferDesc = pBuffer->GetDesc();

	if (((bufferDesc.usageFlags & (V3D_BUFFER_USAGE_UNIFORM_TEXEL | V3D_BUFFER_USAGE_STORAGE_TEXEL)) == 0) ||
		(bufferDesc.size < (desc.offset + desc.size)))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DBufferView* pBufferView = V3DBufferView::Create();
	if (pBufferView == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pBufferView->Initialize(this, pBuffer, desc, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppBufferView) = pBufferView;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateImageView(IV3DImage* pImage, const V3DImageViewDesc& desc, IV3DImageView** ppImageView, const wchar_t* pDebugName)
{
	if ((pImage == nullptr) || (desc.levelCount == 0) || (desc.layerCount == 0))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	const V3DImageDesc& imageDesc = pImage->GetDesc();
	if ((imageDesc.levelCount < (desc.baseLevel + desc.levelCount)) || (imageDesc.layerCount < (desc.baseLayer + desc.layerCount)))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DImageView* pImageView = V3DImageView::Create();
	if (pImageView == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pImageView->Initialize(this, pImage, desc, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppImageView) = pImageView;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateRenderPass(uint32_t attachmentCount, const V3DAttachmentDesc* pAttachments, uint32_t subpassCount, const V3DSubpassDesc* pSubpasses, uint32_t subpassDependencyCount, const V3DSubpassDependencyDesc* pSubpassDependencies, IV3DRenderPass** ppRenderPass, const wchar_t* pDebugName)
{
	if ((attachmentCount == 0) || (pAttachments == nullptr) ||
		(subpassCount == 0) || (pSubpasses == nullptr) ||
		((subpassDependencyCount == 0) && (pSubpassDependencies != nullptr)) ||
		((subpassDependencyCount > 0) && (pSubpassDependencies == nullptr)))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DRenderPass* pRenderPass = V3DRenderPass::Create();
	if (pRenderPass == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pRenderPass->Initialize(this, attachmentCount, pAttachments, subpassCount, pSubpasses, subpassDependencyCount, pSubpassDependencies, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pRenderPass);
		return result;
	}

	(*ppRenderPass) = pRenderPass;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateFrameBuffer(IV3DRenderPass* pRenderPass, uint32_t attachmentCount, IV3DImageView** ppAttachments, IV3DFrameBuffer** ppFrameBuffer, const wchar_t* pDebugName)
{
	if ((pRenderPass == nullptr) || (attachmentCount == 0) || (ppAttachments == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DFrameBuffer* pFrameBuffer = V3DFrameBuffer::Create();
	if (pFrameBuffer == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pFrameBuffer->Initialize(this, pRenderPass, attachmentCount, ppAttachments, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pFrameBuffer);
		return result;
	}

	(*ppFrameBuffer) = pFrameBuffer;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateDescriptorSetLayout(uint32_t descriptorCount, const V3DDescriptorDesc* pDescriptors, uint32_t poolSize, uint32_t poolResizeStep, IV3DDescriptorSetLayout** ppDescriptorSetLayout, const wchar_t* pDebugName)
{
	if ((descriptorCount == 0) || (pDescriptors == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	if ((poolSize == 0) && (poolResizeStep == 0))
	{
		if ((m_Caps.extensionFlags & V3D_DEVICE_EXTENSION_PUSH_DESCRIPTOR_SETS) == 0)
		{
			return V3D_ERROR_NOT_SUPPORTED;
		}

		V3DPushDescriptorSetLayout* pDescriptorSetLayout = V3DPushDescriptorSetLayout::Create();
		if (pDescriptorSetLayout == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pDescriptorSetLayout->Initialize(this, descriptorCount, pDescriptors, pDebugName);
		if (result != V3D_OK)
		{
			return result;
		}

		(*ppDescriptorSetLayout) = pDescriptorSetLayout;
	}
	else
	{
		V3DStandardDescriptorSetLayout* pDescriptorSetLayout = V3DStandardDescriptorSetLayout::Create();
		if (pDescriptorSetLayout == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pDescriptorSetLayout->Initialize(this, descriptorCount, pDescriptors, poolSize, poolResizeStep, pDebugName);
		if (result != V3D_OK)
		{
			return result;
		}

		(*ppDescriptorSetLayout) = pDescriptorSetLayout;
	}

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet, const wchar_t* pDebugName)
{
	if (pLayout == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3D_DESCRIPTOR_SET_TYPE type = pLayout->GetType();

	if (type == V3D_DESCRIPTOR_SET_TYPE_STANDARD)
	{
		V3DStandardDescriptorSet* pDescriptorSet = V3DStandardDescriptorSet::Create();
		if (pDescriptorSet == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pDescriptorSet->Initialize(this, pLayout, pDebugName);
		if (result != V3D_OK)
		{
			return result;
		}

		(*ppDescriptorSet) = pDescriptorSet;
	}
	else if (type == V3D_DESCRIPTOR_SET_TYPE_PUSH)
	{
		V3DPushDescriptorSet* pDescriptorSet = V3DPushDescriptorSet::Create();
		if (pDescriptorSet == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		V3D_RESULT result = pDescriptorSet->Initialize(this, pLayout, pDebugName);
		if (result != V3D_OK)
		{
			return result;
		}

		(*ppDescriptorSet) = pDescriptorSet;
	}
	else
	{
		V3D_ASSERT(0);
	}

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreatePipelineLayout(uint32_t constantCount, V3DConstantDesc* pConstants, uint32_t descriptorSetLayoutCount, IV3DDescriptorSetLayout** ppDescriptorSetLayouts, IV3DPipelineLayout** ppPipelineLayout, const wchar_t* pDebugName)
{
	if (((constantCount > 0) && (pConstants == nullptr)) || ((descriptorSetLayoutCount > 0) && (ppDescriptorSetLayouts == nullptr)))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DPipelineLayout* pPipelineLayout = V3DPipelineLayout::Create();
	if (ppPipelineLayout == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pPipelineLayout->Initialize(this, constantCount, pConstants, descriptorSetLayoutCount, ppDescriptorSetLayouts, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pPipelineLayout);
		return result;
	}

	(*ppPipelineLayout) = pPipelineLayout;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateGraphicsPipeline(IV3DPipelineLayout* pPipelineLayout, const V3DGraphicsPipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName)
{
	if ((pPipelineLayout == nullptr) || (pipelineDesc.pRenderPass == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DGraphicsPipeline* pPipeline = V3DGraphicsPipeline::Create();
	if (pPipeline == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pPipeline->Initialize(this, pPipelineLayout, pipelineDesc, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pPipeline);
		return result;
	}

	(*ppPipeline) = pPipeline;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateComputePipeline(IV3DPipelineLayout* pPipelineLayout, const V3DComputePipelineDesc& pipelineDesc, IV3DPipeline** ppPipeline, const wchar_t* pDebugName)
{
	if ((pPipelineLayout == nullptr) || (pipelineDesc.computeShader.pModule == nullptr) || (pipelineDesc.computeShader.pEntryPointName == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DComputePipeline* pPipeline = V3DComputePipeline::Create();
	if (pPipeline == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pPipeline->Initialize(this, pPipelineLayout, pipelineDesc, pDebugName);
	if (result != V3D_OK)
	{
		V3D_RELEASE(pPipeline);
		return result;
	}

	(*ppPipeline) = pPipeline;

	return V3D_OK;
}

V3D_RESULT V3DDevice::WaitIdle()
{
	VkResult vkResult = vkDeviceWaitIdle(m_Source.device);
	return ToV3DResult(vkResult);
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DDevice::GetRefCount() const
{
	return m_RefCounter;
}

void V3DDevice::AddRef()
{
	V3D_REF_INC(m_RefCounter);
}

void V3DDevice::Release()
{
	if (V3D_REF_DEC(m_RefCounter))
	{
		V3D_REF_FENCE();
		V3D_DELETE_THIS_T(this, V3DDevice);
	}
}

/***********************/
/* private - V3DDevice */
/***********************/

V3DDevice::V3DDevice() :
	m_RefCounter(1),
	m_pInstance(nullptr),
	m_pAdapter(nullptr),
	m_Caps({}),
	m_Statistics({}),
	m_pDebugMarkerSetObjectNameFunction(nullptr)
{
	m_Source.device = VK_NULL_HANDLE;
}

V3DDevice::~V3DDevice()
{
	if (m_Source.device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_Source.device, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pInstance, m_Source.device);
	}

	for (size_t i = 0; i < m_QueueFamilies.size(); i++)
	{
		V3DDevice::QueueFamily& queueFamily = m_QueueFamilies[i];

		for (size_t j = 0; j < queueFamily.queues.size(); j++)
		{
			V3DQueue* queue = queueFamily.queues[j];
			queue->Dispose();
			V3D_RELEASE(queue);
		}
	}

	V3D_RELEASE(m_pAdapter);
	V3D_RELEASE(m_pInstance);
}
