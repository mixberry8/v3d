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
	// �V�F�[�_�[���W���[�����쐬
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
	// �f�X�N���v�^�Z�b�g���C�A�E�g���쐬
	// ----------------------------------------------------------------------------------------------------

	// �V���v���T���v�����O
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

	// �W�I���g�� - ���b�V��
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

	// �W�I���g�� - �}�e���A��
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

	// �W�I���g�� - �X�J�C�h�[��
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

	// �f�B���N�V���i�����C�e�B���O
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

	// �|�C���g���C�e�B���O
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

	// �t�B�j�b�V�����C�e�B���O
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

	// �Q�����������ďo��
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
/*
		result = m_pDevice->CreateDescriptorSetLayout(
			TO_UI32(descriptors.size()), descriptors.data(),
			&m_pDescriptorSetLayouts[GraphicsManager::DESCRIPTOR_SET_TYPE_SSAO]);
*/
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// �p�C�v���C�����C�A�E�g���쐬
	// ----------------------------------------------------------------------------------------------------

	// �W�I���g�� : ���b�V�� & �}�e���A��
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

	// �W�I���g�� : �X�J�C�h�[��
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

	// �V���h�E
	{
		IV3DPipelineLayout* pPipelineLayout = m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_GEOMETRY];

		SAFE_ADD_REF(pPipelineLayout);
		m_pPipelineLayouts[GRAPHICS_PIPELINE_TYPE_SHADOW] = pPipelineLayout;
	}

	// ���C�e�B���O : �f�B���N�V���i��
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

	// ���C�e�B���O : �|�C���g
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

	// ���C�e�B���O : �t�B�j�b�V��
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

	// �u���C�g�p�X
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

	// �u���[ : �_�E���T���v�����O
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

	// �u���[ : �K�E�V�A��
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

	// ���� : �P��
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

	// ���� : �Q��
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

	// �|�X�g�G�t�F�N�g : �R�s�[
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

	// �|�X�g�G�t�F�N�g : �␳
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

	// �|�X�g�G�t�F�N�g : �g�[���}�b�s���O
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

	// �|�X�g�G�t�F�N�g : FXAA
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

	// �I�[�o�[���C : �R�s�[
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
	// �A�^�b�`�����g�̋L�q
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

		// �W�I���g�� : �J���[
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
		m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_0].name = L"geometryColor0";
		m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1] = attachment;
		m_Attachments[IMAGE_TYPE_GEOMETRY_COLOR_1].name = L"geometryColor1";

		// �W�I���g�� : �o�b�t�@�[ ( rgb=�@�� a=�s�N�Z�����������܂ꂽ���ǂ��� 0 or 1 )
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
		m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_0].name = L"geometryBuffer ( normal )";

		// �W�I���g�� : �o�b�t�@�[ ( rgb=�}�e���A�� a=�w���[�x )
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
		m_Attachments[IMAGE_TYPE_GEOMETRY_BUFFER_1].name = L"geometryBuffer ( material depth )";

		// �W�I���g�� : �f�v�X�X�e���V��
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
		m_Attachments[IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL].name = L"geometryDepthStencil";

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
		m_Attachments[IMAGE_TYPE_SSAO_COLOR].name = L"ambientOcclusion";

		// �V���h�E : �o�b�t�@�[ ( rgb=�F a=�w���[�x )
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
		m_Attachments[IMAGE_TYPE_SHADOW_BUFFER].name = L"shadowBuffer";

		// �V���h�E : �f�v�X�X�e���V��
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
		m_Attachments[IMAGE_TYPE_SHADOW_DEPTH_STENCIL].name = L"shadowDepthStencil";

		// ���C�e�B���O
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
		m_Attachments[IMAGE_TYPE_LIGHT_COLOR].name = L"lightColor";

		// LDR : �J���[
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
		m_Attachments[IMAGE_TYPE_LDR_COLOR_0].name = L"ldrColor0";
		m_Attachments[IMAGE_TYPE_LDR_COLOR_1] = attachment;
		m_Attachments[IMAGE_TYPE_LDR_COLOR_1].name = L"ldrColor1";

		// �u���C�g�p�X
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
		m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR] = attachment;
		m_Attachments[IMAGE_TYPE_BRIGHT_PASS_COLOR].name = L"brightPassColor";

		// �u���[
		attachment.imageDesc.format = unorm8BitFormat;
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
		m_Attachments[IMAGE_TYPE_BLUR_2_0].name = L"blurColor2_0";
		m_Attachments[IMAGE_TYPE_BLUR_2_1] = attachment;
		m_Attachments[IMAGE_TYPE_BLUR_2_1].name = L"blurColor2_1";

		attachment.imageSizeType = GraphicsManager::IMAGE_SIZE_QUARTER;
		m_Attachments[IMAGE_TYPE_BLUR_4_0] = attachment;
		m_Attachments[IMAGE_TYPE_BLUR_4_0].name = L"blurColor4_0";
		m_Attachments[IMAGE_TYPE_BLUR_4_1] = attachment;
		m_Attachments[IMAGE_TYPE_BLUR_4_1].name = L"blurColor4_1";
	}

	// ----------------------------------------------------------------------------------------------------
	// �����_�[�p�X�̋L�q
	// ----------------------------------------------------------------------------------------------------

	// �W�I���g��
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
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
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
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
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
		pAttachments[2].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
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
		pAttachments[3].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[3].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[3].clearValue.depthStencil.depth = 1.0f;
		pAttachments[3].clearValue.depthStencil.stencil = 0;

		// �T�u�p�X 0 - �W�I���g��
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 3, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->colorAttachments[1].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0;
		pSubpass->colorAttachments[1].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->colorAttachments[2].attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1;
		pSubpass->colorAttachments[2].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = 0;
	}

	// ���i
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW];

		desc.Allocate(2, 1, 2);

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

		// �T�u�p�X 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_READ | V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = 0;

		pSubpassDependencies[1].srcSubpass = 0;
		pSubpassDependencies[1].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_READ | V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dependencyFlags = 0;
	}

	// �V���h�E
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
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
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
		pAttachments[1].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].finalLayout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
		pAttachments[1].clearValue.depthStencil.depth = 1.0f;
		pAttachments[1].clearValue.depthStencil.stencil = 0;

		// �T�u�p�X 0 - �v���W�F�N�V����
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, true, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_SHADOW_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pSubpass->depthStencilAttachment.attachment = GRAPHICS_ATTACHMENT_TYPE_SHADOW_DEPTH_STENCIL;
		pSubpass->depthStencilAttachment.layout = V3D_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT;
	
		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = 0;
	}

	// ���C�e�B���O
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
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
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

		// �T�u�p�X 0 - �f�B���N�V���i�����C�e�B���O
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 1 - �|�C���g���C�e�B���O
		pSubpass = desc.AllocateSubpass(1, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 2 - �t�B�j�b�V�����C�e�B���O
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COMPOSITE_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		// �f�B���N�V���i�����C�e�B���O �� �|�C���g���C�e�B���O
		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = 1;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// �|�C���g���C�e�B���O �� �t�B�j�b�V�����C�e�B���O
		pSubpassDependencies[1].srcSubpass = 1;
		pSubpassDependencies[1].dstSubpass = 2;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		// �t�B�j�b�V�����C�e�B���O �� �����_�[�p�X�I��
		pSubpassDependencies[2].srcSubpass = 2;
		pSubpassDependencies[2].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[2].dependencyFlags = 0;
	}

	// �u���C�g�p�X
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

		// �T�u�p�X 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BRIGHT_PASS_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = 0;
	}

	// �u���[
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

		// �T�u�p�X 0 : �_�E���T���v�����O
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 1 : ���u���[
		pSubpass = desc.AllocateSubpass(1, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_1;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 2 : �c�u���[
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_1;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
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
		pSubpassDependencies[2].dependencyFlags = 0;
	}

	// ����
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

		// �T�u�p�X 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_COMPOSITE_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_READ | V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = 0;
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
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pAttachments[0].clearValue.color.float32[0] = 1.0f;
		pAttachments[0].clearValue.color.float32[1] = 1.0f;
		pAttachments[0].clearValue.color.float32[2] = 1.0f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		// �T�u�p�X 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_SSAO_COLOR;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dependencyFlags = 0;
	}

	// �C���[�W�G�t�F�N�g
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT];

		desc.Allocate(4, 3, 4);

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
		pAttachments[3].finalLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;

		// �T�u�p�X 0 : HDR
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_1;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 1 : HDR to LDR ( ToneMapping )
		pSubpass = desc.AllocateSubpass(1, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 2 : LDR ( FXAA )
		pSubpass = desc.AllocateSubpass(2, 1, 1, false, 0);
		pSubpass->inputAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0;
		pSubpass->inputAttachments[0].layout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_1;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;

		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = 0;

		pSubpassDependencies[1].srcSubpass = 0;
		pSubpassDependencies[1].dstSubpass = 1;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;// V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;// V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;// V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_SHADER_READ;// V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[2].srcSubpass = 1;
		pSubpassDependencies[2].dstSubpass = 2;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[2].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[3].srcSubpass = 2;
		pSubpassDependencies[3].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[3].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[3].dstStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pSubpassDependencies[3].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[3].dstAccessMask = V3D_ACCESS_SHADER_READ;
		pSubpassDependencies[3].dependencyFlags = 0;
	}

	// �t�B�j�b�V��
	{
		GraphicsManager::RenderPassDesc& desc = m_RenderPassDescs[GRAPHICS_RENDERPASS_TYPE_FINISH];

		desc.Allocate(1, 2, 1);

		V3DAttachmentDesc* pAttachments = desc.pAttachments;
		
		pAttachments[0].format = V3D_FORMAT_UNDEFINED; // �o�b�N�o�b�t�@�[
		pAttachments[0].samples = V3D_SAMPLE_COUNT_1;
		pAttachments[0].loadOp = V3D_ATTACHMENT_LOAD_OP_CLEAR;
		pAttachments[0].storeOp = V3D_ATTACHMENT_STORE_OP_STORE;
		pAttachments[0].stencilLoadOp = V3D_ATTACHMENT_LOAD_OP_UNDEFINED;
		pAttachments[0].stencilStoreOp = V3D_ATTACHMENT_STORE_OP_UNDEFINED;
		pAttachments[0].initialLayout = V3D_IMAGE_LAYOUT_UNDEFINED;// V3D_IMAGE_LAYOUT_PRESENT_SRC;
		pAttachments[0].finalLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pAttachments[0].clearValue.color.float32[0] = 0.0f;
		pAttachments[0].clearValue.color.float32[1] = 0.0f;
		pAttachments[0].clearValue.color.float32[2] = 0.0f;
		pAttachments[0].clearValue.color.float32[3] = 1.0f;

		// �T�u�p�X 0
		RenderPassDesc::Subpass* pSubpass = desc.AllocateSubpass(0, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X 1
		pSubpass = desc.AllocateSubpass(1, 0, 1, false, 0);
		pSubpass->colorAttachments[0].attachment = GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER;
		pSubpass->colorAttachments[0].layout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		// �T�u�p�X�̈ˑ���
		V3DSubpassDependencyDesc* pSubpassDependencies = desc.pSubpassDependencies;
/*
		pSubpassDependencies[0].srcSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[0].dstSubpass = 0;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_MEMORY_READ;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = 0;

		pSubpassDependencies[1].srcSubpass = 0;
		pSubpassDependencies[1].dstSubpass = 1;
		pSubpassDependencies[1].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[1].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[1].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

		pSubpassDependencies[2].srcSubpass = 1;
		pSubpassDependencies[2].dstSubpass = V3D_SUBPASS_EXTERNAL;
		pSubpassDependencies[2].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[2].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[2].dependencyFlags = 0;
*/
		pSubpassDependencies[0].srcSubpass = 0;
		pSubpassDependencies[0].dstSubpass = 1;
		pSubpassDependencies[0].srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pSubpassDependencies[0].srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		pSubpassDependencies[0].dependencyFlags = V3D_DEPENDENCY_BY_REGION;

	}

	// ----------------------------------------------------------------------------------------------------
	// �O���t�B�b�N�X�p�C�v���C���̋L�q
	// ----------------------------------------------------------------------------------------------------

	// �W�I���g�� - ���b�V�� & �}�e���A��
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

	// ���i - �X�J�C�h�[��
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

	// �V���h�E
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

	// ���C�e�B���O : �f�B���N�V���i��
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

	// ���C�e�B���O : �|�C���g
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

	// ���C�e�B���O : �t�B�j�b�V��
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

	// �u���C�g�p�X
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

	// �u���[ : �_�E���T���v�����O
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

	// �u���[ : �K�E�V�A��
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

	// ���� : �P��
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

	// ���� : �Q��
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

	// �C���[�W�G�t�F�N�g : �R�s�[
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

	// �C���[�W�G�t�F�N�g : �␳
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

	// �C���[�W�G�t�F�N�g : �g�[���}�b�s���O
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

	// �C���[�W�G�t�F�N�g : FXAA
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

	// �t�B�j�b�V�� : �R�s�[
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
	// �R�}���h�o�b�t�@���쐬
	// ----------------------------------------------------------------------------------------------------

	{
		V3DCommandPoolDesc commandPoolDesc{};
		commandPoolDesc.queueFamily = m_pQueue->GetFamily();
		commandPoolDesc.usageFlags = V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER;

		result = m_pDevice->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_PRIMARY, &m_pCommandBuffer);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// ----------------------------------------------------------------------------------------------------
	// �t�F���X���쐬
	// ----------------------------------------------------------------------------------------------------

	result = m_pDevice->CreateFence(false, &m_pFence);
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
	// �C���[�W�𕜋A
	// ----------------------------------------------------------------------------------------------------

	const V3DSwapChainDesc& swapChainDesc = m_pSwapChain->GetDesc();

	std::vector<IV3DResource*> attachmenResources;
	attachmenResources.reserve(m_Attachments.size());

	m_Attachments.resize(GraphicsManager::IMAGE_TYPE_COUNT);

	// �A�^�b�`�����g�̃C���[�W���쐬
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

		V3D_RESULT result = m_pDevice->CreateImage(attachment.imageDesc, V3D_IMAGE_LAYOUT_UNDEFINED, &attachment.pImage, attachment.name.c_str());
		if (result != V3D_OK)
		{
			return result;
		}

		attachmenResources.push_back(attachment.pImage);
	}

	// �A�^�b�`�����g�̃C���[�W���o�C���h
	V3D_RESULT result = m_pDevice->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, TO_UI32(attachmenResources.size()), attachmenResources.data());
	if (result != V3D_OK)
	{
		return result;
	}

	// �A�^�b�`�����g�̃C���[�W�r���[���쐬
	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		V3D_RESULT result = m_pDevice->CreateImageView(attachment.pImage, attachment.imageViewDesc, &attachment.pImageView);
		if (result != V3D_OK)
		{
			return result;
		}
	}

	// �X���b�v�`�F�C���̃C���[�W�r���[���쐬
	for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
	{
		m_Attachments.push_back(GraphicsManager::Attachment{});
		GraphicsManager::Attachment& attachment = m_Attachments.back();

		m_pSwapChain->GetImage(i, &attachment.pImage);
		attachment.imageDesc = attachment.pImage->GetDesc();
		attachment.imageStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;// V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		attachment.imageAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;// V3D_ACCESS_MEMORY_READ;
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
	// �C���[�W��������
	// ----------------------------------------------------------------------------------------------------

	IV3DCommandBuffer* pCommandbuffer = ResetCommandBuffer();
	if (pCommandbuffer == nullptr)
	{
		return V3D_ERROR_FAIL;
	}

	for (size_t i = 0; i < m_Attachments.size(); i++)
	{
		GraphicsManager::Attachment& attachment = m_Attachments[i];

		V3DPipelineBarrier pipelineBarrier{};

		V3DImageViewMemoryBarrier memoryBarrier{};
		memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;

		if (attachment.imageDesc.usageFlags & V3D_IMAGE_USAGE_TRANSFER_DST)
		{
			pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;// V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_TRANSFER;

			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			memoryBarrier.dstLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			memoryBarrier.pImageView = attachment.pImageView;
			m_pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

			m_pCommandBuffer->ClearImageView(attachment.pImageView, V3D_IMAGE_LAYOUT_TRANSFER_DST, V3DClearValue{});

			pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_TRANSFER;
			pipelineBarrier.dstStageMask = attachment.imageStageMask;

			memoryBarrier.srcAccessMask = V3D_ACCESS_TRANSFER_WRITE;
			memoryBarrier.dstAccessMask = attachment.imageAccessMask;
			memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_TRANSFER_DST;
			memoryBarrier.dstLayout = attachment.imageLayout;
			memoryBarrier.pImageView = attachment.pImageView;
			m_pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);
		}
		else
		{
			pipelineBarrier.srcStageMask = attachment.imageStageMask;// V3D_PIPELINE_STAGE_TOP_OF_PIPE;
			pipelineBarrier.dstStageMask = attachment.imageStageMask;

			memoryBarrier.srcAccessMask = 0;
			memoryBarrier.dstAccessMask = attachment.imageAccessMask;
			memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_UNDEFINED;
			memoryBarrier.dstLayout = attachment.imageLayout;
			memoryBarrier.pImageView = attachment.pImageView;
			m_pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);
		}
	}

	if (FlushCommandBuffer() == false)
	{
		return V3D_ERROR_FAIL;
	}

	// ----------------------------------------------------------------------------------------------------
	// �����_�[�p�X�A�O���t�B�b�N�X�p�C�v���C���𕜋A
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < GRAPHICS_RENDERPASS_TYPE_COUNT; i++)
	{
		GraphicsManager::Stage& stage = m_Stages[i];

		// �����_�[�p�X
		if (stage.renderPassHandle != nullptr)
		{
			V3D_RESULT result = CreateNativeRenderPass(i, &stage.renderPassHandle->m_pRenderPass);
			if (result != V3D_OK)
			{
				return result;
			}
		}

		// �O���t�B�b�N�X�p�C�v���C��
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
		// �n���h�����擾���ă����_�[�p�X�쐬�𑣂�
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
		attachments.reserve(1);
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

	const wchar_t* pDebugName = nullptr;
#ifdef _DEBUG
	std::wstringstream debugStringStream;
	debugStringStream << L"DS_PT" << pipelineType << L"_DST" << descriptorSetType;
	std::wstring debugString = debugStringStream.str();
	pDebugName = debugString.c_str();
#endif //_DEBUG

	V3D_RESULT result = m_pDevice->CreateDescriptorSet(pDescriptorSetLayout, ppDescriptorSet, pDebugName);
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
