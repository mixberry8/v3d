#include "stdafx.h"
#include "Application.h"
#include "Window.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Font.h"
#include "NamingService.h"
#include "ParallelManager.h"
#include "Frustum.h"

// ----------------------------------------------------------------------------------------------------

static auto constexpr SCREEN_WIDTH = 1024;
static auto constexpr SCREEN_HEIGHT = 768;
static auto constexpr BUFFER_COUNT = 2;

static auto constexpr MESH_X_COUNT = 1;
static auto constexpr MESH_Y_COUNT = 1;
static auto constexpr MESH_Z_COUNT = 1;
static constexpr float MESH_INTERVAL = 5.0f;

static auto constexpr POINT_LIGHT_X_COUNT = 0;// 5;
static auto constexpr POINT_LIGHT_Y_COUNT = 0;// 3;
static auto constexpr POINT_LIGHT_Z_COUNT = 0;// 1;
static constexpr float POINT_LIGHT_OX = 0.0f;
static constexpr float POINT_LIGHT_OY = -100.0f;
static constexpr float POINT_LIGHT_OZ = -180.0f;
static constexpr float POINT_LIGHT_INTERVAL = 600.0f;
static constexpr float POINT_LIGHT_RANGE = 350;// 600.0f;

//static auto constexpr POINT_LIGHT_X_COUNT = 3;
//static auto constexpr POINT_LIGHT_Y_COUNT = 3;
//static auto constexpr POINT_LIGHT_Z_COUNT = 3;
//static constexpr float POINT_LIGHT_OX = 0.0f;
//static constexpr float POINT_LIGHT_OY = 0.0f;
//static constexpr float POINT_LIGHT_OZ = 0.0f;
//static constexpr float POINT_LIGHT_INTERVAL = 20.0f;
//static constexpr float POINT_LIGHT_RANGE = 10.0f;

static auto constexpr MATERIAL_MAX = 1000;

static auto constexpr MESH_MAX = MESH_X_COUNT * MESH_Y_COUNT * MESH_Z_COUNT;
static auto constexpr MESH_UNIFORM_MAX = 6000;// MESH_MAX * 400;
static auto constexpr MESH_COUNT = MESH_MAX;

static auto constexpr POINT_LIGHT_COUNT = POINT_LIGHT_X_COUNT * POINT_LIGHT_Y_COUNT * POINT_LIGHT_Z_COUNT;


static constexpr uint32_t SPACE_SIZE_I = 50;
static constexpr uint32_t SPACE_ACC_I = 10;
static constexpr uint32_t SPACE_SIZE_ACC_I = SPACE_SIZE_I * SPACE_ACC_I;
static constexpr float SPACE_HALF_SIZE = static_cast<float>(SPACE_SIZE_I) * 0.5f;
static constexpr float SPACE_ACC = static_cast<float>(SPACE_ACC_I);

#define ENABLE_MULTITHREAD

struct DrawOpacitySet
{
	uint64_t materialKey;
	uint64_t meshKey;

	IV3DPipelineLayout* pPipelineLayout;
	IV3DPipeline* pPipeline;

	IV3DDescriptorSet* pMaterialDescriptorSet;
	uint32_t materialDynamicOffset;

	IV3DDescriptorSet* pMeshDescriptorSet;
	uint32_t meshDynamicOffset;
	IV3DBuffer* pMeshBuffer;
	uint64_t meshVertexOffset;
	uint64_t meshIndexOffset;
	MeshSubset meshSubset;
};

// ----------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------

class TestWindow : public Window
{
public:
	TestWindow() :
		m_GeometryRenderPassHandle(nullptr),
		m_pGeometryPipelineLayout(nullptr),
		m_pGeometryFrameBuffer(nullptr),
		m_GeometryParallelData({}),
		m_LightingRenderPassHandle(nullptr),
		m_pLightingFrameBuffer(nullptr),
		m_ShadowRenderPassHandle(nullptr),
		m_pShadowPipelineLayout(nullptr),
		m_ShadowMappingPipelineHandle(nullptr),
		m_pShadowFrameBuffer(nullptr),
		m_ShadowParallelData({}),
		m_ShadowMappingRenderPassHandle(nullptr),
		m_pShadowMappingPipelineLayout(nullptr),
		m_pShadowMappingFrameBuffer(nullptr),
		m_pShadowMappingDescriptorSet(nullptr),
		m_FinishRenderPassHandle(nullptr),
		m_pSimpleVertexBuffer(nullptr),
		m_pSimpleSampler(nullptr),
		m_DirectionalLightingStage({}),
		m_PointLightingStage({}),
		m_FinishLightingStage({}),
		m_CameraDistance(5.0f),
		m_CameraAt(Vector3::Zero()),
		m_bDrag(false),
		m_MousePos({})
	{
		m_CameraRot.SetRotationAxis(Vector3::XAxis(), DEG_TO_RAD(30.0f));
	}

	virtual ~TestWindow()
	{
	}

protected:
	virtual bool OnInitialize() override
	{
		// ----------------------------------------------------------------------------------------------------
		// 基本的なものを初期化
		// ----------------------------------------------------------------------------------------------------

		if (m_ParallelManager.Initialize() == false)
		{
			return false;
		}

		if (m_GraphicsManager.Initialize(Application::GetDevice(), GetWorkQueue(), GetSwapChain()) != V3D_OK)
		{
			return false;
		}

		if (m_TextureManager.Initialize(&m_GraphicsManager) != V3D_OK)
		{
			return false;
		}

		if (m_MaterialManager.Initialize(&m_GraphicsManager, &m_TextureManager, MATERIAL_MAX) != V3D_OK)
		{
			return false;
		}

		if (m_MeshManager.Initialize(&m_GraphicsManager, &m_TextureManager, &m_MaterialManager, MESH_UNIFORM_MAX) != V3D_OK)
		{
			return false;
		}

		if (m_Font.Initialize(Application::GetDevice(), L"Arial", 16, FW_NORMAL) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// シンプルバーテックスバッファを作成
		// ----------------------------------------------------------------------------------------------------

		{
			SimpleVertex simpleVertices[] =
			{
				{ Vector4(+1.0f, -1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
				{ Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
				{ Vector4(+1.0f, +1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
				{ Vector4(-1.0f, +1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
			};

			V3DBufferDesc bufferDesc{};
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_VERTEX;
			bufferDesc.size = sizeof(simpleVertices);

			if (Application::GetDevice()->CreateBuffer(bufferDesc, &m_pSimpleVertexBuffer) != V3D_OK)
			{
				return false;
			}

			if (Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, m_pSimpleVertexBuffer) != V3D_OK)
			{
				return false;
			}

			void* pMemory;
			if (m_pSimpleVertexBuffer->Map(0, V3D_WHOLE_SIZE, &pMemory) == V3D_OK)
			{
				MemCopy(pMemory, m_pSimpleVertexBuffer->GetResourceDesc().memorySize, simpleVertices, sizeof(simpleVertices));
				m_pSimpleVertexBuffer->Unmap();
			}
			else
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// シンプルサンプラーを作成
		// ----------------------------------------------------------------------------------------------------

		{
			V3DSamplerDesc samplerDesc{};
			samplerDesc.magFilter = V3D_FILTER_NEAREST;
			samplerDesc.minFilter = V3D_FILTER_NEAREST;
			samplerDesc.mipmapMode = V3D_MIPMAP_MODE_NEAREST;
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_CLAMP_TO_BORDER;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			if (Application::GetDevice()->CreateSampler(samplerDesc, &m_pSimpleSampler) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// メッシュを作成
		// ----------------------------------------------------------------------------------------------------

		{
			bool serialize = false;
			std::wstring meshName = L"mesh\\sanMiguel";

			float scale = 1.0f;
			uint32_t flags = MESH_LOAD_INVERT_TEX_V | MESH_LOAD_FLIP_FACE/* | MESH_LOAD_INVERT_NORMAL*/ | MESH_LOAD_SMOOSING;// | MESH_LOAD_NO_MASK_TEXTURE;// | MESH_LOAD_NO_OPTIMIZE;// | MESH_LOAD_NO_MASK_TEXTURE;// | ;// | MESH_LOAD_NO_TEXTURE;
//			float scale = 0.02f;
//			uint32_t flags = MESH_LOAD_INVERT_NORMAL | MESH_LOAD_SMOOSING;

			Matrix4x4 scaleMat;
			scaleMat.SetScale(Vector3(scale, scale, scale));

			MeshImportDesc meshImportDesc{};
			meshImportDesc.flags = flags;
			meshImportDesc.matrix = Matrix4x4(Quaternion(Vector3::ZAxis(), DEG_TO_RAD(180.0f))) * scaleMat;
			meshImportDesc.smoosingAngle = DEG_TO_RAD(80.0f);

			SkeletalMeshPtr mesh;

			if (serialize == true)
			{
				std::wstring objMeshName = meshName;
				objMeshName.append(L".obj");

				mesh = m_MeshManager.ImportSkeletal(objMeshName.c_str(), meshImportDesc);
			}
			else
			{
				std::wstring mdlMeshName = meshName;
				mdlMeshName.append(L".mdl");

				mesh = m_MeshManager.LoadSkeletal(mdlMeshName.c_str());
			}

			if (mesh == nullptr)
			{
				return false;
			}

//			mesh->GetMaterial(0)->SetSpecularPower(50.0f);
//			mesh->GetMaterial(0)->SetShininess(1.0f);
//			mesh->GetMaterial(0)->SetCullMode(V3D_CULL_MODE_NONE);
//			mesh->GetMaterial(0)->Update();

			if (serialize == true)
			{
				std::wstring mdlMeshName = meshName;
				mdlMeshName.append(L".mdl");

				std::wstring mdlFilePath;

				CreateFilePath(mdlMeshName.c_str(), mdlFilePath);
				mesh->Save(mdlFilePath.c_str());
			}

			m_Meshes.reserve(MESH_COUNT);
			m_Meshes.push_back(mesh);

			uint32_t cloneCount = MESH_COUNT - 1;
			for (uint32_t i = 0; i < cloneCount; i++)
			{
				m_Meshes.push_back(mesh->Clone());
			}

			if (MESH_COUNT == 1)
			{
				Matrix4x4 worldMat;
				worldMat.SetTranslation(Vector3(0, 0.0f, 0));

				mesh->SetWorldMatrix(worldMat);
			}
			else
			{
				const float basePosX = -(MESH_INTERVAL * MESH_X_COUNT) / 2.0f;
				const float basePosY = -(MESH_INTERVAL * MESH_Y_COUNT) / 2.0f;
				const float basePosZ = -(MESH_INTERVAL * MESH_Z_COUNT) / 2.0f;

				uint32_t meshIndex = 0;

				for (int32_t x = 0; x < MESH_X_COUNT; x++)
				{
					float wx = basePosX + MESH_INTERVAL * TO_F32(x);

					for (int32_t y = 0; y < MESH_Y_COUNT; y++)
					{
						float wy = basePosY + MESH_INTERVAL * TO_F32(y);

						for (int32_t z = 0; z < MESH_Z_COUNT; z++)
						{
							float wz = basePosZ + MESH_INTERVAL * TO_F32(z);

							float ox = static_cast<float>(rand() % 100) / 100.0f * 2.0f - 1.0f;
							float oy = static_cast<float>(rand() % 100) / 100.0f * 2.0f - 1.0f;
							float oz = static_cast<float>(rand() % 100) / 100.0f * 2.0f - 1.0f;

							Matrix4x4 worldMat;
							worldMat.SetTranslation(Vector3(wx + ox, wy + oy, wz + oz));

							Vector3 rotAxis;
							rotAxis.x = static_cast<float>(rand() % 1000) / 1000.0f;
							rotAxis.y = static_cast<float>(rand() % 1000) / 1000.0f;
							rotAxis.z = static_cast<float>(rand() % 1000) / 1000.0f;
							rotAxis.Normalize();

							float rotAngle = static_cast<float>(rand() % 360) * 0.5f - 180.0f;

							worldMat *= Quaternion(rotAxis, rotAngle);

							m_Meshes[meshIndex]->SetWorldMatrix(worldMat);
							meshIndex++;
						}
					}
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// ジオメトリ
		// ----------------------------------------------------------------------------------------------------

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_GEOMETRY, GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pGeometryPipelineLayout);

		// レンダーパスのハンドルを取得
		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_GEOMETRY, &m_GeometryRenderPassHandle);

		// ----------------------------------------------------------------------------------------------------
		// ライティング
		// ----------------------------------------------------------------------------------------------------

		// レンダーパスのハンドルを取得
		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_LIGHTING, &m_LightingRenderPassHandle);

		// ディレクショナルライティングのユニフォームバッファーを作成
		{
			V3DBufferDesc bufferDesc;
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
			bufferDesc.size = sizeof(DirectionalLightingUniform);

			if (Application::GetDevice()->CreateBuffer(bufferDesc, &m_DirectionalLightingStage.pUniformBuffer) != V3D_OK)
			{
				return false;
			}
		}

		// ポイントライティングのユニフォームバッファーを作成
		{
			V3DBufferDesc bufferDesc;
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
			bufferDesc.size = sizeof(PointLightingUniform);

			if (Application::GetDevice()->CreateBuffer(bufferDesc, &m_PointLightingStage.pUniformBuffer) != V3D_OK)
			{
				return false;
			}
		}

		// ユニフォームバッファーをメモリにバインド
		{
			Array1<IV3DResource*, 2> resources =
			{
				m_DirectionalLightingStage.pUniformBuffer,
				m_PointLightingStage.pUniformBuffer
			};

			if (Application::GetDevice()->AllocateResourceMemoryAndBind(V3D_MEMORY_PROPERTY_HOST_VISIBLE, TO_UI32(resources.size()), resources.data()) != V3D_OK)
			{
				return false;
			}
		}

		// ディレクショナルライティングの初期化
		{
			m_LightDir = Vector3(0.0f, 1.0f, 0.0f).ToNormalize();

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_LIGHTING, GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING, &m_DirectionalLightingStage.pPipelineLayout);

			// グラフィックスパイプラインのハンドルを取得
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;	
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_ADDITION;
			if (m_GraphicsManager.GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_LIGHTING, GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING, desc, &m_DirectionalLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

			// ユニフォームバッファーを更新
			void* pMemory;
			if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(DirectionalLightingUniform), &pMemory) == V3D_OK)
			{
				DirectionalLightingUniform uniform{};
				uniform.directionalLightCount = 1;
				uniform.directionalLights[0].dir = m_LightDir;
				uniform.directionalLights[0].color.Set(1.0f, 1.0f, 1.0f, 1.0f);

				memcpy_s(pMemory, sizeof(DirectionalLightingUniform), &uniform, sizeof(DirectionalLightingUniform));

				m_DirectionalLightingStage.pUniformBuffer->Unmap();
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_RENDERPASS_TYPE_LIGHTING,
				GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING,
				&m_DirectionalLightingStage.pDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			m_DirectionalLightingStage.pDescriptorSet->SetBuffer(0, m_DirectionalLightingStage.pUniformBuffer, 0, sizeof(DirectionalLightingUniform));
		}

		// ポイントライティングの初期化
		{
			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_LIGHTING, GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING, &m_PointLightingStage.pPipelineLayout);

			// グラフィックスパイプラインのハンドルを取得
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_ADDITION;
			if (m_GraphicsManager.GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_LIGHTING, GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING, desc, &m_PointLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

			// ユニフォームバッファを更新
			void* pMemory;
			if (m_PointLightingStage.pUniformBuffer->Map(0, sizeof(PointLightingUniform), &pMemory) == V3D_OK)
			{
				PointLightingUniform uniform{};
				uniform.pointLightCount = POINT_LIGHT_COUNT;

				const float baseX = -(TO_F32(POINT_LIGHT_X_COUNT - 1) * POINT_LIGHT_INTERVAL) / 2.0f + POINT_LIGHT_OX;
				const float baseY = -(TO_F32(POINT_LIGHT_Y_COUNT - 1) * POINT_LIGHT_INTERVAL) / 2.0f + POINT_LIGHT_OY;
				const float baseZ = -(TO_F32(POINT_LIGHT_Z_COUNT - 1) * POINT_LIGHT_INTERVAL) / 2.0f + POINT_LIGHT_OZ;

				uint32_t lightIndex = 0;

				for (int32_t x = 0; x < POINT_LIGHT_X_COUNT; x++)
				{
					float wx = baseX + POINT_LIGHT_INTERVAL * TO_F32(x);

					for (int32_t y = 0; y < POINT_LIGHT_Y_COUNT; y++)
					{
						float wy = baseY + POINT_LIGHT_INTERVAL * TO_F32(y);

						for (int32_t z = 0; z < POINT_LIGHT_Z_COUNT; z++)
						{
							float wz = baseZ + POINT_LIGHT_INTERVAL * TO_F32(z);

							PointLight& pointLight = uniform.pointLights[lightIndex];

							float r = 1.0f;//static_cast<float>(rand() % 100) / 100.0f;
							float g = 1.0f;//static_cast<float>(rand() % 100) / 100.0f;
							float b = 1.0f;//static_cast<float>(rand() % 100) / 100.0f;

							pointLight.color.Set(r, g, b, 1.0f);
							pointLight.posAndDist.Set(wx, wy, wz, FLOAT_RECIPROCAL(POINT_LIGHT_RANGE));

							lightIndex++;
						}
					}
				}

				memcpy_s(pMemory, sizeof(PointLightingUniform), &uniform, sizeof(PointLightingUniform));

				m_PointLightingStage.pUniformBuffer->Unmap();
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_RENDERPASS_TYPE_LIGHTING,
				GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING,
				&m_PointLightingStage.pDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			m_PointLightingStage.pDescriptorSet->SetBuffer(0, m_PointLightingStage.pUniformBuffer, 0, sizeof(PointLightingUniform));
		}

		// ----------------------------------------------------------------------------------------------------
		// シャドウプロジェクション
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_SHADOW_PROJECTION, &m_ShadowRenderPassHandle);

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_SHADOW_PROJECTION, GRAPHICS_PIPELINE_TYPE_SHADOW_PROJECTION, &m_pShadowPipelineLayout);
		}

		// ----------------------------------------------------------------------------------------------------
		// シャドウマッピング
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_SHADOW_MAPPING, &m_ShadowMappingRenderPassHandle);

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_SHADOW_MAPPING, GRAPHICS_PIPELINE_TYPE_SHADOW_MAPPING, &m_pShadowMappingPipelineLayout);

			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;
			if (m_GraphicsManager.GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_SHADOW_MAPPING, GRAPHICS_PIPELINE_TYPE_SHADOW_MAPPING, desc, &m_ShadowMappingPipelineHandle) != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_RENDERPASS_TYPE_SHADOW_MAPPING,
				GRAPHICS_PIPELINE_TYPE_SHADOW_MAPPING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_SHADOW_MAPPING,
				&m_pShadowMappingDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			;
		}

		// ----------------------------------------------------------------------------------------------------
		// フィニッシュ
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_FINISH, &m_FinishRenderPassHandle);

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_FINISH, GRAPHICS_PIPELINE_TYPE_FINISH, &m_FinishLightingStage.pPipelineLayout);

			// グラフィックスパイプラインのハンドルを取得
			GraphicsPipelienDesc desc;
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;
			if (m_GraphicsManager.GetPipelineHandle(GRAPHICS_RENDERPASS_TYPE_FINISH, GRAPHICS_PIPELINE_TYPE_FINISH,	desc, &m_FinishLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_RENDERPASS_TYPE_FINISH,
				GRAPHICS_PIPELINE_TYPE_FINISH,
				GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING,
				&m_FinishLightingStage.pDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			;
		}

		// ----------------------------------------------------------------------------------------------------
		// 並列データの初期化
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_CommandPools.reserve(threadCount);

		for (uint32_t i = 0; i < threadCount; i++)
		{
			V3DCommandPoolDesc commandPoolDesc{};
			commandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER;
			commandPoolDesc.queueFamily = GetGraphicsQueueFamily();

			IV3DCommandPool* pCommandPool;

			V3D_RESULT result = Application::GetDevice()->CreateCommandPool(commandPoolDesc, &pCommandPool);
			if (result != V3D_OK)
			{
				return false;
			}

			m_CommandPools.push_back(pCommandPool);
		}

		m_GeometryParallelData.pMeshes = &m_Meshes;
		m_ShadowParallelData.pMeshes = &m_Meshes;

		// ----------------------------------------------------------------------------------------------------
		// スワップチェインに関係するリソースの作成
		// ----------------------------------------------------------------------------------------------------

		if (OnRestoreSwapChain() == false)
		{
			return false;
		}

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		if (m_CommandPools.empty() == false)
		{
			for (size_t i = 0; i < m_CommandPools.size(); i++)
			{
				SAFE_RELEASE(m_CommandPools[i]);
			}
			m_CommandPools.clear();
		}

		SAFE_RELEASE(m_FinishLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_FinishLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_pShadowMappingDescriptorSet);
		SAFE_RELEASE(m_pShadowMappingPipelineLayout);

		SAFE_RELEASE(m_pShadowPipelineLayout);

		SAFE_RELEASE(m_PointLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_PointLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_PointLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_DirectionalLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_DirectionalLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_DirectionalLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_pGeometryPipelineLayout);

		SAFE_RELEASE(m_pSimpleSampler);
		SAFE_RELEASE(m_pSimpleVertexBuffer);

		m_Meshes.clear();

		m_Font.Dispose();

		m_MeshManager.Finalize();
		m_MaterialManager.Finalize();
		m_TextureManager.Finalize();
		m_GraphicsManager.Finalize();
		m_ParallelManager.Finalize();
	}

	virtual bool OnIdle() override
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		IV3DFrameBuffer* pFrameBuffer = m_FinishFrameBuffers[currentImageIndex];
		size_t meshCount = m_Meshes.size();

		// ----------------------------------------------------------------------------------------------------
		// テキストを更新
		// ----------------------------------------------------------------------------------------------------

		std::wstringstream text;

		m_Font.Reset();

		text << L"Fps " << Application::GetFps();
		m_Font.AddText(16, 16, text.str().c_str());

		// ----------------------------------------------------------------------------------------------------
		// シーンを更新
		// ----------------------------------------------------------------------------------------------------

		float aspectRatio = (swapChainDesc.windowed == false) ? (16.0f / 9.0f) : (float)swapChainDesc.imageWidth / (float)swapChainDesc.imageHeight;

		Vector3 eyePos = m_Camera.GetEye();
		Vector3 eyeDir = m_Camera.GetEye().ToNormalize();

		void * pMemory;

		// カメラ
		m_Camera.SetProjection(DEG_TO_RAD(45.0f), aspectRatio, 0.1f, 4000.0f);
		m_Camera.SetView(m_CameraRot, m_CameraAt, m_CameraDistance);
		m_Camera.Update();

		// ディレクショナルライト
		if (m_DirectionalLightingStage.pUniformBuffer->Map(0, V3D_WHOLE_SIZE, &pMemory) == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(Vector3), &eyeDir, sizeof(Vector3));

			m_DirectionalLightingStage.pUniformBuffer->Unmap();
		}

		// ポイントライト
		if (m_PointLightingStage.pUniformBuffer->Map(0, V3D_WHOLE_SIZE, &pMemory) == V3D_OK)
		{
			uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

			memcpy_s(pAddr, sizeof(Vector3), &eyePos, sizeof(Vector3));

			const Matrix4x4& invViewProjMat = m_Camera.GetInverseViewProjectionMatrix();
			memcpy_s(pAddr + sizeof(Vector4), sizeof(Matrix4x4), &invViewProjMat, sizeof(Matrix4x4));

			m_PointLightingStage.pUniformBuffer->Unmap();
		}

		// ジオメトリ - 並列データ
		m_GeometryParallelData.frame = currentImageIndex;
		m_GeometryParallelData.frustum.Update(m_Camera.GetViewProjectionMatrix());
		m_GeometryParallelData.pPipelineLayout = m_pGeometryPipelineLayout;
		m_GeometryParallelData.pRenderPass = m_GeometryRenderPassHandle->GetPtr();
		m_GeometryParallelData.pFrameBuffer = m_pGeometryFrameBuffer;

		// シャドウ - 並列データ
		m_ShadowParallelData.frame = currentImageIndex;
		m_ShadowParallelData.pPipelineLayout = m_pShadowPipelineLayout;
		m_ShadowParallelData.pRenderPass = m_ShadowRenderPassHandle->GetPtr();
		m_ShadowParallelData.pFrameBuffer = m_pShadowFrameBuffer;

		// ----------------------------------------------------------------------------------------------------
		// 描画前の処理
		// ----------------------------------------------------------------------------------------------------

		// フォントを焼く
		m_Font.Bake(GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence());

#ifdef ENABLE_MULTITHREAD

		if (meshCount == 1)
		{
			uint32_t meshNodeCount = m_Meshes[0]->GetMeshNodeCount();

			if (m_ParallelManager.GetThreadCount() <= meshNodeCount)
			{
				// メッシュの更新
				m_MeshManager.BeginUpdate();
				m_ParallelManager.Execute(ParallelUpdate, TO_UI32(m_Meshes.size()), &m_GeometryParallelData);
				m_MeshManager.EndUpdate();

				// ジオメトリ  ( サブパス 0 ) - メッシュの描画コマンドを記録
				m_ParallelManager.Execute(ParallelDrawGeometryNode, meshNodeCount, &m_GeometryParallelData);
			}
			else
			{
				SingleUpdate();
			}
		}
		else if (meshCount > 1)
		{
			// メッシュの更新
			m_MeshManager.BeginUpdate();
			m_ParallelManager.Execute(ParallelUpdate, TO_UI32(m_Meshes.size()), &m_GeometryParallelData);
			m_MeshManager.EndUpdate();

			// ジオメトリ ( サブパス 0 ) - メッシュの描画コマンドを記録
			m_ParallelManager.Execute(ParallelDrawGeometry, TO_UI32(m_Meshes.size()), &m_GeometryParallelData);
		}

#else //ENABLE_MULTITHREAD

		// メッシュの更新
		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = m_Meshes.end();
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		m_MeshManager.BeginUpdate();

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			(*it_mesh)->Update();
		}

		m_MeshManager.EndUpdate();

#endif //ENABLE_MULTITHREAD

		{
			const Matrix4x4& viewMat = m_Camera.GetViewMatrix();
			Vector3 eyeVec(viewMat.z.x, viewMat.z.y, viewMat.z.z);

			Vector3 upVec;
			upVec = Vector3::Cross(eyeVec, m_LightDir);
			upVec = Vector3::Cross(m_LightDir, upVec);

			AABB aabb = m_Meshes[0]->GetBounds().aabb;
			aabb.UpdateCenterAndPoints();

			Matrix4x4 lightViewMat = Matrix4x4::LookAt(aabb.center - m_LightDir, aabb.center, upVec);

			Vector3 aabbMin(+FLT_MAX, +FLT_MAX, +FLT_MAX);
			Vector3 aabbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (uint32_t i = 0; i < 8; i++)
			{
				Vector3 temp = lightViewMat * aabb.points[i];
				aabbMin = Vector3::Minimum(aabbMin, temp);
				aabbMax = Vector3::Maximum(aabbMax, temp);
			}

			float bias = 5.0f;
#if 0
			float width = aabbMax.x - aabbMin.x;
			float height = aabbMax.y - aabbMin.y;
			float nearClip = aabbMin.z;
			float farClip = aabbMax.z;
			Matrix4x4 lightProjMatrix = Matrix4x4::Ortho(width, height, nearClip, farClip);
#else
//			m_LightMatrix = Matrix4x4::Ortho(aabbMin, aabbMax);
			aabbMin.z -= bias;
			aabbMax.z += bias;
			Matrix4x4 lightProjMatrix  = Matrix4x4::OrthoOffCenter(aabbMin, aabbMax);
#endif

			m_ShadowParallelData.sceneConstant.viewProjMatrix = lightProjMatrix * lightViewMat;
		}

		if (meshCount == 1)
		{
			uint32_t meshNodeCount = m_Meshes[0]->GetMeshNodeCount();

			if (m_ParallelManager.GetThreadCount() <= meshNodeCount)
			{
				// シャドウ ( サブパス 0 ) - メッシュの描画コマンドを記録
				m_ParallelManager.Execute(ParallelDrawShadowNode, meshNodeCount, &m_ShadowParallelData);
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// 描画
		// ----------------------------------------------------------------------------------------------------

		IV3DCommandBuffer* pGraphicsCommandBuffer = BeginGraphics();
		if (pGraphicsCommandBuffer == nullptr)
		{
			return false;
		}

		IV3DImageView* pBackBuffer;
		pFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER, &pBackBuffer);

		// バックバッファをバリア
		V3DBarrierImageDesc barrier{};
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		barrier.srcAccessMask = V3D_ACCESS_MEMORY_READ;
		barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		barrier.srcQueueFamily = GetGraphicsQueueFamily();
		barrier.dstQueueFamily = GetGraphicsQueueFamily();
		barrier.srcLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pGraphicsCommandBuffer->BarrierImageView(pBackBuffer, barrier);

		pGraphicsCommandBuffer->SetViewport(0, 1, &m_GeometryParallelData.viewport);
		pGraphicsCommandBuffer->SetScissor(0, 1, &m_GeometryParallelData.scissor);

		// ジオメトリ
		DrawGeometry(pGraphicsCommandBuffer);

		// ライティング
		DrawLighting(pGraphicsCommandBuffer);

		// シャドウ
		DrawShadow(pGraphicsCommandBuffer);

		// フィニッシュ
		DrawFinish(pGraphicsCommandBuffer);

		SAFE_RELEASE(pBackBuffer);

		// ----------------------------------------------------------------------------------------------------
		// 描画終了
		// ----------------------------------------------------------------------------------------------------

		if (EndGraphics() == false)
		{
			return false;
		}

		return true;
	}

	virtual void OnLostSwapChain() override
	{
		if (m_GeometryParallelData.commandBuffer.empty() == false)
		{
			for (size_t i = 0; i < m_GeometryParallelData.commandBuffer.size(); i++)
			{
				for (size_t j = 0; j < m_GeometryParallelData.commandBuffer[i].size(); j++)
				{
					SAFE_RELEASE(m_GeometryParallelData.commandBuffer[i][j]);
				}

				m_GeometryParallelData.commandBuffer[i].clear();
			}

			m_GeometryParallelData.commandBuffer.clear();
		}

		if (m_ShadowParallelData.commandBuffer.empty() == false)
		{
			for (size_t i = 0; i < m_ShadowParallelData.commandBuffer.size(); i++)
			{
				for (size_t j = 0; j < m_ShadowParallelData.commandBuffer[i].size(); j++)
				{
					SAFE_RELEASE(m_ShadowParallelData.commandBuffer[i][j]);
				}

				m_ShadowParallelData.commandBuffer[i].clear();
			}

			m_ShadowParallelData.commandBuffer.clear();
		}

		SAFE_RELEASE(m_pGeometryFrameBuffer);
		SAFE_RELEASE(m_pLightingFrameBuffer);
		SAFE_RELEASE(m_pShadowFrameBuffer);
		SAFE_RELEASE(m_pShadowMappingFrameBuffer);

		if (m_FinishFrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_FinishFrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_FinishFrameBuffers[i]);
			}
			m_FinishFrameBuffers.clear();
		}

		m_GraphicsManager.Lost();
		m_Font.Lost();
	}

	virtual bool OnRestoreSwapChain() override
	{
		if (m_GraphicsManager.Restore() != V3D_OK)
		{
			return false;
		}

		if (m_Font.Restore(m_FinishRenderPassHandle->GetPtr(), 1) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// フレームバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		// ジオメトリ
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_GEOMETRY, 0, &m_pGeometryFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// ライティング
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_LIGHTING, 0, &m_pLightingFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// シャドウプロジェクション
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_SHADOW_PROJECTION, 0, &m_pShadowFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// シャドウマッピング
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_SHADOW_MAPPING, 0, &m_pShadowMappingFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// フィニッシュ
		m_FinishFrameBuffers.resize(swapChainDesc.imageCount);
		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_FINISH, i, &m_FinishFrameBuffers[i]) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを再設定
		// ----------------------------------------------------------------------------------------------------

		IV3DImageView* pColorImageView;
		IV3DImageView* pNormalImageView;
		IV3DImageView* pDataImageView;
		IV3DImageView* pLightImageView;
		IV3DImageView* pShadowProjImageView;
		IV3DImageView* pShadowMapImageView;

		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_COLOR, &pColorImageView);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0, &pNormalImageView);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1, &pDataImageView);

		m_pLightingFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR, &pLightImageView);

		m_pShadowFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_SHADOW_PROJECTION_BUFFER, &pShadowProjImageView);
		m_pShadowMappingFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_SHADOW_MAPPING_COLOR, &pShadowMapImageView);

		// ディレクショナルライティング
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->Update();

		// ポイントライティング
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->Update();

		// シャドウマッピング
		m_pShadowMappingDescriptorSet->SetImageViewAndSampler(0, pDataImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_pShadowMappingDescriptorSet->SetImageViewAndSampler(1, pShadowProjImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_pShadowMappingDescriptorSet->Update();

		// フィニッシュ
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(0, pColorImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pLightImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pShadowMapImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->Update();

		SAFE_RELEASE(pShadowMapImageView);
		SAFE_RELEASE(pShadowProjImageView);
		SAFE_RELEASE(pLightImageView);
		SAFE_RELEASE(pDataImageView);
		SAFE_RELEASE(pNormalImageView);
		SAFE_RELEASE(pColorImageView);

		// ----------------------------------------------------------------------------------------------------
		// 並列処理用のデータを作成
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_GeometryParallelData.commandBuffer.resize(swapChainDesc.imageCount);
		m_GeometryParallelData.pRenderPass = m_GeometryRenderPassHandle->GetPtr();
		m_GeometryParallelData.viewport.rect.x = 0;
		m_GeometryParallelData.viewport.rect.y = 0;
		m_GeometryParallelData.viewport.rect.width = swapChainDesc.imageWidth;
		m_GeometryParallelData.viewport.rect.height = swapChainDesc.imageHeight;
		m_GeometryParallelData.viewport.minDepth = 0.0f;
		m_GeometryParallelData.viewport.maxDepth = 1.0f;
		m_GeometryParallelData.scissor.x = 0;
		m_GeometryParallelData.scissor.y = 0;
		m_GeometryParallelData.scissor.width = swapChainDesc.imageWidth;
		m_GeometryParallelData.scissor.height = swapChainDesc.imageHeight;

		m_ShadowParallelData.commandBuffer.resize(swapChainDesc.imageCount);
		m_ShadowParallelData.pRenderPass = m_ShadowRenderPassHandle->GetPtr();
		m_ShadowParallelData.viewport.rect.x = 0;
		m_ShadowParallelData.viewport.rect.y = 0;
		m_ShadowParallelData.viewport.rect.width = 2048;
		m_ShadowParallelData.viewport.rect.height = 2048;
		m_ShadowParallelData.viewport.minDepth = 0.0f;
		m_ShadowParallelData.viewport.maxDepth = 1.0f;
		m_ShadowParallelData.scissor.x = 0;
		m_ShadowParallelData.scissor.y = 0;
		m_ShadowParallelData.scissor.width = 2048;
		m_ShadowParallelData.scissor.height = 2048;

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			m_GeometryParallelData.commandBuffer[i].reserve(threadCount);
			m_ShadowParallelData.commandBuffer[i].reserve(threadCount);

			for (uint32_t j = 0; j < threadCount; j++)
			{
				IV3DCommandBuffer* pCommandBuffer;

				V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(m_CommandPools[j], V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_GeometryParallelData.commandBuffer[i].push_back(pCommandBuffer);

				result = Application::GetDevice()->CreateCommandBuffer(m_CommandPools[j], V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_ShadowParallelData.commandBuffer[i].push_back(pCommandBuffer);
			}
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState)
	{
		if (keyCode == 'Z')
		{
			Quaternion rot;
			rot.SetRotationAxis(Vector3::XAxis(), DEG_TO_RAD(1.0f));
			m_LightDir = Matrix4x4(rot) * m_LightDir;
		}
		else if (keyCode == 'X')
		{
			Quaternion rot;
			rot.SetRotationAxis(Vector3::XAxis(), DEG_TO_RAD(-1.0f));
			m_LightDir = Matrix4x4(rot) * m_LightDir;
		}

		return true;
	}

	virtual bool OnMouseButtonDown(WPARAM keyFlags, const POINT& pos)
	{
		if (keyFlags & MK_LBUTTON)
		{
			if (m_bDrag == false)
			{
				m_bDrag = true;
				m_MousePos = pos;
			}
		}

		if (keyFlags & MK_RBUTTON)
		{
			SaveScreenshot(nullptr);
		}

		return true;
	}

	virtual bool OnMouseButtonUp(WPARAM keyFlags, const POINT& pos)
	{
		if (keyFlags & MK_LBUTTON)
		{
			m_bDrag = false;
		}

		return true;
	}

	virtual bool OnMouseMove(WPARAM keyFlags, const POINT& pos)
	{
		POINT vec;

		vec.x = pos.x - m_MousePos.x;
		vec.y = pos.y - m_MousePos.y;

		if (keyFlags & MK_LBUTTON)
		{
			if (keyFlags & MK_CONTROL)
			{
				m_CameraDistance += static_cast<float>(vec.y) * -0.05f * m_CameraMoveCoiffe;
				m_CameraDistance = MAXIMUM(1.0f, m_CameraDistance);
			}
			else if (keyFlags & MK_SHIFT)
			{
				Matrix4x4 mat = m_CameraRot;

				Vector3 dirX(1.0f, 0.0f, 0.0f);
				Vector3 dirY(0.0f, 1.0f, 0.0f);

				dirX = mat * dirX;
				dirY = mat * dirY;

				m_CameraAt += dirX * static_cast<float>(vec.x) * -0.015f * m_CameraMoveCoiffe;
				m_CameraAt += dirY * static_cast<float>(vec.y) * -0.015f * m_CameraMoveCoiffe;
			}
			else
			{
				Quaternion rot;

				rot.SetRotationAxis(Vector3::YAxis(), DEG_TO_RAD(static_cast<float>(vec.x) * -0.25f * m_CameraRotCoiffe));
				rot.RotationAxis(Vector3::XAxis(), DEG_TO_RAD(static_cast<float>(vec.y) * 0.25f * m_CameraRotCoiffe));

				m_CameraRot = rot * m_CameraRot;
			}
		}

		m_MousePos = pos;

		return true;
	}

	virtual bool OnMouseWheel(WPARAM keyFlags, int16_t delta, const POINT& pos)
	{
		if (delta > 0)
		{
			m_CameraDistance -= 0.5f * m_CameraMoveCoiffe;
		}
		else if (delta < 0)
		{
			m_CameraDistance += 0.5f * m_CameraMoveCoiffe;
		}

		return true;
	}

	void DrawGeometry(IV3DCommandBuffer* pCommandBuffer)
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		size_t meshCount = m_Meshes.size();

		SceneConstant sceneConstant;
		sceneConstant.viewProjMatrix = m_Camera.GetViewProjectionMatrix();
		pCommandBuffer->PushConstant(m_pGeometryPipelineLayout, 0, &sceneConstant);

#ifdef ENABLE_MULTITHREAD

		if (meshCount == 1)
		{
			uint32_t meshNodeCount = m_Meshes[0]->GetMeshNodeCount();

			if (m_ParallelManager.GetThreadCount() <= meshNodeCount)
			{
				// レンダーパスを開始
				pCommandBuffer->BeginRenderPass(m_GeometryRenderPassHandle->GetPtr(), m_pGeometryFrameBuffer, false);

				// サブパス 0 : ジオメトリバッファ
				pCommandBuffer->ExecuteCommandBuffers(m_ParallelManager.GetThreadCount(), m_GeometryParallelData.commandBuffer[m_GeometryParallelData.frame].data());
			}
			else
			{
				// レンダーパスを開始
				pCommandBuffer->BeginRenderPass(m_GeometryRenderPassHandle->GetPtr(), m_pGeometryFrameBuffer, true);

				SingleDraw(pCommandBuffer);
			}
		}
		else if (meshCount > 1)
		{
			// レンダーパスを開始
			pCommandBuffer->BeginRenderPass(m_GeometryRenderPassHandle->GetPtr(), m_pGeometryFrameBuffer, false);

			// サブパス 0 : ジオメトリバッファ
			pCommandBuffer->ExecuteCommandBuffers(m_ParallelManager.GetThreadCount(), m_GeometryParallelData.commandBuffer[m_GeometryParallelData.frame].data());
		}

#else //ENABLE_MULTITHREAD

		// レンダーパスを開始
		pCommandBuffer->BeginRenderPass(m_GeometryRenderPassHandle->GetPtr(), m_pGeometryFrameBuffer, true);

		// サブパス 0 : ジオメトリバッファ
		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = m_Meshes.end();
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		uint64_t prevMeshKey = 0;
		uint64_t prevMaterialKey = 0;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			SkeletalMesh* pMesh = (*it_mesh).get();

			if (m_GeometryParallelData.frustum.Contains(pMesh->GetBounds().sphere) == false)
			{
				continue;
			}

			uint32_t meshNodeCount = pMesh->GetMeshNodeCount();
			uint64_t meshKey = pMesh->GetKey();

			for (uint32_t i = 0; i < meshNodeCount; i++)
			{
				SkeletalMeshNodePtr meshNode = pMesh->GetMeshNode(i);

				if (m_GeometryParallelData.frustum.Contains(meshNode->GetBounds().aabb) == false)
				{
					continue;
				}

				if (prevMeshKey != meshKey)
				{
					pMesh->BindVertexIndexBuffer(pCommandBuffer);
					prevMeshKey = meshKey;
				}

				meshNode->BindDescriptorSet(pCommandBuffer);

				uint32_t subsetCount = meshNode->GetSubsetCount();
				for (uint32_t j = 0; j < subsetCount; j++)
				{
					const MeshSubset& subset = meshNode->GetSubset(j);

					MaterialPtr material = pMesh->GetMaterial(subset.material);

					uint64_t materialKey = material->GetKey();
					if (prevMaterialKey != materialKey)
					{
						material->Bind(pCommandBuffer);
						prevMaterialKey = materialKey;
					}

					pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
				}
			}
			/*
			StaticMesh* pMesh = (*it_mesh).get();
			uint64_t meshKey = pMesh->GetKey();

			if (meshKey != prevMeshKey)
			{
			pMesh->BindVertexIndexBuffer(pGraphicsCommandBuffer);
			prevMeshKey = meshKey;
			}

			pMesh->BindDescriptorSet(pGraphicsCommandBuffer);

			uint32_t subsetCount = pMesh->GetSubsetCount();
			for (uint32_t i = 0; i < subsetCount; i++)
			{
			const MeshSubset& subset = pMesh->GetSubset(i);

			MaterialPtr material = pMesh->GetMaterial(subset.material);
			uint64_t materialKey = material->GetKey();

			if (materialKey != prevMaterialKey)
			{
			material->Bind(pGraphicsCommandBuffer);
			prevMaterialKey = materialKey;
			}

			pGraphicsCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
			*/
		}
#endif //ENABLE_MULTITHREAD

		// レンダーパスを終了
		pCommandBuffer->EndRenderPass();
	}

	void DrawLighting(IV3DCommandBuffer* pCommandBuffer)
	{
		uint64_t simpleVertexOffset = 0;

		pCommandBuffer->SetViewport(0, 1, &m_GeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_GeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(m_LightingRenderPassHandle->GetPtr(), m_pLightingFrameBuffer, true);

		// サブパス 0 : ディレクショナルライティング
		pCommandBuffer->BindPipeline(m_DirectionalLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_DirectionalLightingStage.pPipelineLayout, 0, 1, &m_DirectionalLightingStage.pDescriptorSet, 0, nullptr);
		pCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 1 : ポイントライティング
		pCommandBuffer->NextSubpass();
		pCommandBuffer->BindPipeline(m_PointLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_PointLightingStage.pPipelineLayout, 0, 1, &m_PointLightingStage.pDescriptorSet, 0, nullptr);
		pCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pCommandBuffer->Draw(4, 1, 0, 0);

		pCommandBuffer->EndRenderPass();
	}

	void DrawShadow(IV3DCommandBuffer* pCommandBuffer)
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		uint64_t simpleVertexOffset = 0;

		// プロジェクション

#if 1
		pCommandBuffer->BeginRenderPass(m_ShadowRenderPassHandle->GetPtr(), m_pShadowFrameBuffer, false);

		pCommandBuffer->ExecuteCommandBuffers(TO_UI32(m_ShadowParallelData.commandBuffer[currentImageIndex].size()), m_ShadowParallelData.commandBuffer[currentImageIndex].data());

#else
		pCommandBuffer->BeginRenderPass(m_ShadowRenderPassHandle->GetPtr(), m_pShadowFrameBuffer, true);

		pCommandBuffer->SetViewport(0, 1, &m_ShadowParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_ShadowParallelData.scissor);

		pCommandBuffer->PushConstant(m_pShadowPipelineLayout, 0, &m_ShadowParallelData.sceneConstant);

		SkeletalMeshPtr mesh = (*m_ShadowParallelData.pMeshes)[0];

		mesh->BindVertexIndexBuffer(pCommandBuffer);

		uint64_t prevMaterialKey = 0;

		for (uint32_t i = 0; i < mesh->GetMeshNodeCount(); i++)
		{
			SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);

			meshNode->BindDescriptorSet(pCommandBuffer);

			uint32_t subsetCount = meshNode->GetSubsetCount();
			for (uint32_t j = 0; j < subsetCount; j++)
			{
				const MeshSubset& subset = meshNode->GetSubset(j);

				MaterialPtr material = mesh->GetMaterial(subset.material);

				uint64_t materialKey = material->GetKey();
				if (prevMaterialKey != materialKey)
				{
					material->BindShadow(pCommandBuffer);
					prevMaterialKey = materialKey;
				}

				pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
		}
#endif
		pCommandBuffer->EndRenderPass();

		Matrix4x4 biasMatrix;
		biasMatrix.x.x = 0.5f;
		biasMatrix.y.y = 0.5f;
		biasMatrix.z.z = 0.5f;
		biasMatrix.w.x = 0.5f;
		biasMatrix.w.y = 0.5f;
		biasMatrix.w.z = 0.5f;

		// マッピング
		ShadowMappingConstant mappingConstant;
		mappingConstant.invViewProjMatrix = m_Camera.GetInverseViewProjectionMatrix();
//		mappingConstant.lightMatrix = biasMatrix;
		mappingConstant.lightMatrix *= m_ShadowParallelData.sceneConstant.viewProjMatrix;
		pCommandBuffer->PushConstant(m_pShadowMappingPipelineLayout, 0, &mappingConstant);

		pCommandBuffer->BeginRenderPass(m_ShadowMappingRenderPassHandle->GetPtr(), m_pShadowMappingFrameBuffer, true);

		pCommandBuffer->BindPipeline(m_ShadowMappingPipelineHandle->GetPtr());

		pCommandBuffer->SetViewport(0, 1, &m_GeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_GeometryParallelData.scissor);

		pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_pShadowMappingPipelineLayout, 0, 1, &m_pShadowMappingDescriptorSet, 0, nullptr);
		pCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pCommandBuffer->Draw(4, 1, 0, 0);

		pCommandBuffer->EndRenderPass();
	}

	void DrawFinish(IV3DCommandBuffer* pCommandBuffer)
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		IV3DFrameBuffer* pFrameBuffer = m_FinishFrameBuffers[currentImageIndex];
		uint64_t simpleVertexOffset = 0;

		pCommandBuffer->SetViewport(0, 1, &m_GeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_GeometryParallelData.scissor);

		// レンダーパスを開始
		pCommandBuffer->BeginRenderPass(m_FinishRenderPassHandle->GetPtr(), pFrameBuffer, true);

		// サブパス 0 : フィニッシュ
		pCommandBuffer->BindPipeline(m_FinishLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_FinishLightingStage.pPipelineLayout, 0, 1, &m_FinishLightingStage.pDescriptorSet, 0, nullptr);
		pCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 1 : テキスト
		pCommandBuffer->NextSubpass();
		m_Font.Flush(pCommandBuffer, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

		pCommandBuffer->EndRenderPass();
	}

	void SingleUpdate()
	{
		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = m_Meshes.end();
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		m_MeshManager.BeginUpdate();

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			(*it_mesh)->Update();
		}

		m_MeshManager.EndUpdate();
	}

	void SingleDraw(IV3DCommandBuffer* pCommandBuffer)
	{
		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = m_Meshes.end();
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		// サブパス 0 : ジオメトリバッファ
		uint64_t prevMeshKey = 0;
		uint64_t prevMaterialKey = 0;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			SkeletalMesh* pMesh = (*it_mesh).get();

			if (m_GeometryParallelData.frustum.Contains(pMesh->GetBounds().sphere) == false)
			{
				continue;
			}

			uint32_t meshNodeCount = pMesh->GetMeshNodeCount();
			uint64_t meshKey = pMesh->GetKey();

			for (uint32_t i = 0; i < meshNodeCount; i++)
			{
				SkeletalMeshNodePtr meshNode = pMesh->GetMeshNode(i);

				if (m_GeometryParallelData.frustum.Contains(meshNode->GetBounds().aabb) == false)
				{
					continue;
				}

				if (prevMeshKey != meshKey)
				{
					pMesh->BindVertexIndexBuffer(pCommandBuffer);
					prevMeshKey = meshKey;
				}

				meshNode->BindDescriptorSet(pCommandBuffer);

				uint32_t subsetCount = meshNode->GetSubsetCount();
				for (uint32_t j = 0; j < subsetCount; j++)
				{
					const MeshSubset& subset = meshNode->GetSubset(j);

					MaterialPtr material = pMesh->GetMaterial(subset.material);

					uint64_t materialKey = material->GetKey();
					if (prevMaterialKey != materialKey)
					{
						material->Bind(pCommandBuffer);
						prevMaterialKey = materialKey;
					}

					pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
				}
			}
			/*
			StaticMesh* pMesh = (*it_mesh).get();
			uint64_t meshKey = pMesh->GetKey();

			if (meshKey != prevMeshKey)
			{
			pMesh->BindVertexIndexBuffer(pGraphicsCommandBuffer);
			prevMeshKey = meshKey;
			}

			pMesh->BindDescriptorSet(pGraphicsCommandBuffer);

			uint32_t subsetCount = pMesh->GetSubsetCount();
			for (uint32_t i = 0; i < subsetCount; i++)
			{
			const MeshSubset& subset = pMesh->GetSubset(i);

			MaterialPtr material = pMesh->GetMaterial(subset.material);
			uint64_t materialKey = material->GetKey();

			if (materialKey != prevMaterialKey)
			{
			material->Bind(pGraphicsCommandBuffer);
			prevMaterialKey = materialKey;
			}

			pGraphicsCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
			*/
		}
	}

#ifdef ENABLE_MULTITHREAD

	static void __stdcall ParallelUpdate(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		GeometryParallelData* pParallel = static_cast<GeometryParallelData*>(pData);

		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = pParallel->pMeshes->begin() + first;
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = it_mesh_begin + count;
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			(*it_mesh)->Update();
		}
	}
		
	static void __stdcall ParallelDrawGeometry(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		GeometryParallelData* pParallel = static_cast<GeometryParallelData*>(pData);
		IV3DCommandBuffer* pCommandBuffer = pParallel->commandBuffer[pParallel->frame][thread];

		std::vector<SkeletalMeshPtr>::iterator it_mesh_begin = pParallel->pMeshes->begin() + first;
		std::vector<SkeletalMeshPtr>::iterator it_mesh_end = it_mesh_begin + count;
		std::vector<SkeletalMeshPtr>::iterator it_mesh;

		uint64_t prevMeshKey = 0;
		uint64_t prevMaterialKey = 0;

		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallel->pRenderPass, 0, pParallel->pFrameBuffer);

		pCommandBuffer->SetViewport(0, 1, &pParallel->viewport);
		pCommandBuffer->SetScissor(0, 1, &pParallel->scissor);

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			SkeletalMeshPtr mesh = (*it_mesh);

			if (pParallel->frustum.Contains(mesh->GetBounds().sphere) == false)
			{
				continue;
			}

			uint32_t meshNodeCount = mesh->GetMeshNodeCount();
			uint64_t meshKey = mesh->GetKey();

			for (uint32_t i = 0; i < meshNodeCount; i++)
			{
				SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);

				if (pParallel->frustum.Contains(meshNode->GetBounds().aabb) == false)
				{
					continue;
				}

				if (prevMeshKey != meshKey)
				{
					mesh->BindVertexIndexBuffer(pCommandBuffer);
					prevMeshKey = meshKey;
				}

				meshNode->BindDescriptorSet(pCommandBuffer);

				uint32_t subsetCount = meshNode->GetSubsetCount();
				for (uint32_t j = 0; j < subsetCount; j++)
				{
					const MeshSubset& subset = meshNode->GetSubset(j);

					MaterialPtr material = mesh->GetMaterial(subset.material);

					uint64_t materialKey = material->GetKey();
					if (prevMaterialKey != materialKey)
					{
						material->Bind(pCommandBuffer);
						prevMaterialKey = materialKey;
					}

					pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
				}
			}

//			pMesh->Draw(pCommandBuffer);

/*			StaticMesh* pMesh = (*it_mesh).get();
			uint64_t meshKey = pMesh->GetKey();

			if (meshKey != prevMeshKey)
			{
				pMesh->BindVertexIndexBuffer(pCommandBuffer);
				prevMeshKey = meshKey;
			}

			pMesh->BindDescriptorSet(pCommandBuffer);

			uint32_t subsetCount = pMesh->GetSubsetCount();
			for (uint32_t i = 0; i < subsetCount; i++)
			{
				const MeshSubset& subset = pMesh->GetSubset(i);

				MaterialPtr material = pMesh->GetMaterial(subset.material);
				uint64_t materialKey = material->GetKey();

				if (materialKey != prevMaterialKey)
				{
					material->Bind(pCommandBuffer);
					pCommandBuffer->SetViewport(0, 1, &pParallel->viewport);
					pCommandBuffer->SetScissor(0, 1, &pParallel->scissor);
					prevMaterialKey = materialKey;
				}

				pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
*/
		}

		pCommandBuffer->End();
	}

	static void __stdcall ParallelDrawGeometryNode(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		GeometryParallelData* pParallel = static_cast<GeometryParallelData*>(pData);
		IV3DCommandBuffer* pCommandBuffer = pParallel->commandBuffer[pParallel->frame][thread];

		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallel->pRenderPass, 0, pParallel->pFrameBuffer);

		pCommandBuffer->SetViewport(0, 1, &pParallel->viewport);
		pCommandBuffer->SetScissor(0, 1, &pParallel->scissor);

		SkeletalMeshPtr mesh = (*pParallel->pMeshes)[0];

		mesh->BindVertexIndexBuffer(pCommandBuffer);

		uint64_t prevMaterialKey = 0;

		for (uint32_t i = first; i < (first + count); i++)
		{
			SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);

			if (pParallel->frustum.Contains(meshNode->GetBounds().aabb) == false)
			{
				continue;
			}

			meshNode->BindDescriptorSet(pCommandBuffer);

			uint32_t subsetCount = meshNode->GetSubsetCount();
			for (uint32_t j = 0; j < subsetCount; j++)
			{
				const MeshSubset& subset = meshNode->GetSubset(j);

				MaterialPtr material = mesh->GetMaterial(subset.material);

				uint64_t materialKey = material->GetKey();
				if (prevMaterialKey != materialKey)
				{
					material->Bind(pCommandBuffer);
					prevMaterialKey = materialKey;
				}

				pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
		}

		pCommandBuffer->End();
	}

	static void __stdcall ParallelDrawShadowNode(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		ShadowParallelData* pParallel = static_cast<ShadowParallelData*>(pData);
		IV3DCommandBuffer* pCommandBuffer = pParallel->commandBuffer[pParallel->frame][thread];

		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallel->pRenderPass, 0, pParallel->pFrameBuffer);

		pCommandBuffer->SetViewport(0, 1, &pParallel->viewport);
		pCommandBuffer->SetScissor(0, 1, &pParallel->scissor);

		pCommandBuffer->PushConstant(pParallel->pPipelineLayout, 0, &pParallel->sceneConstant);

		SkeletalMeshPtr mesh = (*pParallel->pMeshes)[0];

		mesh->BindVertexIndexBuffer(pCommandBuffer);

		uint64_t prevMaterialKey = 0;

		for (uint32_t i = first; i < (first + count); i++)
		{
			SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);

			meshNode->BindDescriptorSet(pCommandBuffer);

			uint32_t subsetCount = meshNode->GetSubsetCount();
			for (uint32_t j = 0; j < subsetCount; j++)
			{
				const MeshSubset& subset = meshNode->GetSubset(j);

				MaterialPtr material = mesh->GetMaterial(subset.material);

				uint64_t materialKey = material->GetKey();
				if (prevMaterialKey != materialKey)
				{
					material->BindShadow(pCommandBuffer);
					prevMaterialKey = materialKey;
				}

				pCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
		}

		pCommandBuffer->End();
	}

#endif //ENABLE_MULTITHREAD

private:
	struct GeometryParallelData
	{
		uint32_t frame;
		std::vector<std::vector<IV3DCommandBuffer*>> commandBuffer;

		IV3DPipelineLayout* pPipelineLayout;
		IV3DRenderPass* pRenderPass;
		IV3DFrameBuffer* pFrameBuffer;

		V3DViewport viewport;
		V3DRectangle2D scissor;

		Frustum frustum;
		std::vector<SkeletalMeshPtr>* pMeshes;
	};

	struct ShadowParallelData
	{
		uint32_t frame;
		std::vector<std::vector<IV3DCommandBuffer*>> commandBuffer;

		IV3DPipelineLayout* pPipelineLayout;
		IV3DRenderPass* pRenderPass;
		IV3DFrameBuffer* pFrameBuffer;

		V3DViewport viewport;
		V3DRectangle2D scissor;

		SceneConstant sceneConstant;

		std::vector<SkeletalMeshPtr>* pMeshes;
	};

	struct LightingStage
	{
		IV3DPipelineLayout* pPipelineLayout;
		GraphicsPipelineHandle pipelineHandle;
		IV3DBuffer* pUniformBuffer;
		IV3DDescriptorSet* pDescriptorSet;
	};

	ParallelManager m_ParallelManager;
	GraphicsManager m_GraphicsManager;
	TextureManager m_TextureManager;
	MaterialManager m_MaterialManager;
	MeshManager m_MeshManager;

	Font m_Font;

	IV3DBuffer* m_pSimpleVertexBuffer;
	IV3DSampler* m_pSimpleSampler;

	std::vector<SkeletalMeshPtr> m_Meshes;

	std::vector<IV3DCommandPool*> m_CommandPools;

	GraphicsRenderPassHandle m_GeometryRenderPassHandle;
	IV3DPipelineLayout* m_pGeometryPipelineLayout;
	IV3DFrameBuffer* m_pGeometryFrameBuffer;
	GeometryParallelData m_GeometryParallelData;

	GraphicsRenderPassHandle m_LightingRenderPassHandle;
	IV3DFrameBuffer* m_pLightingFrameBuffer;
	LightingStage m_DirectionalLightingStage;
	LightingStage m_PointLightingStage;

	GraphicsRenderPassHandle m_ShadowRenderPassHandle;
	IV3DPipelineLayout* m_pShadowPipelineLayout;
	IV3DFrameBuffer* m_pShadowFrameBuffer;
	ShadowParallelData m_ShadowParallelData;

	GraphicsRenderPassHandle m_ShadowMappingRenderPassHandle;
	IV3DPipelineLayout* m_pShadowMappingPipelineLayout;
	GraphicsPipelineHandle m_ShadowMappingPipelineHandle;
	IV3DFrameBuffer* m_pShadowMappingFrameBuffer;
	IV3DDescriptorSet* m_pShadowMappingDescriptorSet;

	GraphicsRenderPassHandle m_FinishRenderPassHandle;
	std::vector<IV3DFrameBuffer*> m_FinishFrameBuffers;
	LightingStage m_FinishLightingStage;

	Quaternion m_CameraRot;
	float m_CameraDistance;
	Vector3 m_CameraAt;
	Camera m_Camera;

	bool m_bDrag;
	POINT m_MousePos;

	float m_CameraMoveCoiffe = 10.0f;
	float m_CameraRotCoiffe = 1.0f;

	Vector3 m_LightDir;
};

class TestApp : public Application
{
public:
	TestApp() {}
	virtual ~TestApp() {}

	virtual void OnPreCreate(ApplicationDesc& desc) override
	{
		desc.fps = 0;
	}

	virtual bool OnInitialize() override
	{
		::srand((unsigned)time(nullptr));

		uint32_t queueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 2);
		if (queueFamily == ~0U)
		{
			return false;
		}

		IV3DQueue* pWorkQueue;
		IV3DQueue* pGraphicsQueue;

		Application::GetDevice()->GetQueue(queueFamily, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(queueFamily, 1, &pGraphicsQueue);

		if (m_Window.Initialize(L"test", SCREEN_WIDTH, SCREEN_HEIGHT, false, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);
/*
		// ----------------------------------------------------------------------------------------------------

		Application::GetDevice()->GetQueue(0, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(0, 1, &pGraphicsQueue);

		if (m_Window2.Initialize(L"test2", SCREEN_WIDTH, SCREEN_HEIGHT, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);

		// ----------------------------------------------------------------------------------------------------

		Application::GetDevice()->GetQueue(0, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(0, 1, &pGraphicsQueue);

		if (m_Window3.Initialize(L"test3", SCREEN_WIDTH, SCREEN_HEIGHT, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);
*/
		return true;
	}

	virtual void OnIdle() override {}

	virtual void OnFinalize() override {}

private:
	TestWindow m_Window;
//	TestWindow m_Window2;
//	TestWindow m_Window3;
};

// ----------------------------------------------------------------------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	TestApp app;
	return app.Execute();
}
