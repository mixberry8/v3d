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
#include "V3DDescriptorSetLayout.h"
#include "V3DDescriptorSet.h"
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
	if (m_pInstance->GetLayerType() == V3D_LAYER_STANDARD_VALIDATION)
	{
		if (std::find_if(vkLayerProps.begin(), vkLayerProps.end(), V3DFindLayer(V3D_LAYER_LUNARG_standard_validation)) != vkLayerProps.end())
		{
			vkEnableLayers.push_back(V3D_LAYER_LUNARG_standard_validation);
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// エクステンションの列挙
	// ----------------------------------------------------------------------------------------------------

	uint32_t vkExtensionCount;
	vkResult = vkEnumerateDeviceExtensionProperties(adapterSource.physicalDevice, nullptr, &vkExtensionCount, nullptr);
	if (vkResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<VkExtensionProperties> vkExtensionProps;
	vkExtensionProps.resize(vkExtensionCount);
	vkResult = vkEnumerateDeviceExtensionProperties(adapterSource.physicalDevice, nullptr, &vkExtensionCount, vkExtensionProps.data());
	if (vkResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	STLVector<char*> vkEnableExtensions;
	vkEnableExtensions.resize(vkExtensionCount);
	for (uint32_t i = 0; i < vkExtensionCount; i++)
	{
		vkEnableExtensions[i] = vkExtensionProps[i].extensionName;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスの特性を取得
	// ----------------------------------------------------------------------------------------------------

	vkGetPhysicalDeviceProperties(m_pAdapter->GetSource().physicalDevice, &m_Source.deviceProps);
	vkGetPhysicalDeviceFeatures(m_pAdapter->GetSource().physicalDevice, &m_Source.deviceFeatures);

	// ----------------------------------------------------------------------------------------------------
	// キューファミリーの取得
	// キューの作成情報リストを作成
	// ----------------------------------------------------------------------------------------------------

	uint32_t vkQueueFamilyPropCount;
	vkGetPhysicalDeviceQueueFamilyProperties(adapterSource.physicalDevice, &vkQueueFamilyPropCount, nullptr);

	STLVector<VkQueueFamilyProperties> vkQueueFamilyProps;
	vkQueueFamilyProps.resize(vkQueueFamilyPropCount);
	vkGetPhysicalDeviceQueueFamilyProperties(adapterSource.physicalDevice, &vkQueueFamilyPropCount, vkQueueFamilyProps.data());

	uint32_t totalQueueCount = 0;
	for (uint32_t i = 0; i < vkQueueFamilyPropCount; i++)
	{
		const VkQueueFamilyProperties& vkProps = vkQueueFamilyProps[i];
		totalQueueCount += vkProps.queueCount;
	}

	STLVector<float> vkQueuePriorities;
	vkQueuePriorities.resize(totalQueueCount, 0.0f);

	STLVector<VkDeviceQueueCreateInfo> vkQueueCreateInfos;
	vkQueueCreateInfos.reserve(vkQueueFamilyPropCount);

	uint32_t firstQueuePriority = 0;

	m_QueueFamilies.reserve(vkQueueFamilyPropCount);
	for (uint32_t i = 0; i < vkQueueFamilyPropCount; i++)
	{
		const VkQueueFamilyProperties& vkProps = vkQueueFamilyProps[i];

		V3DDevice::QueueFamily queueFamily{};
		queueFamily.desc.queueFlags = ToV3DQueueFlags(vkProps.queueFlags);
		queueFamily.desc.queueCount = vkProps.queueCount;
		m_QueueFamilies.push_back(queueFamily);

		VkDeviceQueueCreateInfo vkQueueCreateInfo{};
		vkQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		vkQueueCreateInfo.pNext = nullptr;
		vkQueueCreateInfo.flags = 0;
		vkQueueCreateInfo.queueCount = vkProps.queueCount;
		vkQueueCreateInfo.queueFamilyIndex = i;
		vkQueueCreateInfo.pQueuePriorities = &vkQueuePriorities[firstQueuePriority];
		vkQueueCreateInfos.push_back(vkQueueCreateInfo);

		firstQueuePriority += vkProps.queueCount;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスを作成
	// ----------------------------------------------------------------------------------------------------

	VkDeviceCreateInfo devInfo;
	devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devInfo.pNext = nullptr;
	devInfo.flags = 0;
	devInfo.queueCreateInfoCount = static_cast<uint32_t>(vkQueueCreateInfos.size());
	devInfo.pQueueCreateInfos = vkQueueCreateInfos.data();
	devInfo.enabledLayerCount = static_cast<uint32_t>(vkEnableLayers.size());
	devInfo.ppEnabledLayerNames = vkEnableLayers.data();
	devInfo.enabledExtensionCount = static_cast<uint32_t>(vkEnableExtensions.size());
	devInfo.ppEnabledExtensionNames = vkEnableExtensions.data();
	devInfo.pEnabledFeatures = &m_Source.deviceFeatures;

	vkResult = vkCreateDevice(adapterSource.physicalDevice, &devInfo, nullptr, &m_Source.device);
	if (vkResult != VK_SUCCESS)
	{
		return V3D_ERROR_FAIL;
	}

	V3D_ADD_DEBUG_OBJECT(m_pInstance, m_Source.device, pDebugName);

	// ----------------------------------------------------------------------------------------------------
	// キューを取得
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < vkQueueFamilyPropCount; i++)
	{
		V3DDevice::QueueFamily& queueFamily = m_QueueFamilies[i];
		queueFamily.queues.reserve(queueFamily.desc.queueCount);

		for (uint32_t j = 0; j < queueFamily.desc.queueCount; j++)
		{
			VkQueue srcQueue;
			vkGetDeviceQueue(m_Source.device, i, j, &srcQueue);

			V3DQueue* pQueue = V3DQueue::Create();
			if (pQueue == nullptr)
			{
				return V3D_ERROR_OUT_OF_HOST_MEMORY;
			}

			V3D_RESULT result = pQueue->Initialize(this, i, srcQueue);
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

	const VkPhysicalDeviceFeatures vkPDFeatures = m_Source.deviceFeatures;

	if (vkPDFeatures.fullDrawIndexUint32 == VK_TRUE) { m_Caps.flags |= V3D_CAP_FULL_DRAW_INDEX_UINT32; }
	if (vkPDFeatures.samplerAnisotropy == VK_TRUE) { m_Caps.flags |= V3D_CAP_SAMPLER_ANISOTROPY; }

	if (vkPDFeatures.occlusionQueryPrecise == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_OCCLUSION_QUERY_PRECISE; }
	if (vkPDFeatures.pipelineStatisticsQuery == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_PIPELINE_STATISTICS_QUERY; }
	if (vkPDFeatures.inheritedQueries == VK_TRUE) { m_Caps.queryFlags |= V3D_QUERY_CAP_INHERITED_QUERIES; }

	if (vkPDFeatures.imageCubeArray == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_CUBE_ARRAY; }
	if (vkPDFeatures.textureCompressionETC2 == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_ETC2; }
	if (vkPDFeatures.textureCompressionASTC_LDR == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_ASTC_LDR; }
	if (vkPDFeatures.textureCompressionBC == VK_TRUE) { m_Caps.imageFlags |= V3D_IMAGE_CAP_COMPRESSION_BC; }

	if (vkPDFeatures.geometryShader == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_GEOMETRY; }
	if (vkPDFeatures.tessellationShader == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_TESSELLATION; }
	if (vkPDFeatures.robustBufferAccess == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_ROBUST_BUFFER_ACCESS; }
	if (vkPDFeatures.vertexPipelineStoresAndAtomics == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_VERTEX_PIPELINE_STORES_AND_ATOMICS; }
	if (vkPDFeatures.fragmentStoresAndAtomics == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_FRAGMENT_STORES_AND_ATOMICS; }
	if (vkPDFeatures.largePoints == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_LARGE_POINTS; }
	if (vkPDFeatures.shaderTessellationAndGeometryPointSize == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_TESSELLATION_AND_GEOMETRY_POINTSIZE; }
	if (vkPDFeatures.shaderImageGatherExtended == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_IMAGE_GATHER_EXTENDED; }
	if (vkPDFeatures.shaderStorageImageExtendedFormats == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_EXTENDED_FORMATS; }
	if (vkPDFeatures.shaderStorageImageMultisample == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_MULTISAMPLE; }
	if (vkPDFeatures.shaderStorageImageReadWithoutFormat == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_READ_WITH_OUT_FORMAT; }
	if (vkPDFeatures.shaderStorageImageWriteWithoutFormat == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_WRITE_WITH_OUT_FORMAT; }
	if (vkPDFeatures.shaderUniformBufferArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_UNIFORM_BUFFER_ARRAY_DYNAMIC_INDEXING; }
	if (vkPDFeatures.shaderSampledImageArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_SAMPLED_IMAGE_ARRAY_DYNAMIC_INDEXING; }
	if (vkPDFeatures.shaderStorageBufferArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_BUFFER_ARRAY_DYNAMIC_INDEXING; }
	if (vkPDFeatures.shaderStorageImageArrayDynamicIndexing == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_STORAGE_IMAGE_ARRAY_DYNAMIC_INDEXING; }
	if (vkPDFeatures.shaderClipDistance == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_CLIP_DISTANCE; }
	if (vkPDFeatures.shaderCullDistance == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_CULL_DISTANCE; }
	if (vkPDFeatures.shaderFloat64 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_FLOAT64; }
	if (vkPDFeatures.shaderInt64 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_INT64; }
	if (vkPDFeatures.shaderInt16 == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_INT16; }
	if (vkPDFeatures.shaderResourceResidency == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_RESOURCE_RESIDENCY; }
	if (vkPDFeatures.shaderResourceMinLod == VK_TRUE) { m_Caps.shaderFlags |= V3D_SHADER_CAP_RESOURCE_MIN_LOD; }

	if (vkPDFeatures.multiViewport == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_MULTI_VIEWPORT; }
	if (vkPDFeatures.fillModeNonSolid == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_FILL_MODE_NON_SOLID; }
	if (vkPDFeatures.depthClamp == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_DEPTH_CLAMP; }
	if (vkPDFeatures.depthBiasClamp == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_RASTERIZER_CAP_DEPTH_BIAS_CLAMP; }

	if (vkPDFeatures.variableMultisampleRate == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_VARIABLE_RATE; }
	if (vkPDFeatures.sampleRateShading == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_SAMPLE_RATE_SHADING; }
	if (vkPDFeatures.alphaToOne == VK_TRUE) { m_Caps.multisampleFlags |= V3D_MULTISAMPLE_CAP_ALPHA_TO_ONE; }

	if (vkPDFeatures.depthBounds == VK_TRUE) { m_Caps.depthStencilFlags |= V3D_DEPTH_STENCIL_CAP_DEPTH_BOUNDS; }

	if (vkPDFeatures.independentBlend == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_INDEPENDENT; }
	if (vkPDFeatures.dualSrcBlend == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_DUAL_SRC; }
	if (vkPDFeatures.logicOp == VK_TRUE) { m_Caps.colorBlendFlags |= V3D_COLOR_BLEND_CAP_LOGIC_OP; }

	const VkPhysicalDeviceLimits& vkPDLimits = m_Source.deviceProps.limits;

	m_Caps.maxImageDimension1D = vkPDLimits.maxImageDimension1D;
	m_Caps.maxImageDimension2D = vkPDLimits.maxImageDimension2D;
	m_Caps.maxImageDimension3D = vkPDLimits.maxImageDimension3D;
	m_Caps.maxImageDimensionCube = vkPDLimits.maxImageDimensionCube;
	m_Caps.maxImageArrayLayers = vkPDLimits.maxImageArrayLayers;
	m_Caps.maxTexelBufferElements = vkPDLimits.maxTexelBufferElements;
	m_Caps.maxUniformBufferSize = vkPDLimits.maxUniformBufferRange;
	m_Caps.maxStorageBufferSize = vkPDLimits.maxStorageBufferRange;
	m_Caps.maxPushConstantsSize = vkPDLimits.maxPushConstantsSize;
	m_Caps.maxResourceMemoryCount = vkPDLimits.maxMemoryAllocationCount;
	m_Caps.maxSamplerCreateCount = vkPDLimits.maxSamplerAllocationCount;
	m_Caps.bufferImageGranularity = vkPDLimits.bufferImageGranularity;
	m_Caps.maxBoundDescriptorSets = vkPDLimits.maxBoundDescriptorSets;
	m_Caps.maxPerStageDescriptorSamplers = vkPDLimits.maxPerStageDescriptorSamplers;
	m_Caps.maxPerStageDescriptorUniformBuffers = vkPDLimits.maxPerStageDescriptorUniformBuffers;
	m_Caps.maxPerStageDescriptorStorageBuffers = vkPDLimits.maxPerStageDescriptorStorageBuffers;
	m_Caps.maxPerStageDescriptorSampledImages = vkPDLimits.maxPerStageDescriptorSampledImages;
	m_Caps.maxPerStageDescriptorStorageImages = vkPDLimits.maxPerStageDescriptorStorageImages;
	m_Caps.maxPerStageDescriptorInputAttachments = vkPDLimits.maxPerStageDescriptorInputAttachments;
	m_Caps.maxPerStageResources = vkPDLimits.maxPerStageResources;
	m_Caps.maxDescriptorSetSamplers = vkPDLimits.maxDescriptorSetSamplers;
	m_Caps.maxDescriptorSetUniformBuffers = vkPDLimits.maxDescriptorSetUniformBuffers;
	m_Caps.maxDescriptorSetUniformBuffersDynamic = vkPDLimits.maxDescriptorSetUniformBuffersDynamic;
	m_Caps.maxDescriptorSetStorageBuffers = vkPDLimits.maxDescriptorSetStorageBuffers;
	m_Caps.maxDescriptorSetStorageBuffersDynamic = vkPDLimits.maxDescriptorSetStorageBuffersDynamic;
	m_Caps.maxDescriptorSetSampledImages = vkPDLimits.maxDescriptorSetSampledImages;
	m_Caps.maxDescriptorSetStorageImages = vkPDLimits.maxDescriptorSetStorageImages;
	m_Caps.maxDescriptorSetInputAttachments = vkPDLimits.maxDescriptorSetInputAttachments;
	m_Caps.maxVertexInputElements = vkPDLimits.maxVertexInputAttributes;
	m_Caps.maxVertexInputBindings = vkPDLimits.maxVertexInputBindings;
	m_Caps.maxVertexInputElementOffset = vkPDLimits.maxVertexInputAttributeOffset;
	m_Caps.maxVertexInputBindingStride = vkPDLimits.maxVertexInputBindingStride;
	m_Caps.maxVertexOutputComponents = vkPDLimits.maxVertexOutputComponents;
	m_Caps.maxTessellationGenerationLevel = vkPDLimits.maxTessellationGenerationLevel;
	m_Caps.maxTessellationPatchSize = vkPDLimits.maxTessellationPatchSize;
	m_Caps.maxTessellationControlPerVertexInputComponents = vkPDLimits.maxTessellationControlPerVertexInputComponents;
	m_Caps.maxTessellationControlPerVertexOutputComponents = vkPDLimits.maxTessellationControlPerVertexOutputComponents;
	m_Caps.maxTessellationControlPerPatchOutputComponents = vkPDLimits.maxTessellationControlPerPatchOutputComponents;
	m_Caps.maxTessellationControlTotalOutputComponents = vkPDLimits.maxTessellationControlTotalOutputComponents;
	m_Caps.maxTessellationEvaluationInputComponents = vkPDLimits.maxTessellationEvaluationInputComponents;
	m_Caps.maxTessellationEvaluationOutputComponents = vkPDLimits.maxTessellationEvaluationOutputComponents;
	m_Caps.maxGeometryShaderInvocations = vkPDLimits.maxGeometryShaderInvocations;
	m_Caps.maxGeometryInputComponents = vkPDLimits.maxGeometryInputComponents;
	m_Caps.maxGeometryOutputComponents = vkPDLimits.maxGeometryOutputComponents;
	m_Caps.maxGeometryOutputVertices = vkPDLimits.maxGeometryOutputVertices;
	m_Caps.maxGeometryTotalOutputComponents = vkPDLimits.maxGeometryTotalOutputComponents;
	m_Caps.maxFragmentInputComponents = vkPDLimits.maxFragmentInputComponents;
	m_Caps.maxFragmentOutputAttachments = vkPDLimits.maxFragmentOutputAttachments;
	m_Caps.maxFragmentDualSrcAttachments = vkPDLimits.maxFragmentDualSrcAttachments;
	m_Caps.maxFragmentCombinedOutputResources = vkPDLimits.maxFragmentCombinedOutputResources;
	m_Caps.maxComputeSharedMemorySize = vkPDLimits.maxComputeSharedMemorySize;
	m_Caps.maxComputeDispatchGroupCount[0] = vkPDLimits.maxComputeWorkGroupCount[0];
	m_Caps.maxComputeDispatchGroupCount[1] = vkPDLimits.maxComputeWorkGroupCount[1];
	m_Caps.maxComputeDispatchGroupCount[2] = vkPDLimits.maxComputeWorkGroupCount[2];
	m_Caps.maxComputeDispatchGroupInvocations = vkPDLimits.maxComputeWorkGroupInvocations;
	m_Caps.maxComputeDispatchGroupSize[0] = vkPDLimits.maxComputeWorkGroupSize[0];
	m_Caps.maxComputeDispatchGroupSize[1] = vkPDLimits.maxComputeWorkGroupSize[1];
	m_Caps.maxComputeDispatchGroupSize[2] = vkPDLimits.maxComputeWorkGroupSize[2];
	m_Caps.subPixelPrecisionBits = vkPDLimits.subPixelPrecisionBits;
	m_Caps.subTexelPrecisionBits = vkPDLimits.subTexelPrecisionBits;
	m_Caps.mipmapPrecisionBits = vkPDLimits.mipmapPrecisionBits;
	m_Caps.maxDrawIndexedIndexValue = vkPDLimits.maxDrawIndexedIndexValue;
	m_Caps.maxSamplerLodBias = vkPDLimits.maxSamplerLodBias;
	m_Caps.maxSamplerAnisotropy = vkPDLimits.maxSamplerAnisotropy;
	m_Caps.maxViewports = vkPDLimits.maxViewports;
	m_Caps.maxViewportDimensions[0] = vkPDLimits.maxViewportDimensions[0];
	m_Caps.maxViewportDimensions[1] = vkPDLimits.maxViewportDimensions[1];
	m_Caps.viewportBoundsRange[0] = vkPDLimits.viewportBoundsRange[0];
	m_Caps.viewportBoundsRange[1] = vkPDLimits.viewportBoundsRange[1];
	m_Caps.viewportSubPixelBits = vkPDLimits.viewportSubPixelBits;
	m_Caps.minMemoryMapAlignment = vkPDLimits.minMemoryMapAlignment;
	m_Caps.minTexelBufferOffsetAlignment = vkPDLimits.minTexelBufferOffsetAlignment;
	m_Caps.minUniformBufferOffsetAlignment = vkPDLimits.minUniformBufferOffsetAlignment;
	m_Caps.minStorageBufferOffsetAlignment = vkPDLimits.minStorageBufferOffsetAlignment;
	m_Caps.minTexelOffset = vkPDLimits.minTexelOffset;
	m_Caps.maxTexelOffset = vkPDLimits.maxTexelOffset;
	m_Caps.minTexelGatherOffset = vkPDLimits.minTexelGatherOffset;
	m_Caps.maxTexelGatherOffset = vkPDLimits.maxTexelGatherOffset;
	m_Caps.minInterpolationOffset = vkPDLimits.minInterpolationOffset;
	m_Caps.maxInterpolationOffset = vkPDLimits.maxInterpolationOffset;
	m_Caps.subPixelInterpolationOffsetBits = vkPDLimits.subPixelInterpolationOffsetBits;
	m_Caps.maxFrameBufferWidth = vkPDLimits.maxFramebufferWidth;
	m_Caps.maxFrameBufferHeight = vkPDLimits.maxFramebufferHeight;
	m_Caps.maxFrameBufferLayers = vkPDLimits.maxFramebufferLayers;
	m_Caps.framebufferColorSampleCounts = ToVkSampleCountFlags(vkPDLimits.framebufferColorSampleCounts);
	m_Caps.framebufferDepthSampleCounts = ToVkSampleCountFlags(vkPDLimits.framebufferDepthSampleCounts);
	m_Caps.framebufferStencilSampleCounts = ToVkSampleCountFlags(vkPDLimits.framebufferStencilSampleCounts);
	m_Caps.framebufferNoAttachmentsSampleCounts = ToVkSampleCountFlags(vkPDLimits.framebufferNoAttachmentsSampleCounts);
	m_Caps.sampledImageColorSampleCounts = ToVkSampleCountFlags(vkPDLimits.sampledImageColorSampleCounts);
	m_Caps.sampledImageIntegerSampleCounts = ToVkSampleCountFlags(vkPDLimits.sampledImageIntegerSampleCounts);
	m_Caps.sampledImageDepthSampleCounts = ToVkSampleCountFlags(vkPDLimits.sampledImageDepthSampleCounts);
	m_Caps.sampledImageStencilSampleCounts = ToVkSampleCountFlags(vkPDLimits.sampledImageStencilSampleCounts);
	m_Caps.storageImageSampleCounts = ToVkSampleCountFlags(vkPDLimits.storageImageSampleCounts);
	m_Caps.maxColorAttachments = vkPDLimits.maxColorAttachments;
	m_Caps.maxSampleMaskWords = vkPDLimits.maxSampleMaskWords;
	m_Caps.timestampComputeAndGraphics = ToV3DBool(vkPDLimits.timestampComputeAndGraphics);
	m_Caps.timestampPeriod = vkPDLimits.timestampPeriod;
	m_Caps.maxClipDistances = vkPDLimits.maxClipDistances;
	m_Caps.maxCullDistances = vkPDLimits.maxCullDistances;
	m_Caps.maxCombinedClipAndCullDistances = vkPDLimits.maxCombinedClipAndCullDistances;
	m_Caps.pointSizeRange[0] = vkPDLimits.pointSizeRange[0];
	m_Caps.pointSizeRange[1] = vkPDLimits.pointSizeRange[1];
	m_Caps.pointSizeGranularity = vkPDLimits.pointSizeGranularity;
	m_Caps.discreteQueuePriorities = vkPDLimits.discreteQueuePriorities;
	m_Caps.standardSampleLocations = ToV3DBool(vkPDLimits.standardSampleLocations);
	m_Caps.optimalBufferCopyOffsetAlignment = vkPDLimits.optimalBufferCopyOffsetAlignment;
	m_Caps.nonCoherentAtomSize = vkPDLimits.nonCoherentAtomSize;

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

V3D_RESULT V3DDevice::CreateDescriptorSetLayout(uint32_t descriptorCount, const V3DDescriptorDesc* descriptors, uint32_t poolSize, uint32_t poolResizeStep, IV3DDescriptorSetLayout** ppDescriptorSetLayout, const wchar_t* pDebugName)
{
	if ((descriptorCount == 0) || (descriptors == nullptr))
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DDescriptorSetLayout* pDescriptorSetLayout = V3DDescriptorSetLayout::Create();
	if (pDescriptorSetLayout == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = pDescriptorSetLayout->Initialize(this, descriptorCount, descriptors, poolSize, poolResizeStep, pDebugName);
	if (result != V3D_OK)
	{
		return result;
	}

	(*ppDescriptorSetLayout) = pDescriptorSetLayout;

	return V3D_OK;
}

V3D_RESULT V3DDevice::CreateDescriptorSet(IV3DDescriptorSetLayout* pLayout, IV3DDescriptorSet** ppDescriptorSet, const wchar_t* pDebugName)
{
	if (pLayout == nullptr)
	{
		return V3D_ERROR_INVALID_ARGUMENT;
	}

	V3DDescriptorSet* pDescriptorSet = V3DDescriptorSet::Create();
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
	m_Statistics({})
{
	m_Source.device = VK_NULL_HANDLE;
}

V3DDevice::~V3DDevice()
{
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

	if (m_Source.device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_Source.device, nullptr);
		V3D_REMOVE_DEBUG_OBJECT(m_pInstance, m_Source.device);
	}

	V3D_RELEASE(m_pAdapter);
	V3D_RELEASE(m_pInstance);
}
