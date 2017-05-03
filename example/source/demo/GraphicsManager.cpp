#include "GraphicsManager.h"
#include "Mesh.h"

#define GRAPHICS_MANAGER_COMPILE_SHADER

GraphicsManager::GraphicsManager() :
	m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_Lost(true)
{
}

GraphicsManager::~GraphicsManager()
{
}

V3D_RESULT GraphicsManager::Initialize(IV3DDevice* pDevice, IV3DQueue* pQueue, IV3DSwapChain* pSwapChain)
{
	SAFE_ADD_REF(pDevice);
	m_pDevice = pDevice;

	SAFE_ADD_REF(pQueue);
	m_pQueue = pQueue;

	SAFE_ADD_REF(pSwapChain);
	m_pSwapChain = pSwapChain;

	V3D_RESULT result;

	// ----------------------------------------------------------------------------------------------------
	// シェーダーモジュールを作成
	// ----------------------------------------------------------------------------------------------------

	std::wstring dstFilePath;
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	std::wstring srcFilePath;
#endif //GRAPHICS_MANAGER_COMPILE_SHADER

	// シンプル - vert
	CreateFilePath(L"shader\\simple.vert.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\simple.vert", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// シンプル - frag
	CreateFilePath(L"shader\\simple.frag.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\simple.frag", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_SIMPLE]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// メッシュ - vert
	CreateFilePath(L"shader\\mesh.vert.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\mesh.vert", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_MESH]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// メッシュ - frag
	CreateFilePath(L"shader\\mesh_d.frag.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\mesh_d.frag", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_MESH_D]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// ディレクショナルライティング
	CreateFilePath(L"shader\\directionalLighting_d.frag.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\directionalLighting_d.frag", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_DIRECTIONAL_LIGHTING_D]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// ポイントライティング
	CreateFilePath(L"shader\\pointLighting_d.frag.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\pointLighting_d.frag", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_POINT_LIGHTING_D]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// フィニッシュライティング
	CreateFilePath(L"shader\\finishLighting_d.frag.spv", dstFilePath);
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	CreateFilePath(L"shader\\source\\finishLighting_d.frag", srcFilePath);
	if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER
	if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_FINISH_LIGHTING_D]) != V3D_OK)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	// Deffered メッシュ
	{
		V3DDescriptorDesc descriptors[] =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, V3D_SHADER_STAGE_VERTEX }
		};

		result = pDevice->CreateDescriptorSetLayout(
			_countof(descriptors), descriptors,
			GraphicsManager::DRMeshDescPoolSize, GraphicsManager::DRMeshDescPoolSize,
			&m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY][GRAPHICS_DESCRIPTOR_SET_TYPE_MESH]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered マテリアル
	{
		V3DDescriptorDesc descriptors[] =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         V3D_SHADER_STAGE_FRAGMENT },
			{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
		};

		result = pDevice->CreateDescriptorSetLayout(
			_countof(descriptors), descriptors,
			GraphicsManager::DRMaterialDescPoolSize, GraphicsManager::DRMaterialDescPoolSize,
			&m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY][GRAPHICS_DESCRIPTOR_SET_TYPE_MATERIAL]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered ディレクショナルライティング
	{
		V3DDescriptorDesc descriptors[] =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         V3D_SHADER_STAGE_FRAGMENT },
			{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			_countof(descriptors), descriptors,
			GraphicsManager::DRLightingDescPoolSize, GraphicsManager::DRLightingDescPoolSize,
			&m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered ポイントライティング
	{
		V3DDescriptorDesc descriptors[] =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         V3D_SHADER_STAGE_FRAGMENT },
			{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			_countof(descriptors), descriptors,
			GraphicsManager::DRLightingDescPoolSize, GraphicsManager::DRLightingDescPoolSize,
			&m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered フィニッシュライティング
	{
		V3DDescriptorDesc descriptors[] =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			_countof(descriptors), descriptors,
			GraphicsManager::DRLightingDescPoolSize, GraphicsManager::DRLightingDescPoolSize,
			&m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	// シンプル
	{
		result = m_pDevice->CreatePipelineLayout(0, nullptr, 0, nullptr, &m_pPipelineLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_SIMPLE]);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered メッシュ
	{
		IV3DDescriptorSetLayout* pDescriptorSetLayouts[] =
		{
			m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY][GRAPHICS_DESCRIPTOR_SET_TYPE_MESH],
			m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY][GRAPHICS_DESCRIPTOR_SET_TYPE_MATERIAL],
		};

		result = pDevice->CreatePipelineLayout(
			0, nullptr,
			_countof(pDescriptorSetLayouts), pDescriptorSetLayouts,
			&m_pPipelineLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered ディレクショナルライティング
	{
		IV3DDescriptorSetLayout* pDescriptorSetLayouts[1] =
		{
			m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			_countof(pDescriptorSetLayouts), pDescriptorSetLayouts,
			&m_pPipelineLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered ポイントライティング
	{
		IV3DDescriptorSetLayout* pDescriptorSetLayouts[1] =
		{
			m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			_countof(pDescriptorSetLayouts), pDescriptorSetLayouts,
			&m_pPipelineLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// Deffered フィニッシュライティング
	{
		IV3DDescriptorSetLayout* pDescriptorSetLayouts[1] =
		{
			m_pDescriptorSetLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING][GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			_countof(pDescriptorSetLayouts), pDescriptorSetLayouts,
			&m_pPipelineLayouts[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// グラフィックスパイプラインの記述を作成
	// ----------------------------------------------------------------------------------------------------

	// シンプル
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_SIMPLE];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_SIMPLE];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32A32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 16;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(SimpleVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		pipeline.rasterization.discardEnable = false;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_NONE;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = false;
		pipeline.depthStencil.depthWriteEnable = false;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_ALWAYS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 1;
	}

	// Deffered メッシュ
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY];
		pipeline.Allocate(5, 1, 3);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_MESH];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_MESH_D];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 12;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;
		pipeline.vertexInput.pElements[2].location = 2;
		pipeline.vertexInput.pElements[2].offset = 20;
		pipeline.vertexInput.pElements[2].format = V3D_FORMAT_R32G32B32_SFLOAT;
		pipeline.vertexInput.pElements[3].location = 3;
		pipeline.vertexInput.pElements[3].offset = 32;
		pipeline.vertexInput.pElements[3].format = V3D_FORMAT_R32G32B32_SFLOAT;
		pipeline.vertexInput.pElements[4].location = 4;
		pipeline.vertexInput.pElements[4].offset = 44;
		pipeline.vertexInput.pElements[4].format = V3D_FORMAT_R32G32B32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(Mesh::Vertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline.rasterization.discardEnable = false;

		pipeline.tessellation.patchControlPoints = 3;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_BACK;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = true;
		pipeline.depthStencil.depthWriteEnable = true;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_LESS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);
		pipeline.colorBlend.pAttachments[1] = InitializeColorBlendAttachment(BLEND_MODE_COPY);
		pipeline.colorBlend.pAttachments[2] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 0;
	}

	// Deffered ディレクショナルライティング
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_DIRECTIONAL_LIGHTING_D];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32A32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 16;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(SimpleVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		pipeline.rasterization.discardEnable = false;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_NONE;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = false;
		pipeline.depthStencil.depthWriteEnable = false;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_ALWAYS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 1;
	}

	// Deffered ポイントライティング
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_POINT_LIGHTING_D];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32A32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 16;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(SimpleVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		pipeline.rasterization.discardEnable = false;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_NONE;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = false;
		pipeline.depthStencil.depthWriteEnable = false;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_ALWAYS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 2;
	}

	// Deffered フィニッシュライティング
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY][GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_FINISH_LIGHTING_D];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32A32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 16;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(SimpleVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		pipeline.rasterization.discardEnable = false;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_NONE;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = false;
		pipeline.depthStencil.depthWriteEnable = false;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_ALWAYS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 3;
	}

	// ----------------------------------------------------------------------------------------------------
	// アタッチメントの記述を作成
	// ----------------------------------------------------------------------------------------------------

	// Deffered
	{
		GraphicsManager::Attachment attachment{};

		attachment.imageDesc.type = V3D_IMAGE_TYPE_2D;
		attachment.imageDesc.depth = 1;
		attachment.imageDesc.levelCount = 1;
		attachment.imageDesc.samples = V3D_SAMPLE_COUNT_1;
		attachment.imageDesc.tiling = V3D_IMAGE_TILING_OPTIMAL;

		attachment.imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
		attachment.imageViewDesc.baseLevel = 0;
		attachment.imageViewDesc.levelCount = 1;
		attachment.imageViewDesc.baseLayer = 0;
		attachment.imageViewDesc.layerCount = 1;

		m_Attachments.resize(GraphicsManager::ATTACHMENT_TYPE_COUNT);

		// カラー
		attachment.imageDesc.format = V3D_FORMAT_B8G8R8A8_UNORM;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageAccessType = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			attachment.imageDesc.format = V3D_FORMAT_A8B8G8R8_UNORM;
			if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				attachment.imageDesc.format = V3D_FORMAT_R8G8B8A8_UNORM;
				if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
				{
					return V3D_ERROR_FAIL;
				}
			}
		}
		m_Attachments[GraphicsManager::ATTACHMENT_TYPE_COLOR] = attachment;

		// バッファー 0
		attachment.imageDesc.format = V3D_FORMAT_B8G8R8A8_UNORM;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageAccessType = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			attachment.imageDesc.format = V3D_FORMAT_A8B8G8R8_UNORM;
			if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				attachment.imageDesc.format = V3D_FORMAT_R8G8B8A8_UNORM;
				if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
				{
					return V3D_ERROR_FAIL;
				}
			}
		}
		m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_0] = attachment;

		// バッファー 1
		attachment.imageDesc.format = V3D_FORMAT_R32G32B32A32_SFLOAT;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageAccessType = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			attachment.imageDesc.format = V3D_FORMAT_R16G16B16A16_SFLOAT;
			if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				return V3D_ERROR_FAIL;
			}
		}
		m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_1] = attachment;

		// デプスステンシル
		attachment.imageDesc.format = V3D_FORMAT_D24_UNORM_S8_UINT;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		attachment.imageAccessType = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			attachment.imageDesc.format = V3D_FORMAT_D16_UNORM;
			if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				return V3D_ERROR_FAIL;
			}
		}
		m_Attachments[GraphicsManager::ATTACHMENT_TYPE_DEPTH_STENCIL] = attachment;

		// ライトカラー
		attachment.imageDesc.format = V3D_FORMAT_B8G8R8A8_UNORM;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_TRANSFER_DST | V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageAccessType = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			attachment.imageDesc.format = V3D_FORMAT_A8B8G8R8_UNORM;
			if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
			{
				attachment.imageDesc.format = V3D_FORMAT_R8G8B8A8_UNORM;
				if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
				{
					return V3D_ERROR_FAIL;
				}
			}
		}
		m_Attachments[GraphicsManager::ATTACHMENT_TYPE_LIGHT] = attachment;
	}

	// ----------------------------------------------------------------------------------------------------
	// コマンドバッファを作成
	// ----------------------------------------------------------------------------------------------------

	{
		V3DCommandPoolDesc commandPoolDesc{};
		commandPoolDesc.queueFamily = m_pQueue->GetFamily();
		commandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER;

		result = m_pDevice->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &m_pCommandBuffer);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// フェンスを作成
	// ----------------------------------------------------------------------------------------------------

	result = m_pDevice->CreateFence(&m_pFence);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void GraphicsManager::Finalize()
{
	for (auto i = 0; i < GRAPHICS_RENDERPASS_TYPE_COUNT; i++)
	{
		for (auto j = 0; j < GRAPHICS_PIPELINE_TYPE_COUNT; j++)
		{
			for (auto k = 0; k < GRAPHICS_DESCRIPTOR_SET_TYPE_COUNT; k++)
			{
				SAFE_RELEASE(m_pDescriptorSetLayouts[i][j][k]);
			}

			SAFE_RELEASE(m_pPipelineLayouts[i][j]);
		}

		for (uint32_t j = 0; j < GRAPHICS_PIPELINE_TYPE_COUNT; j++)
		{
			GraphicsManager::PipelineMap& pipelineMap = m_PipelineMap[i][j];

			if (pipelineMap.empty() == false)
			{
				GraphicsManager::PipelineMap::iterator it_begin = pipelineMap.begin();
				GraphicsManager::PipelineMap::iterator it_end = pipelineMap.end();
				GraphicsManager::PipelineMap::iterator it = pipelineMap.begin();

				for (it = it_begin; it != it_end; ++it)
				{
					if (it->second != nullptr)
					{
						SAFE_RELEASE(it->second->m_pPipeline);
						SAFE_DELETE(it->second);
					}
				}
			}
		}

		if (m_RenderPasses[i] != nullptr)
		{
			SAFE_RELEASE(m_RenderPasses[i]->m_pRenderPass);
			SAFE_DELETE(m_RenderPasses[i]);
		}
	}

	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		SAFE_RELEASE(m_Attachments[i].pImageView);
		SAFE_RELEASE(m_Attachments[i].pImage);
	}

	for (auto i = 0; i < SHADER_TYPE_COUNT; i++)
	{
		SAFE_RELEASE(m_pShaderModules[i]);
	}

	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pFence);
	SAFE_RELEASE(m_pCommandBuffer);
	SAFE_RELEASE(m_pQueue);
	SAFE_RELEASE(m_pDevice);
}

void GraphicsManager::Lost()
{
	for (uint32_t i = 0; i < GRAPHICS_RENDERPASS_TYPE_COUNT; i++)
	{
		for (uint32_t j = 0; j < GRAPHICS_PIPELINE_TYPE_COUNT; j++)
		{
			GraphicsManager::PipelineMap& pipelineMap = m_PipelineMap[i][j];

			if (pipelineMap.empty() == false)
			{
				GraphicsManager::PipelineMap::iterator it_begin = pipelineMap.begin();
				GraphicsManager::PipelineMap::iterator it_end = pipelineMap.end();
				GraphicsManager::PipelineMap::iterator it;

				for (it = it_begin; it != it_end; ++it)
				{
					if (it->second != nullptr)
					{
						SAFE_RELEASE(it->second->m_pPipeline);
					}
				}
			}
		}

		if (m_RenderPasses[i] != nullptr)
		{
			SAFE_RELEASE(m_RenderPasses[i]->m_pRenderPass);
		}
	}

	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		SAFE_RELEASE(m_Attachments[i].pImageView);
		SAFE_RELEASE(m_Attachments[i].pImage);
	}

	m_Attachments.resize(GraphicsManager::ATTACHMENT_TYPE_COUNT);

	m_Lost = true;
}

V3D_RESULT GraphicsManager::Restore()
{
	if (m_Lost == false)
	{
		return V3D_OK;
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージを復帰
	// ----------------------------------------------------------------------------------------------------

	ASSERT(m_Attachments.size() == GraphicsManager::ATTACHMENT_TYPE_COUNT);

	const V3DSwapChainDesc& swapChainDesc = m_pSwapChain->GetDesc();

	std::vector<IV3DResource*> attachmenResources;
	attachmenResources.reserve(m_Attachments.size());

	// イメージを作成
	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		attachment.imageDesc.width = swapChainDesc.imageWidth;
		attachment.imageDesc.height = swapChainDesc.imageHeight;

		V3D_RESULT result = m_pDevice->CreateImage(attachment.imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &attachment.pImage);
		if (result != V3D_OK)
		{
			return result;
		}

		attachmenResources.push_back(attachment.pImage);
	}

	// イメージをまとめてバインド
	V3D_RESULT result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, TO_UI32(attachmenResources.size()), attachmenResources.data());
	if (result != V3D_OK)
	{
		return result;
	}

	// イメージビューを作成
	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		V3D_RESULT result = m_pDevice->CreateImageView(attachment.pImage, attachment.imageViewDesc, &attachment.pImageView);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// スワップチェインのイメージビューを作成
	for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
	{
		m_Attachments.push_back(GraphicsManager::Attachment{});
		GraphicsManager::Attachment& attachment = m_Attachments.back();

		m_pSwapChain->GetImage(i, &attachment.pImage);
		attachment.imageDesc = attachment.pImage->GetDesc();
		attachment.imageAccessType = V3D_ACCESS_MEMORY_READ;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;

		attachment.imageViewDesc.type = V3D_IMAGE_VIEW_TYPE_2D;
		attachment.imageViewDesc.baseLevel = 0;
		attachment.imageViewDesc.levelCount = 1;
		attachment.imageViewDesc.baseLayer = 0;
		attachment.imageViewDesc.layerCount = 1;

		V3D_RESULT result = m_pDevice->CreateImageView(attachment.pImage, attachment.imageViewDesc, &attachment.pImageView);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// イメージを初期化
	// ----------------------------------------------------------------------------------------------------

	IV3DCommandBuffer* pCommandbuffer = ResetCommandBuffer();
	if (pCommandbuffer == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		V3DBarrierImageDesc barrier{};
		barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

		if (attachment.imageDesc.usageFlags & V3D_IMAGE_USAGE_TRANSFER_DST)
		{
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			m_pCommandBuffer->BarrierImageView(attachment.pImageView, barrier);

			m_pCommandBuffer->ClearImageView(attachment.pImageView, V3D_IMAGE_LAYOUT_TRANSFER_DST, V3DClearValue{});

			barrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.dstAccessMask = attachment.imageAccessType;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			barrier.dstLayout = attachment.imageLayout;
			m_pCommandBuffer->BarrierImageView(attachment.pImageView, barrier);
		}
		else
		{
			V3DBarrierImageDesc barrier{};
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = attachment.imageAccessType;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			barrier.dstLayout = attachment.imageLayout;
			m_pCommandBuffer->BarrierImageView(attachment.pImageView, barrier);
		}
	}

	if (FlushCommandBuffer() == false)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// レンダーパス、グラフィックスパイプラインを復帰
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < GRAPHICS_RENDERPASS_TYPE_COUNT; i++)
	{
		// レンダーパス
		if (m_RenderPasses[i] != nullptr)
		{
			V3D_RESULT result = CreateNativeRenderPass(i, &m_RenderPasses[i]->m_pRenderPass);
			if (result != V3D_OK)
			{
				return result;
			}
		}

		// グラフィックスパイプライン
		for (uint32_t j = 0; j < GRAPHICS_PIPELINE_TYPE_COUNT; j++)
		{
			GraphicsManager::PipelineMap& pipelineMap = m_PipelineMap[i][j];
			GraphicsManager::PipelineMap::iterator it_begin = pipelineMap.begin();
			GraphicsManager::PipelineMap::iterator it_end = pipelineMap.end();
			GraphicsManager::PipelineMap::iterator it;

			for (it = it_begin; it != it_end; ++it)
			{
				V3D_RESULT result = CreateNativePipeline(i, j, it->first, &it->second->m_pPipeline);
				if (result != V3D_OK)
				{
					return result;
				}
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	m_Lost = false;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void GraphicsManager::GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE renderpassType, GRAPHICS_PIPELINE_TYPE pipelineType, IV3DPipelineLayout** ppPipelineLayout)
{
	SAFE_ADD_REF(m_pPipelineLayouts[renderpassType][pipelineType]);
	*ppPipelineLayout = m_pPipelineLayouts[renderpassType][pipelineType];
}

V3D_RESULT GraphicsManager::GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE renderpassType, GraphicsRenderPassHandle* pRenderPassHandle)
{
	GraphicsRenderPassHandle renderPassHandle;

	if (m_RenderPasses[renderpassType] != nullptr)
	{
		renderPassHandle = m_RenderPasses[renderpassType];
	}
	else
	{
		renderPassHandle = new GraphicsRenderPassHandleT();
		if (renderPassHandle == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		renderPassHandle->m_pRenderPass = nullptr;
		m_RenderPasses[renderpassType] = renderPassHandle;
	}

	if ((m_Lost == false) && (renderPassHandle->m_pRenderPass == nullptr))
	{
		V3D_RESULT result = CreateNativeRenderPass(renderpassType, &renderPassHandle->m_pRenderPass);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	*pRenderPassHandle = renderPassHandle;

	return V3D_OK;
}

V3D_RESULT GraphicsManager::GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE renderpassType, GRAPHICS_PIPELINE_TYPE pipelineType, const GraphicsPipelienDesc& desc, GraphicsPipelineHandle* pPipelineHandle)
{
	GraphicsManager::PipelineMap& pipelineMap = m_PipelineMap[renderpassType][pipelineType];
	GraphicsPipelineHandle pipelineHandle;

	GraphicsManager::PipelineMap::iterator it = pipelineMap.find(desc);
	if (it != pipelineMap.end())
	{
		pipelineHandle = it->second;
	}
	else
	{
		pipelineHandle = new GraphicsPipelineHandleT();
		if (pipelineHandle == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		pipelineHandle->m_pPipeline = nullptr;
		pipelineMap[desc] = pipelineHandle;
	}

	if ((m_Lost == false) && (pipelineHandle->m_pPipeline == nullptr))
	{
		// ハンドルを取得してレンダーパス作成を促す
		GraphicsRenderPassHandle renderPassHandle;
		V3D_RESULT result = GetRenderPassHandle(renderpassType, &renderPassHandle);
		if (result != V3D_OK)
		{
			return result;
		}

		result = CreateNativePipeline(renderpassType, pipelineType, desc, &pipelineHandle->m_pPipeline);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	*pPipelineHandle = pipelineHandle;

	return V3D_OK;
}

V3D_RESULT GraphicsManager::CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE renderpassType, uint32_t index, IV3DFrameBuffer** ppFrameBuffer)
{
	if (renderpassType == GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY)
	{
		IV3DImageView* pAttachments[] =
		{
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_COLOR].pImageView,
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_0].pImageView,
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_1].pImageView,
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_DEPTH_STENCIL].pImageView,
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_LIGHT].pImageView,
			m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BACK_BUFFER + index].pImageView,
		};

		V3D_RESULT result = m_pDevice->CreateFrameBuffer(m_RenderPasses[renderpassType]->m_pRenderPass, _countof(pAttachments), pAttachments, ppFrameBuffer);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	return V3D_OK;
}

V3D_RESULT GraphicsManager::CreateDescriptorSet(GRAPHICS_RENDERPASS_TYPE renderpassType, GRAPHICS_PIPELINE_TYPE pipelineType, GRAPHICS_DESCRIPTOR_SET_TYPE descriptorSetType, IV3DDescriptorSet** ppDescriptorSet)
{
	return m_pDevice->CreateDescriptorSet(m_pDescriptorSetLayouts[renderpassType][pipelineType][descriptorSetType], ppDescriptorSet);
}

V3D_RESULT GraphicsManager::CreateNativeRenderPass(uint32_t renderpassType, IV3DRenderPass** ppRenderPass)
{
	if (renderpassType == GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY)
	{
		Array1<V3DAttachmentDesc, 6> attachments;

		attachments[0].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_COLOR].imageDesc.format;
		attachments[0].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_COLOR].imageDesc.samples;
		attachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[0].clearValue.color.float32[0] = 0.0f;
		attachments[0].clearValue.color.float32[1] = 0.0f;
		attachments[0].clearValue.color.float32[2] = 0.2f;
		attachments[0].clearValue.color.float32[3] = 0.0f;

		attachments[1].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_0].imageDesc.format;
		attachments[1].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_0].imageDesc.samples;
		attachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[1].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[1].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[1].clearValue.color.float32[0] = 0.0f;
		attachments[1].clearValue.color.float32[1] = 0.0f;
		attachments[1].clearValue.color.float32[2] = 0.0f;
		attachments[1].clearValue.color.float32[3] = 0.0f;

		attachments[2].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_1].imageDesc.format;
		attachments[2].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BUFFER_1].imageDesc.samples;
		attachments[2].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[2].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[2].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[2].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[2].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[2].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[2].clearValue.color.float32[0] = 0.0f;
		attachments[2].clearValue.color.float32[1] = 0.0f;
		attachments[2].clearValue.color.float32[2] = 0.0f;
		attachments[2].clearValue.color.float32[3] = 1.0f;

		attachments[3].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_DEPTH_STENCIL].imageDesc.format;
		attachments[3].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_DEPTH_STENCIL].imageDesc.samples;
		attachments[3].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[3].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[3].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[3].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[3].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[3].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachments[3].clearValue.depthStencil.depth = 1.0f;
		attachments[3].clearValue.depthStencil.stencil = 0;

		attachments[4].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_LIGHT].imageDesc.format;
		attachments[4].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_LIGHT].imageDesc.samples;
		attachments[4].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[4].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[4].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[4].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[4].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[4].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[4].clearValue.color.float32[0] = 0.0f;
		attachments[4].clearValue.color.float32[1] = 0.0f;
		attachments[4].clearValue.color.float32[2] = 0.0f;
		attachments[4].clearValue.color.float32[3] = 0.0f;

		attachments[5].format = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BACK_BUFFER].imageDesc.format;
		attachments[5].samples = m_Attachments[GraphicsManager::ATTACHMENT_TYPE_BACK_BUFFER].imageDesc.samples;
		attachments[5].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[5].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		attachments[5].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		attachments[5].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		attachments[5].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachments[5].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		attachments[5].clearValue.color.float32[0] = 0.0f;
		attachments[5].clearValue.color.float32[1] = 0.0f;
		attachments[5].clearValue.color.float32[2] = 0.0f;
		attachments[5].clearValue.color.float32[3] = 0.0f;

		// 0 : color
		// 1 : buffer 0
		// 2 : buffer 1
		// 3 : depth
		// 4 : light
		// 5 : back buffer

		Array1<V3DSubpassDesc, 5> subpasses;

		// サブパス 0 - ジオメトリバッファー
		Array1<V3DAttachmentReference, 3> colorReferences0;
		colorReferences0[0].attachment = 0;
		colorReferences0[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		colorReferences0[1].attachment = 1;
		colorReferences0[1].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		colorReferences0[2].attachment = 2;
		colorReferences0[2].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		V3DAttachmentReference depthReference0;
		depthReference0.attachment = 3;
		depthReference0.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		subpasses[0].inputAttachmentCount = 0;
		subpasses[0].pInputAttachments = nullptr;
		subpasses[0].colorAttachmentCount = TO_UI32(colorReferences0.size());
		subpasses[0].pColorAttachments = colorReferences0.data();
		subpasses[0].pResolveAttachments = nullptr;
		subpasses[0].pDepthStencilAttachment = &depthReference0;
		subpasses[0].preserveAttachmentCount = 0;
		subpasses[0].pPreserveAttachments = nullptr;

		// サブパス 1 - ディレクショナルライティング
		Array1<V3DAttachmentReference, 1> inputReferences1;
		inputReferences1[0].attachment = 1;
		inputReferences1[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		Array1<V3DAttachmentReference, 1> colorReferences1;
		colorReferences1[0].attachment = 4;
		colorReferences1[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		subpasses[1].inputAttachmentCount = TO_UI32(inputReferences1.size());
		subpasses[1].pInputAttachments = inputReferences1.data();
		subpasses[1].colorAttachmentCount = TO_UI32(colorReferences1.size());
		subpasses[1].pColorAttachments = colorReferences1.data();
		subpasses[1].pResolveAttachments = nullptr;
		subpasses[1].pDepthStencilAttachment = nullptr;
		subpasses[1].preserveAttachmentCount = 0;
		subpasses[1].pPreserveAttachments = nullptr;

		// サブパス 2 - ポイントライティング
		Array1<V3DAttachmentReference, 2> inputReferences2;
		inputReferences2[0].attachment = 1;
		inputReferences2[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		inputReferences2[1].attachment = 2;
		inputReferences2[1].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		Array1<V3DAttachmentReference, 1> colorReferences2;
		colorReferences2[0].attachment = 4;
		colorReferences2[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		subpasses[2].inputAttachmentCount = TO_UI32(inputReferences2.size());
		subpasses[2].pInputAttachments = inputReferences2.data();
		subpasses[2].colorAttachmentCount = TO_UI32(colorReferences2.size());
		subpasses[2].pColorAttachments = colorReferences2.data();
		subpasses[2].pResolveAttachments = nullptr;
		subpasses[2].pDepthStencilAttachment = nullptr;
		subpasses[2].preserveAttachmentCount = 0;
		subpasses[2].pPreserveAttachments = nullptr;

		// サブパス 3 - フィニッシュライティング
		Array1<V3DAttachmentReference, 2> inputReferences3;
		inputReferences3[0].attachment = 0;
		inputReferences3[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		inputReferences3[1].attachment = 4;
		inputReferences3[1].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		Array1<V3DAttachmentReference, 1> colorReferences3;
		colorReferences3[0].attachment = 5;
		colorReferences3[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		subpasses[3].inputAttachmentCount = TO_UI32(inputReferences3.size());
		subpasses[3].pInputAttachments = inputReferences3.data();
		subpasses[3].colorAttachmentCount = TO_UI32(colorReferences3.size());
		subpasses[3].pColorAttachments = colorReferences3.data();
		subpasses[3].pResolveAttachments = nullptr;
		subpasses[3].pDepthStencilAttachment = nullptr;
		subpasses[3].preserveAttachmentCount = 0;
		subpasses[3].pPreserveAttachments = nullptr;

		// サブパス 4 - テキストの描画
		Array1<V3DAttachmentReference, 1> colorReferences4;
		colorReferences4[0].attachment = 5;
		colorReferences4[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		subpasses[4].inputAttachmentCount = 0;
		subpasses[4].pInputAttachments = nullptr;
		subpasses[4].colorAttachmentCount = TO_UI32(colorReferences4.size());
		subpasses[4].pColorAttachments = colorReferences4.data();
		subpasses[4].pResolveAttachments = nullptr;
		subpasses[4].pDepthStencilAttachment = nullptr;
		subpasses[4].preserveAttachmentCount = 0;
		subpasses[4].pPreserveAttachments = nullptr;

		// サブパスの依存性
		Array1<V3DSubpassDependencyDesc, 5> subpassDependencies;

		// ジオメトリバッファ → ディレクショナルライティング
		subpassDependencies[0].srcSubpass = 0;
		subpassDependencies[0].dstSubpass = 1;
		subpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		subpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		subpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// ディレクショナルライティング → ポイントライティング
		subpassDependencies[1].srcSubpass = 1;
		subpassDependencies[1].dstSubpass = 2;
		subpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		subpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;
		subpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// ポイントライティング → フィニッシュライティング
		subpassDependencies[2].srcSubpass = 2;
		subpassDependencies[2].dstSubpass = 3;
		subpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		subpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		subpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// フィニッシュライティング → テキストの描画
		subpassDependencies[3].srcSubpass = 3;
		subpassDependencies[3].dstSubpass = 4;
		subpassDependencies[3].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[3].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[3].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[3].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[3].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// テキストの描画 → レンダーパス終了
		subpassDependencies[4].srcSubpass = 4;
		subpassDependencies[4].dstSubpass = V3D_SUBPASS_EXTERNAL;
		subpassDependencies[4].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		subpassDependencies[4].dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;
		subpassDependencies[4].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		subpassDependencies[4].dstAccessMask = V3D_ACCESS_MEMORY_READ;
		subpassDependencies[4].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		V3D_RESULT result = m_pDevice->CreateRenderPass(
			TO_UI32(attachments.size()), attachments.data(),
			TO_UI32(subpasses.size()), subpasses.data(),
			TO_UI32(subpassDependencies.size()), subpassDependencies.data(),
			ppRenderPass);

		if (result != V3D_OK)
		{
			return result;
		}
	}
	else
	{
		return V3D_ERROR_FAIL;
	}

	return V3D_OK;
}

V3D_RESULT GraphicsManager::CreateNativePipeline(uint32_t renderpassType, uint32_t pipelineType, const GraphicsPipelienDesc& desc, IV3DPipeline** ppPipeline)
{
	const V3DSwapChainDesc& swapChainDesc = m_pSwapChain->GetDesc();
	GraphicsManager::PipelineDesc& pipelineDesc = m_PipelineDescs[renderpassType][pipelineType];

	pipelineDesc.rasterization.polygonMode = desc.polygonMode;
	pipelineDesc.rasterization.cullMode = desc.cullMode;
	pipelineDesc.depthStencil.depthTestEnable = desc.depthTestEnable;
	pipelineDesc.depthStencil.depthWriteEnable = desc.depthWriteEnable;
	pipelineDesc.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(desc.blendMode);
	pipelineDesc.pRenderPass = m_RenderPasses[renderpassType]->m_pRenderPass;

	V3D_RESULT result = m_pDevice->CreateGraphicsPipeline(m_pPipelineLayouts[renderpassType][pipelineType], pipelineDesc, ppPipeline);
	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

IV3DDevice* GraphicsManager::GetDevicePtr()
{
	return m_pDevice;
}

IV3DQueue* GraphicsManager::GetQueuePtr()
{
	return m_pQueue;
}

IV3DCommandBuffer* GraphicsManager::GetCommandBufferPtr()
{
	return m_pCommandBuffer;
}

IV3DFence* GraphicsManager::GetFencePtr()
{
	return m_pFence;
}

IV3DCommandBuffer* GraphicsManager::ResetCommandBuffer()
{
	V3D_RESULT result = m_pCommandBuffer->Begin();
	if (result != V3D_OK)
	{
		return nullptr;
	}

	return m_pCommandBuffer;
}

bool GraphicsManager::FlushCommandBuffer()
{
	V3D_RESULT result = m_pCommandBuffer->End();
	if (result == V3D_OK)
	{
		result = m_pFence->Reset();
		if (result == V3D_OK)
		{
			result = m_pQueue->Submit(1, &m_pCommandBuffer, m_pFence);
			if (result == V3D_OK)
			{
				result = m_pFence->Wait();
			}
		}
	}

	if (result != V3D_OK)
	{
		return false;
	}

	return true;
}
