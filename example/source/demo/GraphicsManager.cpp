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

	const wchar_t* shaderFilePathes[SHADER_TYPE_COUNT] =
	{
		L"simple.vert",
		L"simple.frag",
		L"downSampling2x2.frag",
		nullptr,

		nullptr,
		nullptr,
		L"gaussianBlur.frag",
		nullptr,

		L"brightPass.frag",
		L"composite2.frag",

		L"shadowMesh.vert",
		L"shadowMesh.frag",
		L"mesh.vert",
		L"mesh_d.frag",
		L"skyDome.vert",
		L"skyDome.frag",
		L"directionalLighting_d.frag",
		L"pointLighting_d.frag",
		L"finishLighting_d.frag",
		nullptr,

		L"ssao.frag",
		L"correction.frag",
		L"toneMapping.frag",
		L"fxaa.frag",
	};

	std::wstring dstFilePath;
#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
	std::wstring srcFilePath;
#endif //GRAPHICS_MANAGER_COMPILE_SHADER

	for (auto i = 0; i < SHADER_TYPE_COUNT; i++)
	{
		if (shaderFilePathes[i] == nullptr)
		{
			continue;
		}

		std::wstringstream stringStream;

		stringStream << L"shader\\" << shaderFilePathes[i] << L".spv";
		CreateFilePath(stringStream.str().c_str(), dstFilePath);

#ifdef GRAPHICS_MANAGER_COMPILE_SHADER
		stringStream.str(L"");
		stringStream << L"shader\\source\\" << shaderFilePathes[i];
		CreateFilePath(stringStream.str().c_str(), srcFilePath);

		if (CompileShaderFromFile(COMPILE_SHADER_SPIR_V_BINARY_VULKAN, srcFilePath.c_str(), dstFilePath.c_str()) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
#endif //GRAPHICS_MANAGER_COMPILE_SHADER

		if (CreateShaderModuleFromFile(m_pDevice, dstFilePath.c_str(), &m_pShaderModules[i]) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// デスクリプタセットレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	// シンプルサンプリング
	{
		Array1<V3DDescriptorDesc, 1> descriptors =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			4, 4,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ジオメトリ - メッシュ
	{
		Array1<V3DDescriptorDesc, 1> descriptors =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, V3D_SHADER_STAGE_VERTEX }
		};

		result = pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			GraphicsManager::DRMeshDescPoolSize, GraphicsManager::DRMeshDescPoolSize,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_MESH]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ジオメトリ - マテリアル
	{
		Array1<V3DDescriptorDesc, 5> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 3, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 4, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			GraphicsManager::DRMaterialDescPoolSize, GraphicsManager::DRMaterialDescPoolSize,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_MATERIAL]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ジオメトリ - スカイドーム
	{
		Array1<V3DDescriptorDesc, 1> descriptors =
		{
			{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER, V3D_SHADER_STAGE_FRAGMENT }
		};

		result = pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			1, 1,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SKYDOME]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ディレクショナルライティング
	{
		Array1<V3DDescriptorDesc, 4> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 3, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			GraphicsManager::DRLightingDescPoolSize, GraphicsManager::DRLightingDescPoolSize,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ポイントライティング
	{
		Array1<V3DDescriptorDesc, 3> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			GraphicsManager::DRLightingDescPoolSize, GraphicsManager::DRLightingDescPoolSize,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_POINT_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// フィニッシュライティング
	{
		Array1<V3DDescriptorDesc, 3> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			1, 1,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_FINISH_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ２枚を合成して出力
	{
		Array1<V3DDescriptorDesc, 2> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			1, 1,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_COMPOSITE_2]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// SSAO
	{
		Array1<V3DDescriptorDesc, 3> descriptors =
		{
			{
				{ 0, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 1, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
				{ 2, V3D_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, V3D_SHADER_STAGE_FRAGMENT },
			}
		};

		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			1, 1,
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SSAO]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// パイプラインレイアウトを作成
	// ----------------------------------------------------------------------------------------------------

	// ジオメトリ : メッシュ & マテリアル
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_VERTEX;
		constants[0].offset = 0;
		constants[0].size = sizeof(SceneConstant);

		Array1<IV3DDescriptorSetLayout*, 2> descriptorSetLayouts =
		{
			{
				m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_MESH],
				m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_MATERIAL],
			}
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_GEOMETRY]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ジオメトリ : スカイドーム
	{
		Array1<V3DConstantDesc, 1> constants;

		constants[0].shaderStageFlags = V3D_SHADER_STAGE_VERTEX;
		constants[0].offset = 0;
		constants[0].size = sizeof(SkyDomeVertConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SKYDOME],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// シャドウ
	{
		IV3DPipelineLayout* pPipelineLayout = m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_GEOMETRY];

		SAFE_ADD_REF(pPipelineLayout);
		m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_SHADOW] = pPipelineLayout;
	}

	// ライティング : ディレクショナル
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ライティング : ポイント
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_POINT_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ライティング : フィニッシュ
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_FINISH_LIGHTING]
		};

		result = m_pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ブライトパス
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(BrightPassConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ブラー : ダウンサンプリング
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(SamplingConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ブラー : ガウシアン
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(GaussianBlurConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()) , constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// 合成 : １枚
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_COMPOSITE_1]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// 合成 : ２枚
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(Composite2Constant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_COMPOSITE_2],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_COMPOSITE_2]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// SSAO
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(SsaoConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SSAO],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_SSAO]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ポストエフェクト : コピー
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ポストエフェクト : 補正
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(CorrectionConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_CORRECTION]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ポストエフェクト : トーンマッピング
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ポストエフェクト : FXAA
	{
		Array1<V3DConstantDesc, 1> constants;
		constants[0].shaderStageFlags = V3D_SHADER_STAGE_FRAGMENT;
		constants[0].offset = 0;
		constants[0].size = sizeof(FxaaConstant);

		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			TO_UI32(constants.size()), constants.data(),
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// オーバーレイ : コピー
	{
		Array1<IV3DDescriptorSetLayout*, 1> descriptorSetLayouts =
		{
			m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING],
		};

		result = pDevice->CreatePipelineLayout(
			0, nullptr,
			TO_UI32(descriptorSetLayouts.size()), descriptorSetLayouts.data(),
			&m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_FINISH_INIT]);

		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// アタッチメントの記述
	// ----------------------------------------------------------------------------------------------------

	// Deffered
	{
		V3DFlags formatFeatureFlags;

		V3D_FORMAT unorm8BitFormat;
		V3D_FORMAT sfloatFormat;
		V3D_FORMAT colorFormat;
		V3D_FORMAT depthStencilFormat;

		formatFeatureFlags = V3D_IMAGE_FORMAT_FEATURE_SAMPLED | V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT | V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND;
		unorm8BitFormat = V3D_FORMAT_B8G8R8A8_UNORM;
		if (m_pDevice->CheckImageFormatFeature(unorm8BitFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
		{
			unorm8BitFormat = V3D_FORMAT_A8B8G8R8_UNORM;
			if (m_pDevice->CheckImageFormatFeature(unorm8BitFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
			{
				unorm8BitFormat = V3D_FORMAT_R8G8B8A8_UNORM;
				if (m_pDevice->CheckImageFormatFeature(unorm8BitFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
				{
					return V3D_ERROR_FAIL;
				}
			}
		}

		formatFeatureFlags = V3D_IMAGE_FORMAT_FEATURE_SAMPLED | V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT | V3D_IMAGE_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND;
		sfloatFormat = V3D_FORMAT_R32G32B32A32_SFLOAT;
		if (m_pDevice->CheckImageFormatFeature(sfloatFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
		{
			sfloatFormat = V3D_FORMAT_R16G16B16A16_SFLOAT;
			if (m_pDevice->CheckImageFormatFeature(sfloatFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
			{
				return V3D_ERROR_FAIL;
			}
		}

		colorFormat = sfloatFormat;// unorm8BitFormat;

		formatFeatureFlags = V3D_IMAGE_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT;
		depthStencilFormat = V3D_FORMAT_D24_UNORM_S8_UINT;
		if (m_pDevice->CheckImageFormatFeature(depthStencilFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
		{
			depthStencilFormat = V3D_FORMAT_D16_UNORM;
			if (m_pDevice->CheckImageFormatFeature(depthStencilFormat, V3D_IMAGE_TILING_OPTIMAL, formatFeatureFlags) != V3D_OK)
			{
				return V3D_ERROR_FAIL;
			}
		}

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

		m_Attachments.resize(GraphicsManager::IMAGE_TYPE_COUNT);

		// ジオメトリ : カラー
		attachment.imageDesc.format = colorFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0] = attachment;
		m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1] = attachment;

		// ジオメトリ : バッファー ( rgb=法線 a=ピクセルが書き込まれたかどうか 0 or 1 )
		attachment.imageDesc.format = unorm8BitFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_0] = attachment;

		// ジオメトリ : バッファー ( rgb=マテリアル a=指数深度 )
		attachment.imageDesc.format = sfloatFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_1] = attachment;

		// ジオメトリ : デプスステンシル
		attachment.imageDesc.format = depthStencilFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS | V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS;
		attachment.imageAccessMask = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL] = attachment;

		// SSAO
		attachment.imageDesc.format = unorm8BitFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_SSAO_COLOR] = attachment;

		// シャドウ : バッファー ( rgb=色 a=指数深度 )
		attachment.imageDesc.format = sfloatFormat;
		attachment.imageDesc.width = GraphicsManager::ShadowMapSize;
		attachment.imageDesc.height = GraphicsManager::ShadowMapSize;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_RESERVED;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_SHADOW_BUFFER] = attachment;

		// シャドウ : デプスステンシル
		attachment.imageDesc.format = depthStencilFormat;
		attachment.imageDesc.width = GraphicsManager::ShadowMapSize;
		attachment.imageDesc.height = GraphicsManager::ShadowMapSize;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS | V3D_PIPELINE_STAGE_LATE_FRAGMENT_TESTS;
		attachment.imageAccessMask = V3D_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_RESERVED;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_SHADOW_DEPTH_STENCIL] = attachment;

		// ライティング
		attachment.imageDesc.format = colorFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_LIGHT_COLOR] = attachment;

		// LDR : カラー
		attachment.imageDesc.format = unorm8BitFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_LDR_COLOR_0] = attachment;
		m_Attachments[IMAGE_TYPE_LDR_COLOR_1] = attachment;

		// ブライトパス
		attachment.imageDesc.format = colorFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_DEFAULT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}
		m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR] = attachment;

		// ブラー
		attachment.imageDesc.format = colorFormat;
		attachment.imageDesc.layerCount = 1;
		attachment.imageDesc.usageFlags = V3D_IMAGE_USAGE_SAMPLED | V3D_IMAGE_USAGE_COLOR_ATTACHMENT | V3D_IMAGE_USAGE_INPUT_ATTACHMENT;
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		attachment.imageLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		if (m_pDevice->GetImageFormatDesc(attachment.imageDesc.format, attachment.imageDesc.type, attachment.imageDesc.tiling, attachment.imageDesc.usageFlags, nullptr) != V3D_OK)
		{
			return V3D_ERROR_FAIL;
		}

		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_HALF;
		m_Attachments[IMAGE_TYPE_BLUR_2_0] = attachment;
		m_Attachments[IMAGE_TYPE_BLUR_2_1] = attachment;

		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_QUARTER;
		m_Attachments[IMAGE_TYPE_BLUR_4_0] = attachment;
		m_Attachments[IMAGE_TYPE_BLUR_4_1] = attachment;
	}

	// ----------------------------------------------------------------------------------------------------
	// レンダーパスの記述
	// ----------------------------------------------------------------------------------------------------

	// ジオメトリ
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_GEOMETRY];

		desc.Allocate(4, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;
		pAttachments[0].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 0.2f;
		pAttachments[0].clearValue.color.float32[1] = 0.2f;
		pAttachments[0].clearValue.color.float32[2] = 0.2f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_0].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_0].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[1].clearValue.color.float32[0] = 0.0f;
		pAttachments[1].clearValue.color.float32[1] = 0.0f;
		pAttachments[1].clearValue.color.float32[2] = 0.0f;
		pAttachments[1].clearValue.color.float32[3] = 0.0f;

		pAttachments[2].format = m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_1].imageDesc.format;
		pAttachments[2].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_1].imageDesc.samples;
		pAttachments[2].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[2].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[2].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[2].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[2].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[2].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[2].clearValue.color.float32[0] = 0.0f;
		pAttachments[2].clearValue.color.float32[1] = 0.0f;
		pAttachments[2].clearValue.color.float32[2] = 0.0f;
		pAttachments[2].clearValue.color.float32[3] = 1.0f;

		pAttachments[3].format = m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].imageDesc.format;
		pAttachments[3].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].imageDesc.samples;
		pAttachments[3].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[3].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[3].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[3].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[3].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[3].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[3].clearValue.depthStencil.depth = 1.0f;
		pAttachments[3].clearValue.depthStencil.stencil = 0;

		// サブパス 0 - ジオメトリ
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 3, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->colorAttachments[1].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0;
		pSubpass->colorAttachments[1].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->colorAttachments[2].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1;
		pSubpass->colorAttachments[2].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// 遠景
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW];

		desc.Allocate(2, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;
		pAttachments[0].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 0.2f;
		pAttachments[0].clearValue.color.float32[1] = 0.2f;
		pAttachments[0].clearValue.color.float32[2] = 0.2f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].clearValue.depthStencil.depth = 1.0f;
		pAttachments[1].clearValue.depthStencil.stencil = 0;

		// サブパス 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_READ | V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// シャドウ
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_SHADOW];

		desc.Allocate(2, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_SHADOW_BUFFER].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_SHADOW_BUFFER].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 1.0f;
		pAttachments[0].clearValue.color.float32[1] = 1.0f;
		pAttachments[0].clearValue.color.float32[2] = 1.0f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_SHADOW_DEPTH_STENCIL].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_SHADOW_DEPTH_STENCIL].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].clearValue.depthStencil.depth = 1.0f;
		pAttachments[1].clearValue.depthStencil.stencil = 0;

		// サブパス 0 - プロジェクション
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_SHADOW_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_SHADOW_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// ライティング
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_LIGHTING];

		desc.Allocate(2, 3, 3);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_LIGHT_COLOR].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_LIGHT_COLOR].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].clearValue.color.float32[0] = 1.0f;
		pAttachments[0].clearValue.color.float32[1] = 1.0f;
		pAttachments[0].clearValue.color.float32[2] = 1.0f;
		pAttachments[0].clearValue.color.float32[3] = 0.0f;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[1].clearValue.color.float32[0] = 0.0f;
		pAttachments[1].clearValue.color.float32[1] = 0.0f;
		pAttachments[1].clearValue.color.float32[2] = 0.0f;
		pAttachments[1].clearValue.color.float32[3] = 0.0f;

		// サブパス 0 - ディレクショナルライティング
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 1 - ポイントライティング
		pSubpass = desc.AllocateSubpass(1, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 2 - フィニッシュライティング
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COMPOSITE_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		// ディレクショナルライティング → ポイントライティング
		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = 1;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// ポイントライティング → フィニッシュライティング
		pSubpassDependencies[1].srcSubpass = 1;
		pSubpassDependencies[1].dstSubpass = 2;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// フィニッシュライティング → レンダーパス終了
		pSubpassDependencies[2].srcSubpass = 1;
		pSubpassDependencies[2].dstSubpass = 2;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// ブライトパス
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS];

		desc.Allocate(1, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 0.0f;
		pAttachments[0].clearValue.color.float32[1] = 0.0f;
		pAttachments[0].clearValue.color.float32[2] = 0.0f;
		pAttachments[0].clearValue.color.float32[3] = 0.0f;

		// サブパス 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BRIGHT_PASS_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// ブラー
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_BLUR];

		desc.Allocate(2, 3, 3);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_BLUR_2_0].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_BLUR_2_0].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 0.0f;
		pAttachments[0].clearValue.color.float32[1] = 0.0f;
		pAttachments[0].clearValue.color.float32[2] = 0.0f;
		pAttachments[0].clearValue.color.float32[3] = 0.0f;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_BLUR_2_0].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_BLUR_2_0].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[1].clearValue.color.float32[0] = 0.0f;
		pAttachments[1].clearValue.color.float32[1] = 0.0f;
		pAttachments[1].clearValue.color.float32[2] = 0.0f;
		pAttachments[1].clearValue.color.float32[3] = 0.0f;

		// サブパス 0 : ダウンサンプリング
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 1 : 横ブラー
		pSubpass = desc.AllocateSubpass(1, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_1;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 2 : 縦ブラー
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_1;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = 1;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[1].srcSubpass = 1;
		pSubpassDependencies[1].dstSubpass = 2;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[2].srcSubpass = 2;
		pSubpassDependencies[2].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// 合成
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_COMPOSITE];

		desc.Allocate(1, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		// サブパス 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_COMPOSITE_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_READ | V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// SSAO
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_SSAO];

		desc.Allocate(1, 1, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_SSAO_COLOR].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_SSAO_COLOR].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 1.0f;
		pAttachments[0].clearValue.color.float32[1] = 1.0f;
		pAttachments[0].clearValue.color.float32[2] = 1.0f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		// サブパス 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_SSAO_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// イメージエフェクト
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT];

		desc.Allocate(4, 3, 3);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		pAttachments[1].format = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].imageDesc.format;
		pAttachments[1].samples = m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].imageDesc.samples;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		pAttachments[2].format = m_Attachments[IMAGE_TYPE_LDR_COLOR_0].imageDesc.format;
		pAttachments[2].samples = m_Attachments[IMAGE_TYPE_LDR_COLOR_0].imageDesc.samples;
		pAttachments[2].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[2].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[2].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[2].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[2].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[2].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		pAttachments[3].format = m_Attachments[IMAGE_TYPE_LDR_COLOR_1].imageDesc.format;
		pAttachments[3].samples = m_Attachments[IMAGE_TYPE_LDR_COLOR_1].imageDesc.samples;
		pAttachments[3].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[3].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[3].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[3].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[3].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[3].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 0 : HDR
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_1;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 1 : HDR to LDR
		pSubpass = desc.AllocateSubpass(1, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 2 : LDR
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_1;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[1].srcSubpass = 0;
		pSubpassDependencies[1].dstSubpass = 1;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[2].srcSubpass = 1;
		pSubpassDependencies[2].dstSubpass = 2;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
/*
		pSubpassDependencies[3].srcSubpass = 0;
		pSubpassDependencies[3].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[3].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[3].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[3].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[3].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[3].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
*/	}

	// フィニッシュ
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_FINISH];

		desc.Allocate(2, 2, 3);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;

		pAttachments[0].format = m_Attachments[IMAGE_TYPE_LDR_COLOR_1].imageDesc.format;
		pAttachments[0].samples = m_Attachments[IMAGE_TYPE_LDR_COLOR_1].imageDesc.samples;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_LOAD;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].clearValue.color.float32[0] = 0.0f;
		pAttachments[0].clearValue.color.float32[1] = 0.0f;
		pAttachments[0].clearValue.color.float32[2] = 0.0f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;
		
		pAttachments[1].format = V3D_FORMAT_UNDEFINED; // バックバッファー
		pAttachments[1].samples = V3D_SAMPLE_COUNT_1;
		pAttachments[1].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[1].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[1].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[1].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		pAttachments[1].clearValue.color.float32[0] = 0.0f;
		pAttachments[1].clearValue.color.float32[1] = 0.0f;
		pAttachments[1].clearValue.color.float32[2] = 0.0f;
		pAttachments[1].clearValue.color.float32[3] = 1.0f;

		// サブパス 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_FINISH_SOURCE_COLOR;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパス 1
		pSubpass = desc.AllocateSubpass(1, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// サブパスの依存性
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[1].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[1].dstSubpass = 0;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ | V3D_ACCESS_COLOR_ATTACHMENT_READ;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[2].srcSubpass = 0;
		pSubpassDependencies[2].dstSubpass = 1;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;
	}

	// ----------------------------------------------------------------------------------------------------
	// グラフィックスパイプラインの記述
	// ----------------------------------------------------------------------------------------------------

	// ジオメトリ - メッシュ & マテリアル
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_GEOMETRY][GRAPHICS_PIPELINE_TYPE_GEOMETRY];
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
		pipeline.vertexInput.pLayouts[0].stride = sizeof(MeshVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline.rasterization.discardEnable = false;

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

	// 遠景 - スカイドーム
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW][GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME];
		pipeline.Allocate(1, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SKYDOME];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_SKYDOME];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(Vector3);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline.rasterization.discardEnable = false;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_BACK;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = true;
		pipeline.depthStencil.depthWriteEnable = false;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_LESS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 0;
	}

	// シャドウ
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_SHADOW][GRAPHICS_PIPELINE_TYPE_SHADOW];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SHADOW_MESH];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_SHADOW_MESH];
		pipeline.fragmentShader.pEntryPointName = GraphicsManager::BasicEntryPointName;

		pipeline.vertexInput.pElements[0].location = 0;
		pipeline.vertexInput.pElements[0].offset = 0;
		pipeline.vertexInput.pElements[0].format = V3D_FORMAT_R32G32B32_SFLOAT;
		pipeline.vertexInput.pElements[1].location = 1;
		pipeline.vertexInput.pElements[1].offset = 12;
		pipeline.vertexInput.pElements[1].format = V3D_FORMAT_R32G32_SFLOAT;

		pipeline.vertexInput.pLayouts[0].binding = 0;
		pipeline.vertexInput.pLayouts[0].stride = sizeof(MeshVertex);
		pipeline.vertexInput.pLayouts[0].firstElement = 0;
		pipeline.vertexInput.pLayouts[0].elementCount = pipeline.vertexInput.elementCount;

		pipeline.primitiveTopology = V3D_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipeline.rasterization.discardEnable = false;

		pipeline.tessellation.patchControlPoints = 3;

		pipeline.rasterization.polygonMode = V3D_POLYGON_MODE_FILL;
		pipeline.rasterization.cullMode = V3D_CULL_MODE_FRONT;
		pipeline.multisample.rasterizationSamples = V3D_SAMPLE_COUNT_1;
		pipeline.multisample.sampleShadingEnable = false;
		pipeline.multisample.minSampleShading = 0.0f;
		pipeline.multisample.alphaToCoverageEnable = false;
		pipeline.multisample.alphaToOneEnable = false;
		pipeline.depthStencil.depthTestEnable = true;
		pipeline.depthStencil.depthWriteEnable = true;
		pipeline.depthStencil.depthCompareOp = V3D_COMPARE_OP_LESS;

		pipeline.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(BLEND_MODE_COPY);

		pipeline.pRenderPass = nullptr;
		pipeline.subpass = 0;
	}

	// ライティング : ディレクショナル
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_LIGHTING][GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING];
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
		pipeline.subpass = 0;
	}

	// ライティング : ポイント
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_LIGHTING][GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING];
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
		pipeline.subpass = 1;
	}

	// ライティング : フィニッシュ
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_LIGHTING][GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING];
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
		pipeline.subpass = 2;
	}

	// ブライトパス
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS][GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_BRIGHT_PASS];
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

	// ブラー : ダウンサンプリング
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_BLUR][GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_DOWN_SAMPLING_2x2];
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

	// ブラー : ガウシアン
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_BLUR][GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_GAUSSIAN_BLUR];
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

	// 合成 : １枚
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_COMPOSITE][GRAPHICS_PIPELINE_TYPE_COMPOSITE_1];
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
		pipeline.subpass = 0;
	}

	// 合成 : ２枚
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_COMPOSITE][GRAPHICS_PIPELINE_TYPE_COMPOSITE_2];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_COMPOSITE_2];
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
		pipeline.subpass = 0;
	}

	// SSAO
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_SSAO][GRAPHICS_PIPELINE_TYPE_SSAO];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_SSAO];
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
		pipeline.subpass = 0;
	}

	// ポストエフェクト : コピー
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT][GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY];
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
		pipeline.subpass = 0;
	}

	// ポストエフェクト : 補正
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT][GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_CORRECTION];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_CORRECTION];
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
		pipeline.subpass = 0;
	}

	// ポストエフェクト : トーンマッピング
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT][GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_TONE_MAPPING];
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
		pipeline.subpass = 0;
	}

	// ポストエフェクト : FXAA
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT][GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA];
		pipeline.Allocate(2, 1, 1);

		pipeline.vertexShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_VERT_SIMPLE];
		pipeline.vertexShader.pEntryPointName = GraphicsManager::BasicEntryPointName;
		pipeline.fragmentShader.pModule = m_pShaderModules[GraphicsManager::SHADER_TYPE_FRAG_FXAA];
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
		pipeline.subpass = 0;
	}

	// オーバーレイ : コピー
	{
		GraphicsManager::PipelineDesc& pipeline = m_PipelineDescs[GRAPHICS_RENDERPASS_TYPE_FINISH][GRAPHICS_PIPELINE_TYPE_FINISH_INIT];
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
		pipeline.subpass = 0;
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
		GraphicsManager::Stage& stage = m_Stages[i];

		for (uint32_t j = 0; j < GraphicsManager::MAX_SUBPASS; j++)
		{
			for (uint32_t k = 0; k < GRAPHICS_PIPELINE_TYPE_COUNT; k++)
			{
				GraphicsManager::PipelineHandleMap& pipelineHandleMap = stage.pipelineHandleMaps[j][k];

				if (pipelineHandleMap.empty() == false)
				{
					GraphicsManager::PipelineHandleMap::iterator it_begin = pipelineHandleMap.begin();
					GraphicsManager::PipelineHandleMap::iterator it_end = pipelineHandleMap.end();
					GraphicsManager::PipelineHandleMap::iterator it = pipelineHandleMap.begin();

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
		}

		if (stage.renderPassHandle != nullptr)
		{
			SAFE_RELEASE(stage.renderPassHandle->m_pRenderPass);
			SAFE_DELETE(stage.renderPassHandle);
		}
	}

	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		SAFE_RELEASE(m_Attachments[i].pImageView);
		SAFE_RELEASE(m_Attachments[i].pImage);
	}

	for (size_t i = 0; i < m_pPipelineLayouts.size(); i++)
	{
		SAFE_RELEASE(m_pPipelineLayouts[i]);
	}

	for (size_t i = 0; i < m_pDescriptorSetLayouts.size(); i++)
	{
		SAFE_RELEASE(m_pDescriptorSetLayouts[i]);
	}

	for (size_t i = 0; i < m_pShaderModules.size(); i++)
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
		GraphicsManager::Stage& stage = m_Stages[i];

		for (uint32_t j = 0; j < GraphicsManager::MAX_SUBPASS; j++)
		{
			for (uint32_t k = 0; k < GRAPHICS_PIPELINE_TYPE_COUNT; k++)
			{
				GraphicsManager::PipelineHandleMap& pipelineHandleMap = stage.pipelineHandleMaps[j][k];

				if (pipelineHandleMap.empty() == false)
				{
					GraphicsManager::PipelineHandleMap::iterator it_begin = pipelineHandleMap.begin();
					GraphicsManager::PipelineHandleMap::iterator it_end = pipelineHandleMap.end();
					GraphicsManager::PipelineHandleMap::iterator it;

					for (it = it_begin; it != it_end; ++it)
					{
						if (it->second != nullptr)
						{
							SAFE_RELEASE(it->second->m_pPipeline);
						}
					}
				}
			}
		}

		if (stage.renderPassHandle != nullptr)
		{
			SAFE_RELEASE(stage.renderPassHandle->m_pRenderPass);
		}
	}

	if (m_Attachments.empty() == false)
	{
		std::vector<GraphicsManager::Attachment>::iterator it_begin = m_Attachments.begin();
		std::vector<GraphicsManager::Attachment>::iterator it_end = m_Attachments.end();
		std::vector<GraphicsManager::Attachment>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			SAFE_RELEASE(it->pImageView);
			SAFE_RELEASE(it->pImage);
		}
	}

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

	const V3DSwapChainDesc& swapChainDesc = m_pSwapChain->GetDesc();

	std::vector<IV3DResource*> attachmenResources;
	attachmenResources.reserve(m_Attachments.size());

	m_Attachments.resize(GraphicsManager::IMAGE_TYPE_COUNT);

	// アタッチメントのイメージを作成
	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		switch (attachment.imageSizeType)
		{
		case GraphicsManager::IMAGE_SIZE_DEFAULT:
			attachment.imageDesc.width = swapChainDesc.imageWidth;
			attachment.imageDesc.height = swapChainDesc.imageHeight;
			break;
		case GraphicsManager::IMAGE_SIZE_HALF:
			attachment.imageDesc.width = MAXIMUM(1, swapChainDesc.imageWidth / 2);
			attachment.imageDesc.height = MAXIMUM(1, swapChainDesc.imageHeight / 2);
			break;
		case GraphicsManager::IMAGE_SIZE_QUARTER:
			attachment.imageDesc.width = MAXIMUM(1, swapChainDesc.imageWidth / 4);
			attachment.imageDesc.height = MAXIMUM(1, swapChainDesc.imageHeight / 4);
			break;
		}

		V3D_RESULT result = m_pDevice->CreateImage(attachment.imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &attachment.pImage);
		if (result != V3D_OK)
		{
			return result;
		}

		attachmenResources.push_back(attachment.pImage);
	}

	// アタッチメントのイメージをバインド
	V3D_RESULT result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, TO_UI32(attachmenResources.size()), attachmenResources.data());
	if (result != V3D_OK)
	{
		return result;
	}

	// アタッチメントのイメージビューを作成
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
		attachment.imageStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		attachment.imageAccessMask = V3D_ACCESS_MEMORY_READ;
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
			barrier.dstStageMask = attachment.imageStageMask;
			barrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			barrier.dstAccessMask = attachment.imageAccessMask;
			barrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			barrier.dstLayout = attachment.imageLayout;
			m_pCommandBuffer->BarrierImageView(attachment.pImageView, barrier);
		}
		else
		{
			V3DBarrierImageDesc barrier{};
			barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			barrier.dstStageMask = attachment.imageStageMask;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = attachment.imageAccessMask;
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
		GraphicsManager::Stage& stage = m_Stages[i];

		// レンダーパス
		if (stage.renderPassHandle != nullptr)
		{
			V3D_RESULT result = CreateNativeRenderPass(i, &stage.renderPassHandle->m_pRenderPass);
			if (result != V3D_OK)
			{
				return result;
			}
		}

		// グラフィックスパイプライン
		for (uint32_t j = 0; j < GraphicsManager::MAX_SUBPASS; j++)
		{
			for (uint32_t k = 0; k < GRAPHICS_PIPELINE_TYPE_COUNT; k++)
			{
				GraphicsManager::PipelineHandleMap& pipelineHandleMap = stage.pipelineHandleMaps[j][k];

				GraphicsManager::PipelineHandleMap::iterator it_begin = pipelineHandleMap.begin();
				GraphicsManager::PipelineHandleMap::iterator it_end = pipelineHandleMap.end();
				GraphicsManager::PipelineHandleMap::iterator it;

//				std::wstringstream stringStream;
//				stringStream << j << k << L"\n";
//				::OutputDebugString(stringStream.str().c_str());

				for (it = it_begin; it != it_end; ++it)
				{
					V3D_RESULT result = CreateNativePipeline(i, j, k, it->first, &it->second->m_pPipeline);
					if (result != V3D_OK)
					{
						return result;
					}
				}
			}
		}
	}

	// ----------------------------------------------------------------------------------------------------

	m_Lost = false;

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

void GraphicsManager::GetPipelineLayout(GRAPHICS_PIPELINE_TYPE pipelineType, IV3DPipelineLayout** ppPipelineLayout)
{
	SAFE_ADD_REF(m_pPipelineLayouts[pipelineType]);
	*ppPipelineLayout = m_pPipelineLayouts[pipelineType];
}

V3D_RESULT GraphicsManager::GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE renderpassType, GraphicsRenderPassHandle* pRenderPassHandle)
{
	GraphicsManager::Stage& stage = m_Stages[renderpassType];
	GraphicsRenderPassHandle renderPassHandle;

	if (stage.renderPassHandle != nullptr)
	{
		renderPassHandle = stage.renderPassHandle;
	}
	else
	{
		stage.renderPassHandle = new GraphicsRenderPassHandleT();
		if (stage.renderPassHandle == nullptr)
		{
			return V3D_ERROR_OUT_OF_HOST_MEMORY;
		}

		stage.renderPassHandle->m_pRenderPass = nullptr;
		renderPassHandle = stage.renderPassHandle;
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

V3D_RESULT GraphicsManager::GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE renderpassType, uint32_t subpass, GRAPHICS_PIPELINE_TYPE pipelineType, const GraphicsPipelienDesc& desc, GraphicsPipelineHandle* pPipelineHandle)
{
	GraphicsManager::PipelineHandleMap& pipelineHandleMap = m_Stages[renderpassType].pipelineHandleMaps[subpass][pipelineType];
	GraphicsPipelineHandle pipelineHandle;

	GraphicsManager::PipelineHandleMap::iterator it = pipelineHandleMap.find(desc);
	if (it != pipelineHandleMap.end())
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
		pipelineHandleMap[desc] = pipelineHandle;
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

		result = CreateNativePipeline(renderpassType, subpass, pipelineType, desc, &pipelineHandle->m_pPipeline);
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
	std::vector<IV3DImageView*> attachments;

	if (renderpassType == GRAPHICS_RENDERPASS_TYPE_GEOMETRY)
	{
		attachments.reserve(4);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_0].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_1].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW)
	{
		attachments.reserve(2);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_SSAO)
	{
		attachments.reserve(1);
		attachments.push_back(m_Attachments[IMAGE_TYPE_SSAO_COLOR].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_SHADOW)
	{
		attachments.reserve(2);
		attachments.push_back(m_Attachments[IMAGE_TYPE_SHADOW_BUFFER].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_SHADOW_DEPTH_STENCIL].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_LIGHTING)
	{
		attachments.reserve(2);
		attachments.push_back(m_Attachments[IMAGE_TYPE_LIGHT_COLOR].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT)
	{
		attachments.reserve(4);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_LDR_COLOR_0].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_LDR_COLOR_1].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_FINISH)
	{
		attachments.reserve(2);
		attachments.push_back(m_Attachments[IMAGE_TYPE_LDR_COLOR_1].pImageView);
		attachments.push_back(m_Attachments[IMAGE_TYPE_BACK_BUFFER + index].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS)
	{
		attachments.reserve(1);
		attachments.push_back(m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR].pImageView);
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_BLUR)
	{
		attachments.reserve(2);

		switch (index)
		{
		case 0:
			attachments.push_back(m_Attachments[IMAGE_TYPE_BLUR_2_0].pImageView);
			attachments.push_back(m_Attachments[IMAGE_TYPE_BLUR_2_1].pImageView);
			break;
		case 1:
			attachments.push_back(m_Attachments[IMAGE_TYPE_BLUR_4_0].pImageView);
			attachments.push_back(m_Attachments[IMAGE_TYPE_BLUR_4_1].pImageView);
			break;
		}
	}
	else if (renderpassType == GRAPHICS_RENDERPASS_TYPE_COMPOSITE)
	{
		attachments.reserve(1);
		attachments.push_back(m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0 + index].pImageView);
	}

	V3D_RESULT result = m_pDevice->CreateFrameBuffer(m_Stages[renderpassType].renderPassHandle->m_pRenderPass, TO_UI32(attachments.size()), attachments.data(), ppFrameBuffer);
	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

V3D_RESULT GraphicsManager::CreateDescriptorSet(GRAPHICS_PIPELINE_TYPE pipelineType, GRAPHICS_DESCRIPTOR_SET_TYPE descriptorSetType, IV3DDescriptorSet** ppDescriptorSet)
{
	IV3DDescriptorSetLayout* pDescriptorSetLayout;

	m_pPipelineLayouts[pipelineType]->GetDescriptorSetLayout(descriptorSetType, &pDescriptorSetLayout);

	V3D_RESULT result = m_pDevice->CreateDescriptorSet(pDescriptorSetLayout, ppDescriptorSet);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(pDescriptorSetLayout);
		return result;
	}

	SAFE_RELEASE(pDescriptorSetLayout);

	return result;
}

V3D_RESULT GraphicsManager::CreateNativeRenderPass(uint32_t renderpassType, IV3DRenderPass** ppRenderPass)
{
	const GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[renderpassType];

	switch (renderpassType)
	{
	case GRAPHICS_RENDERPASS_TYPE_FINISH:
		desc.pAttachments[GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER].format = m_Attachments[IMAGE_TYPE_BACK_BUFFER].imageDesc.format;
		desc.pAttachments[GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER].samples = m_Attachments[IMAGE_TYPE_BACK_BUFFER].imageDesc.samples;
		break;
	}

	V3D_RESULT result = m_pDevice->CreateRenderPass(
		desc.attachmentCount, desc.pAttachments,
		desc.subpassCount, desc.pSubpasses,
		desc.subpassDependencyCount, desc.pSubpassDependencies,
		ppRenderPass);

	if (result != V3D_OK)
	{
		return result;
	}

	return V3D_OK;
}

V3D_RESULT GraphicsManager::CreateNativePipeline(uint32_t renderpassType, uint32_t subpass, uint32_t pipelineType, const GraphicsPipelienDesc& desc, IV3DPipeline** ppPipeline)
{
	const V3DSwapChainDesc& swapChainDesc = m_pSwapChain->GetDesc();
	GraphicsManager::PipelineDesc& pipelineDesc = m_PipelineDescs[renderpassType][pipelineType];

	pipelineDesc.rasterization.polygonMode = desc.polygonMode;
	pipelineDesc.rasterization.cullMode = desc.cullMode;
	pipelineDesc.depthStencil.depthTestEnable = desc.depthTestEnable;
	pipelineDesc.depthStencil.depthWriteEnable = desc.depthWriteEnable;
	pipelineDesc.colorBlend.pAttachments[0] = InitializeColorBlendAttachment(desc.blendMode);
	pipelineDesc.pRenderPass = m_Stages[renderpassType].renderPassHandle->m_pRenderPass;
	pipelineDesc.subpass = subpass;

	V3D_RESULT result = m_pDevice->CreateGraphicsPipeline(m_pPipelineLayouts[pipelineType], pipelineDesc, ppPipeline);
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
