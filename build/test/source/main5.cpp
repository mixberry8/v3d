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
#include "SkyDome.h"

// ----------------------------------------------------------------------------------------------------

static auto constexpr SCREEN_WIDTH = 1024;// 1600;
static auto constexpr SCREEN_HEIGHT = 768;// 900;
static auto constexpr BUFFER_COUNT = 2;

static auto constexpr MESH_X_COUNT = 1;
static auto constexpr MESH_Y_COUNT = 1;
static auto constexpr MESH_Z_COUNT = 1;
static constexpr float MESH_INTERVAL = 5.0f;

static auto constexpr POINT_LIGHT_X_COUNT = 1;
static auto constexpr POINT_LIGHT_Y_COUNT = 1;// 3;
static auto constexpr POINT_LIGHT_Z_COUNT = 1;// 1;
static constexpr float POINT_LIGHT_OX = 0.0f;
static constexpr float POINT_LIGHT_OY = 0.0f;
static constexpr float POINT_LIGHT_OZ = 0.0f;
static constexpr float POINT_LIGHT_INTERVAL = 600.0f;
static constexpr float POINT_LIGHT_RANGE = 100;// 600.0f;

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

//#define DEBUG_MARKER_ENABLE

#if defined(DEBUG_MARKER_ENABLE)
#define DEBUG_MARKER_CODE(code) code
#else
#define DEBUG_MARKER_CODE(code)
#endif

struct DrawObject
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
	V3D_INDEX_TYPE meshIndexType;
	MeshSubset meshSubset;
};

// ----------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------

class TestWindow : public Window
{
public:
	TestWindow()
	{
		m_CameraRot.SetRotationAxis(Vector3::XAxis(), DEG_TO_RAD(4.0f));
		m_CameraRot.RotationAxis(Vector3::YAxis(), DEG_TO_RAD(15.0f));
		m_CameraDistance = 24.0f;

//		m_CameraRot.SetRotationAxis(Vector3::XAxis(), DEG_TO_RAD(4.0f));
//		m_CameraRot.RotationAxis(Vector3::YAxis(), DEG_TO_RAD(90.0f));
//		m_CameraDistance = 19.0f;
//		m_CameraAt.Set(0.0f, 14.0f, 0.0f);

		m_DirectionalLightUniform.shadowParam.z = 0.0f;
		m_DirectionalLightUniform.shadowParam.w = 0.8f;

		m_UpdateParallelData.shadowEnable = true;

		m_CorrectionConstant.invGamma = 1.0f;// 0.65f;
		m_CorrectionConstant.contrast = 1.0f;// 1.2f;
//		m_CorrectionConstant.invGamma = 1.0f;
//		m_CorrectionConstant.contrast = 1.0f;
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

		if (m_Font.Initialize(Application::GetDevice(), GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(), L"Arial", 16, FW_NORMAL) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// クエリプールを作成
		// ----------------------------------------------------------------------------------------------------

		{
			V3DQueryPoolDesc desc;
			desc.queryType = V3D_QUERY_TYPE_PIPELINE_STATISTICS;
			desc.queryCount = 1;

			V3D_RESULT result = Application::GetDevice()->CreateQueryPool(desc, &m_pQueryPool, L"StatisticsQuery");
			if (result != V3D_OK)
			{
				return false;
			}
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
			if (m_pSimpleVertexBuffer->Map(0, 0, &pMemory) == V3D_OK)
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
		// サンプラーを作成
		// ----------------------------------------------------------------------------------------------------

		{
			const V3DDeviceCaps& deviceCaps = Application::GetDevice()->GetCaps();

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
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 0.0f;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			if (Application::GetDevice()->CreateSampler(samplerDesc, &m_pSimpleSampler, L"SimpleSampling") != V3D_OK)
			{
				return false;
			}

			samplerDesc.magFilter = V3D_FILTER_LINEAR;
			samplerDesc.minFilter = V3D_FILTER_LINEAR;
			samplerDesc.mipmapMode = V3D_MIPMAP_MODE_NEAREST;
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = (deviceCaps.flags & V3D_CAP_SAMPLER_ANISOTROPY) == V3D_CAP_SAMPLER_ANISOTROPY;
			samplerDesc.maxAnisotropy = (samplerDesc.anisotropyEnable == true) ? deviceCaps.maxSamplerAnisotropy : 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 0.0f;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			if (Application::GetDevice()->CreateSampler(samplerDesc, &m_pDownSampler, L"DownSampling") != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// メッシュを作成
		// ----------------------------------------------------------------------------------------------------

		{
			bool serialize = false;
//			std::wstring meshName = L"mesh\\house";
//			std::wstring meshName = L"mesh\\Room";
//			std::wstring meshName = L"mesh\\shadow";
			std::wstring meshName = L"mesh\\sibenik";
//			std::wstring meshName = L"mesh\\sponza";
//			std::wstring meshName = L"mesh\\dabrovic-sponza";
//			std::wstring meshName = L"mesh\\sanMiguel";

			SkeletalMeshPtr mesh;

			if (serialize == true)
			{
				float scale = 1.0f;
				uint32_t flags = MESH_IMPORT_INVERT_TEX_V | MESH_IMPORT_FLIP_FACE/* | MESH_IMPORT_INVERT_NORMAL*/ | MESH_IMPORT_SMOOSING;// | MESH_IMPORT_NO_MASK_TEXTURE | MESH_IMPORT_NO_OPTIMIZE;// | MESH_IMPORT_NO_MASK_TEXTURE;// | ;// | MESH_IMPORT_NO_TEXTURE;
//				float scale = 0.02f;
//				uint32_t flags = MESH_IMPORT_INVERT_NORMAL | MESH_IMPORT_SMOOSING;

				Matrix4x4 scaleMat;
				scaleMat.SetScale(Vector3(scale, scale, scale));

				MeshImportDesc meshImportDesc{};
				meshImportDesc.flags = flags;
				meshImportDesc.matrix = Matrix4x4(Quaternion(Vector3::ZAxis(), DEG_TO_RAD(180.0f))) * scaleMat;
				meshImportDesc.smoosingAngle = DEG_TO_RAD(80.0f);

				std::wstring objMeshName = meshName;
				objMeshName.append(L".obj");

				mesh = m_MeshManager.ImportSkeletal(objMeshName.c_str(), meshImportDesc);

				for (uint32_t i = 0; i < mesh->GetMaterialCount(); i++)
				{
					mesh->GetMaterial(i)->SetCullMode(V3D_CULL_MODE_NONE);
					mesh->GetMaterial(i)->Update();
				}

				std::wstring mdlMeshName = meshName;
				mdlMeshName.append(L".skm");

				std::wstring mdlFilePath;

				CreateFilePath(mdlMeshName.c_str(), mdlFilePath);
				mesh->Save(mdlFilePath.c_str());
			}
			else
			{
				std::wstring mdlMeshName = meshName;
				mdlMeshName.append(L".skm");

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
				mesh->Update();
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
							m_Meshes[meshIndex]->Update();
							meshIndex++;
						}
					}
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// ブライトパス
		// ----------------------------------------------------------------------------------------------------

		{
		// レンダーパスのハンドルを取得
		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS, &m_BrightPassRenderPassHandle);

		GraphicsPipelienDesc desc{};
		desc.polygonMode = V3D_POLYGON_MODE_FILL;
		desc.cullMode = V3D_CULL_MODE_NONE;
		desc.depthTestEnable = false;
		desc.depthWriteEnable = false;
		desc.blendMode = BLEND_MODE_COPY;

		// パイプラインのハンドルを取得
		if (m_GraphicsManager.GetPipelineHandle(
			GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS, 0,
			GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS, desc,
			&m_BrightPassPipelineHandle) != V3D_OK)
		{
			return false;
		}

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS, &m_pBrightPassPipelineLayout);

		// デスクリプタセットを作成
		if (m_GraphicsManager.CreateDescriptorSet(
			GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS,
			GRAPHICS_DESCRIPTOR_SET_TYPE_BRIGHT_PASS,
			&m_pBrightPassDescriptorSet) != V3D_OK)
		{
			return false;
		}
		}

		// ----------------------------------------------------------------------------------------------------
		// ブラー
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパス
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_BLUR, &m_BlurRenderPassHandle);

			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			/**********************/
			/* ダウンサンプリング */
			/**********************/

			// パイプラインレイアウト
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING, &m_pBlurDawnSamplingPipelineLayout);

			// パイプライン
			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_BLUR, 0,
				GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING, desc,
				&m_BlurDawnSamplingPipelineHandle) != V3D_OK)
			{
				return false;
			}

			/**************/
			/* ガウシアン */
			/**************/

			// パイプラインレイアウト
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN, &m_pBlurGaussianPipelineLayout);

			for (size_t i = 0; i < m_BlurGaussianPipelineHandle.size(); i++)
			{
				// パイプライン
				if (m_GraphicsManager.GetPipelineHandle(
					GRAPHICS_RENDERPASS_TYPE_BLUR, 1 + TO_UI32(i),
					GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN, desc,
					&m_BlurGaussianPipelineHandle[i]) != V3D_OK)
				{
					return false;
				}

				for (size_t j = 0; j < m_pBlurGaussianDescriptorSet[i].size(); j++)
				{
					// デスクリプタセット
					if (m_GraphicsManager.CreateDescriptorSet(
						GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN,
						GRAPHICS_DESCRIPTOR_SET_TYPE_BLUR_GAUSSIAN,
						&m_pBlurGaussianDescriptorSet[i][j]) != V3D_OK)
					{
						return false;
					}
				}
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// 合成
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパス
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_COMPOSITE, &m_CompositeRenderPassHandle);

			/********/
			/* ２枚 */
			/********/

			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_SCREEN;

			// パイプラインレイアウト
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_COMPOSITE_2, &m_pComposite2PipelineLayout);

			// パイプライン
			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_COMPOSITE, 0,
				GRAPHICS_PIPELINE_TYPE_COMPOSITE_2, desc,
				&m_Composite2PipelineHandle) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// ジオメトリ
		// ----------------------------------------------------------------------------------------------------

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_GEOMETRY, &m_pGeometryPipelineLayout);

		// レンダーパスのハンドルを取得
		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_GEOMETRY, &m_GeometryRenderPassHandle);

		// ----------------------------------------------------------------------------------------------------
		// 遠景
		// ----------------------------------------------------------------------------------------------------

		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW, &m_DistantViewRenderPassHandle);

		// スカイドーム
		if (m_SkyDome.Initialize(&m_GraphicsManager) == false)
		{
			return false;
		}

//		m_SkyDome.SetHemisphere(1000.0f, 500.0f);
		m_SkyDome.SetHemisphere(5000.0f, 1000.0f);
		m_SkyDome.SetSunPosition(Vector3(0.0f, -1.0f, 0.0f));

		// ----------------------------------------------------------------------------------------------------
		// SSAO
		// ----------------------------------------------------------------------------------------------------

		{
			std::wstring filePath;
			CreateFilePath(L"image\\noise.bmp", filePath);

			V3D_RESULT result = CreateImageFromFile(
				Application::GetDevice(),
				GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence(),
				filePath.c_str(), false, V3D_PIPELINE_STAGE_FRAGMENT_SHADER,
				&m_pSSAONoiseImage);

			if (result != V3D_OK)
			{
				return false;
			}

			// レンダーパス
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_SSAO, &m_SsaoRenderPassHandle);

			// パイプラインレイアウト
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_SSAO, &m_pSsaoPipelineLayout);

			// パイプライン
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_SSAO, 0,
				GRAPHICS_PIPELINE_TYPE_SSAO, desc,
				&m_SsaoPipelineHandle) != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_PIPELINE_TYPE_SSAO,
				GRAPHICS_DESCRIPTOR_SET_TYPE_SSAO,
				&m_pSsaoDescriptorSet) != V3D_OK)
			{
				return false;
			}

			m_pSsaoDescriptorSet->SetImageViewAndSampler(2, m_pSSAONoiseImage, m_pSimpleSampler);

			// サンプラー
			V3DSamplerDesc samplerDesc{};
			samplerDesc.magFilter = V3D_FILTER_NEAREST;
			samplerDesc.minFilter = V3D_FILTER_NEAREST;
			samplerDesc.mipmapMode = V3D_MIPMAP_MODE_NEAREST;
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_CLAMP_TO_EDGE;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 0.0f;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

			if (Application::GetDevice()->CreateSampler(samplerDesc, &m_pSsaoSampler) != V3D_OK)
			{
				return false;
			}

			/**********/
			/* ブラー */
			/*********/

			// ダウンサンプリング : デスクリプタセット
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_BLUR_DAWN_SAMPLING,
				&m_pSsaoBlurDawnSamplingDescriptorSet) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// シャドウ
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_SHADOW, &m_ShadowRenderPassHandle);

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_SHADOW, &m_pShadowPipelineLayout);
		}

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
			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING, &m_DirectionalLightingStage.pPipelineLayout);

			// グラフィックスパイプラインのハンドルを取得
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;	
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_LIGHTING, 0,
				GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING, desc,
				&m_DirectionalLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

			// ユニフォームバッファーを更新
			m_DirectionalLightUniform.lightDir = Vector3(0.0f, 1.0f, 0.0f).ToNormalize();
			m_DirectionalLightUniform.lightColor.Set(1.0f, 1.0f, 1.0f, 1.0f);

			void* pMemory;
			if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(DirectionalLightingUniform), &pMemory) == V3D_OK)
			{
				memcpy_s(pMemory, sizeof(DirectionalLightingUniform), &m_DirectionalLightUniform, sizeof(DirectionalLightingUniform));
				m_DirectionalLightingStage.pUniformBuffer->Unmap();
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
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
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING, &m_PointLightingStage.pPipelineLayout);

			// グラフィックスパイプラインのハンドルを取得
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_ADDITION;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_LIGHTING, 1,
				GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING, desc,
				&m_PointLightingStage.pipelineHandle) != V3D_OK)
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
							
							float r = 0.15f;//static_cast<float>(rand() % 100) / 100.0f;
							float g = 0.15f;//static_cast<float>(rand() % 100) / 100.0f;
							float b = 0.15f;//static_cast<float>(rand() % 100) / 100.0f;

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
				GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING,
				&m_PointLightingStage.pDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			m_PointLightingStage.pDescriptorSet->SetBuffer(0, m_PointLightingStage.pUniformBuffer, 0, sizeof(PointLightingUniform));
		}

		// フィニッシュライティング
		{
			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING, &m_FinishLightingStage.pPipelineLayout);

			// パイプラインのハンドルを取得
			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_LIGHTING, 2,
				GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING, desc,
				&m_FinishLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING,
				GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING,
				&m_FinishLightingStage.pDescriptorSet) != V3D_OK)
			{
				return false;
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			;
		}

		// ----------------------------------------------------------------------------------------------------
		// ブルーム
		// ----------------------------------------------------------------------------------------------------

		{
			/**********/
			/* ブラー */
			/**********/

			// ダウンサンプリング : デスクリプタセット
			for (size_t i = 0; i < m_pBloomBlurDawnSamplingDescriptorSet.size(); i++)
			{
				if (m_GraphicsManager.CreateDescriptorSet(
					GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING,
					GRAPHICS_DESCRIPTOR_SET_TYPE_BLUR_DAWN_SAMPLING,
					&m_pBloomBlurDawnSamplingDescriptorSet[i]) != V3D_OK)
				{
					return false;
				}
			}

			/********/
			/* 合成 */
			/********/

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_PIPELINE_TYPE_COMPOSITE_2,
				GRAPHICS_DESCRIPTOR_SET_TYPE_COMPOSITE,
				&m_pBloomComposite2DescriptorSet) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージエフェクト
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, &m_ImageEffectRenderPassHandle);

			for (size_t i = 0; i < m_pImageEffectDescriptorSet.size(); i++)
			{
				// デスクリプタセットを作成
				if (m_GraphicsManager.CreateDescriptorSet(
					GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA,
					GRAPHICS_DESCRIPTOR_SET_TYPE_IMAGE_EFFECT_FXAA,
					&m_pImageEffectDescriptorSet[i]) != V3D_OK)
				{
					return false;
				}
			}

			// イメージの設定、更新は OnRestoreSwapChain で行う
			;

			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			/********/
			/* Copy */
			/********/

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY, &m_pIE_CopyPipelineLayout);

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, 1,
				GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY, desc,
				&m_PeCopyPipelineHandle) != V3D_OK)
			{
				return false;
			}

			/**************/
			/* Correction */
			/**************/

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_CORRECTION, &m_pIE_CorrectionPipelineLayout);

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, 0,
				GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_CORRECTION, desc,
				&m_PeCorrectionPipelineHandle) != V3D_OK)
			{
				return false;
			}

			/***************/
			/* ToneMapping */
			/***************/

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING, &m_pIE_ToneMappingPipelineLayout);

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, 1,
				GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING, desc,
				&m_PeToneMappingPipelineHandle) != V3D_OK)
			{
				return false;
			}

			/********/
			/* FXAA */
			/********/

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA, &m_pIE_FxaaPipelineLayout);

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, 2,
				GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA, desc,
				&m_PeFxaaPipelineHandle) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// オーバーレイ
		// ----------------------------------------------------------------------------------------------------

		{
			// レンダーパスのハンドルを取得
			m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_FINISH, &m_OverlayRenderPassHandle);

			// パイプラインレイアウトを取得
			m_GraphicsManager.GetPipelineLayout(GRAPHICS_PIPELINE_TYPE_FINISH_INIT, &m_pOverlayCopyPipelineLayout);

			/********/
			/* Copy */
			/********/

			GraphicsPipelienDesc desc{};
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_FINISH, 0,
				GRAPHICS_PIPELINE_TYPE_FINISH_INIT, desc,
				&m_OverlayCopyPipelineHandle) != V3D_OK)
			{
				return false;
			}

			// デスクリプタセットを作成
			if (m_GraphicsManager.CreateDescriptorSet(
				GRAPHICS_PIPELINE_TYPE_FINISH_INIT,
				GRAPHICS_DESCRIPTOR_SET_TYPE_OVERLAY_COPY,
				&m_pOverlayCopyDescriptorSet) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// 並列データの初期化
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_UpdateParallelData.pMeshes = &m_Meshes;
		m_UpdateParallelData.threadDatum.resize(threadCount);

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

		/****************/
		/* オーバーレイ */
		/****************/

		SAFE_RELEASE(m_pOverlayCopyDescriptorSet);
		SAFE_RELEASE(m_pOverlayCopyPipelineLayout);

		/**********************/
		/* イメージエフェクト */
		/**********************/

		for (size_t i = 0; i < m_pImageEffectDescriptorSet.size(); i++)
		{
			SAFE_RELEASE(m_pImageEffectDescriptorSet[i]);
		}

		SAFE_RELEASE(m_pIE_CopyPipelineLayout);
		SAFE_RELEASE(m_pIE_CorrectionPipelineLayout);
		SAFE_RELEASE(m_pIE_ToneMappingPipelineLayout);
		SAFE_RELEASE(m_pIE_FxaaPipelineLayout);

		/********/
		/* SSAO */
		/********/

		SAFE_RELEASE(m_pSsaoSampler);
		SAFE_RELEASE(m_pSsaoDescriptorSet);
		SAFE_RELEASE(m_pSsaoPipelineLayout);
		SAFE_RELEASE(m_pSSAONoiseImage);

		SAFE_RELEASE(m_pSsaoBlurDawnSamplingDescriptorSet);

		/********/
		/* 合成 */
		/********/

		SAFE_RELEASE(m_pBloomComposite2DescriptorSet);
		SAFE_RELEASE(m_pComposite2PipelineLayout);

		/**********/
		/* ブラー */
		/**********/

		SAFE_RELEASE(m_pBlurDawnSamplingPipelineLayout);

		if (m_pBloomBlurDawnSamplingDescriptorSet.empty() == false)
		{
			for (size_t i = 0; i < m_pBloomBlurDawnSamplingDescriptorSet.size(); i++)
			{
				SAFE_RELEASE(m_pBloomBlurDawnSamplingDescriptorSet[i]);
			}
		}

		SAFE_RELEASE(m_pBlurGaussianPipelineLayout);

		if (m_pBlurGaussianDescriptorSet.empty() == false)
		{
			for (size_t i = 0; i < m_pBlurGaussianDescriptorSet.size(); i++)
			{
				for (size_t j = 0; j < m_pBlurGaussianDescriptorSet.size(); j++)
				{
					SAFE_RELEASE(m_pBlurGaussianDescriptorSet[i][j]);
				}
			}
		}

		/****************/
		/* ブライトパス */
		/****************/

		SAFE_RELEASE(m_pBrightPassDescriptorSet);
		SAFE_RELEASE(m_pBrightPassPipelineLayout);

		/****************/
		/* ライティング */
		/****************/

		SAFE_RELEASE(m_FinishLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_FinishLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_PointLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_PointLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_PointLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_DirectionalLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_DirectionalLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_DirectionalLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_pShadowPipelineLayout);

		SAFE_RELEASE(m_pGeometryPipelineLayout);

		SAFE_RELEASE(m_pDownSampler);
		SAFE_RELEASE(m_pSimpleSampler);
		SAFE_RELEASE(m_pSimpleVertexBuffer);

		m_SkyDome.Dispose();
		m_Meshes.clear();

		SAFE_RELEASE(m_pQueryPool);

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
		size_t meshCount = m_Meshes.size();

		// ----------------------------------------------------------------------------------------------------
		// テキスト
		// ----------------------------------------------------------------------------------------------------

		uint64_t data[11] = {};

		if (m_bQueryActive == true)
		{
			m_pQueryPool->GetResult(0, 1, sizeof(data), data, 0, 0);
		}

		std::wstringstream text;

		m_Font.Reset();

		text << L"Fps " << Application::GetAverageFpsPreSec();
		m_Font.AddText(16, 16, text.str().c_str());
/*
		float dt = static_cast<float>(Application::GetDeltaTime());

		int32_t textY = 48;
		for (uint32_t i = 0; i < m_ParallelManager.GetThreadCount(); i++)
		{
#if 1
			long long busy = static_cast<long long>(FLOAT_DIV(static_cast<float>(m_ParallelManager.GetThreadInfo(i).busy) / 1000000.0f, dt) * 100.0f);
#else
			long long busy = MAXIMUM(1, m_ParallelManager.GetThreadInfo(i).jobCount);
#endif

			text.str(L"");
			text << L"Thread[" << i << L"] ";

			for (auto j = 0; j < busy; j++)
			{
				text << L"[]";
			}

			m_Font.AddText(16, textY, text.str().c_str());
			textY += 16;
		}
*/
		if (m_bQueryEnable == true)
		{
			text.str(L"");
			text << L"VertexCount " << data[0];
			m_Font.AddText(16, 48, text.str().c_str());
			text.str(L"");
			text << L"PrimiitiveCount " << data[1];
			m_Font.AddText(16, 64, text.str().c_str());
			text.str(L"");
			text << L"VertexShaderInvocationCount " << data[2];
			m_Font.AddText(16, 80, text.str().c_str());
			text.str(L"");
			text << L"GeometryShaderInvocationCount " << data[3];
			m_Font.AddText(16, 96, text.str().c_str());
			text.str(L"");
			text << L"GeometryShaderPrimitiveCount " << data[4];
			m_Font.AddText(16, 112, text.str().c_str());
			text.str(L"");
			text << L"ClippingInvocationCount " << data[5];
			m_Font.AddText(16, 128, text.str().c_str());
			text.str(L"");
			text << L"ClippingPrimitiveCount " << data[6];
			m_Font.AddText(16, 144, text.str().c_str());
			text.str(L"");
			text << L"FragmentShaderInvocationCount " << data[7];
			m_Font.AddText(16, 160, text.str().c_str());
			text.str(L"");
			text << L"TessellationControlPatchCount " << data[8];
			m_Font.AddText(16, 176, text.str().c_str());
			text.str(L"");
			text << L"TessellationEvaluationShaderInvocationCount " << data[9];
			m_Font.AddText(16, 192, text.str().c_str());
			text.str(L"");
			text << L"ComnputeShaderInvocationCount " << data[10];
			m_Font.AddText(16, 208, text.str().c_str());
		}
/*
		text.str(L"");
		text << L"Radius " << m_SsaoRadius;
		m_Font.AddText(16, 48, text.str().c_str());
		text.str(L"");
		text << L"Threshold " << m_SsaoThreshold;
		m_Font.AddText(16, 64, text.str().c_str());
		text.str(L"");
		text << L"Distance " << m_SsaoDist;
		m_Font.AddText(16, 80, text.str().c_str());
		text.str(L"");
		text << L"Intensity " << m_SsaoIntensity;
		m_Font.AddText(16, 98, text.str().c_str());
*/
//		text.str(L"");
//		text << L"SortDrawObject " << m_DrawGeometryParallelData.sortDrawObjects;
//		m_Font.AddText(16, 64, text.str().c_str());

		m_Font.Bake();

		// ----------------------------------------------------------------------------------------------------
		// 更新
		// ----------------------------------------------------------------------------------------------------

		m_ParallelManager.Reset();

		float aspectRatio = (float)swapChainDesc.imageWidth / (float)swapChainDesc.imageHeight;
//		float aspectRatio = (swapChainDesc.windowed == false) ? (16.0f / 9.0f) : (float)swapChainDesc.imageWidth / (float)swapChainDesc.imageHeight;

		// カメラ
		m_Camera.SetProjection(DEG_TO_RAD(80.0f), aspectRatio, 0.1f, 4000.0f);
		m_Camera.SetView(m_CameraRot, m_CameraAt, m_CameraDistance);
		m_Camera.Update();

		Vector3 eyePos = m_Camera.GetEye();
		Vector3 eyeDir = m_Camera.GetEye().ToNormalize();

		// メッシュ
		Update(currentImageIndex);

		void * pMemory;

		// ディレクショナルライト
		m_DirectionalLightUniform.eyeDir = eyeDir;
		m_DirectionalLightUniform.invViewProjMatrix = m_Camera.GetInverseViewProjectionMatrix();
		m_DirectionalLightUniform.lightMatrix = m_DrawShadowParallelData.constant.viewProjMatrix;
		m_DirectionalLightUniform.shadowParam.x = 1.0f / TO_F32(GraphicsManager::ShadowMapSize);
		m_DirectionalLightUniform.shadowParam.y = 1.0f / TO_F32(GraphicsManager::ShadowMapSize);

		if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(DirectionalLightingUniform), &pMemory) == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(DirectionalLightingUniform), &m_DirectionalLightUniform, sizeof(DirectionalLightingUniform));
			m_DirectionalLightingStage.pUniformBuffer->Unmap();
		}

		// ポイントライト
		if (m_PointLightingStage.pUniformBuffer->Map(0, 0, &pMemory) == V3D_OK)
		{
			uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

			memcpy_s(pAddr, sizeof(Vector3), &eyePos, sizeof(Vector3));

			const Matrix4x4& invViewProjMat = m_Camera.GetInverseViewProjectionMatrix();
			memcpy_s(pAddr + sizeof(Vector4), sizeof(Matrix4x4), &invViewProjMat, sizeof(Matrix4x4));

			m_PointLightingStage.pUniformBuffer->Unmap();
		}


		// スカイドーム
		m_SkyDome.Update(m_Camera.GetViewProjectionMatrix(), eyePos);

		// ----------------------------------------------------------------------------------------------------
		// 描画
		// ----------------------------------------------------------------------------------------------------

		IV3DCommandBuffer* pGraphicsCommandBuffer = BeginGraphics();
		if (pGraphicsCommandBuffer == nullptr)
		{
			return false;
		}

		float color[4] = { 1.0f, 0.5f, 0.5f, 1.0f };

		// クエリを開始
		if (m_bQueryEnable == true)
		{
			DEBUG_MARKER_CODE(pGraphicsCommandBuffer->InsertDebugMarker("BeginQuery", color));
			pGraphicsCommandBuffer->ResetQueryPool(m_pQueryPool, 0, 1);
			pGraphicsCommandBuffer->BeginQuery(m_pQueryPool, 0);
		}

		// ジオメトリ
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("Geometry", color));
		DrawGeometry(pGraphicsCommandBuffer);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// 遠景
		color[0] = 0.5f;
		color[1] = 1.0f;
		color[2] = 0.5f;
		color[3] = 1.0f;
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("DistantView", color));
		DrawDistantView(pGraphicsCommandBuffer);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// イルミネーション ( SSAO、シャドウマッピング、ライティング )
		color[0] = 0.5f;
		color[1] = 0.5f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("Illumination", color));
		DrawIllumination(pGraphicsCommandBuffer, currentImageIndex);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// ブルーム
		color[0] = 1.0f;
		color[1] = 1.0f;
		color[2] = 0.5f;
		color[3] = 1.0f;
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("Bloom", color));
		DrawBloom(pGraphicsCommandBuffer);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// イメージエフェクト
		color[0] = 1.0f;
		color[1] = 0.5f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("ImageEffect", color));
		DrawImageEffect(pGraphicsCommandBuffer);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// フィニッシュ
		color[0] = 0.5f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		color[3] = 1.0f;
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->BeginDebugMarker("Finish", color));
		DrawFinish(pGraphicsCommandBuffer);
		DEBUG_MARKER_CODE(pGraphicsCommandBuffer->EndDebugMarker());

		// クエリを終了
		if (m_bQueryEnable == true)
		{
			DEBUG_MARKER_CODE(pGraphicsCommandBuffer->InsertDebugMarker("EndQuery", color));
			pGraphicsCommandBuffer->EndQuery(m_pQueryPool, 0);
			m_bQueryActive = true;
		}

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
		if (m_DrawGeometryParallelData.commandBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_DrawGeometryParallelData.commandBuffers.size(); i++)
			{
				for (size_t j = 0; j < m_DrawGeometryParallelData.commandBuffers[i].size(); j++)
				{
					SAFE_RELEASE(m_DrawGeometryParallelData.commandBuffers[i][j]);
				}

				m_DrawGeometryParallelData.commandBuffers[i].clear();
			}

			m_DrawGeometryParallelData.commandBuffers.clear();
		}

		if (m_DrawShadowParallelData.commandBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_DrawShadowParallelData.commandBuffers.size(); i++)
			{
				for (size_t j = 0; j < m_DrawShadowParallelData.commandBuffers[i].size(); j++)
				{
					SAFE_RELEASE(m_DrawShadowParallelData.commandBuffers[i][j]);
				}

				m_DrawShadowParallelData.commandBuffers[i].clear();
			}

			m_DrawShadowParallelData.commandBuffers.clear();
		}

		SAFE_RELEASE(m_pGeometryFrameBuffer);
		SAFE_RELEASE(m_pDistantViewFrameBuffer);
		SAFE_RELEASE(m_pShadowFrameBuffer);
		SAFE_RELEASE(m_pLightingFrameBuffer);
		SAFE_RELEASE(m_pBrightPassFrameBuffer);
		SAFE_RELEASE(m_pSsaoFrameBuffer);

		for (size_t i = 0; i < m_pBlurFrameBuffer.size(); i++)
		{
			SAFE_RELEASE(m_pBlurFrameBuffer[i]);
		}

		SAFE_RELEASE(m_pCompositeFrameBuffer);
		SAFE_RELEASE(m_pImageEffectFrameBuffer);

		if (m_OverlayFrameBuffers.empty() == false)
		{
			for (size_t i = 0; i < m_OverlayFrameBuffers.size(); i++)
			{
				SAFE_RELEASE(m_OverlayFrameBuffers[i]);
			}
			m_OverlayFrameBuffers.clear();
		}

		SAFE_RELEASE(m_pFinishBarrierSet);

		m_GraphicsManager.Lost();
		m_Font.Lost();
	}

	virtual bool OnRestoreSwapChain() override
	{
		if (m_GraphicsManager.Restore() != V3D_OK)
		{
			return false;
		}
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		// ----------------------------------------------------------------------------------------------------
		// フレームバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		// ジオメトリ
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_GEOMETRY, 0, &m_pGeometryFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// 遠景
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW, 0, &m_pDistantViewFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// シャドウ
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_SHADOW, 0, &m_pShadowFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// ライティング
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_LIGHTING, 0, &m_pLightingFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// ブライトパス
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS, 0, &m_pBrightPassFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// ブラー
		for (size_t i = 0; i < m_pBlurFrameBuffer.size(); i++)
		{
			if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_BLUR, TO_UI32(i), &m_pBlurFrameBuffer[i]) != V3D_OK)
			{
				return false;
			}
		}

		// 合成
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_COMPOSITE, 1, &m_pCompositeFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// SSAO
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_SSAO, 0, &m_pSsaoFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// イメージエフェクト
		if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT, 0, &m_pImageEffectFrameBuffer) != V3D_OK)
		{
			return false;
		}

		// オーバーレイ
		m_OverlayFrameBuffers.resize(swapChainDesc.imageCount);
		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_FINISH, i, &m_OverlayFrameBuffers[i]) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを再設定
		// ----------------------------------------------------------------------------------------------------

		IV3DImageView* pColorImageView0;
		IV3DImageView* pColorImageView1;
		IV3DImageView* pNormalImageView;
		IV3DImageView* pDataImageView;
		IV3DImageView* pSsaoImageView;
		IV3DImageView* pShadowImageView;
		IV3DImageView* pLightImageView;
		IV3DImageView* pBrightPassImageView;
		IV3DImageView* pBlurImageView0;
		IV3DImageView* pPeImageView0;
		IV3DImageView* pPeImageView1;
		IV3DImageView* pIeLDRImageView0;
		IV3DImageView* pIeLDRImageView1;

		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_COLOR, &pColorImageView0);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0, &pNormalImageView);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1, &pDataImageView);

		m_pSsaoFrameBuffer->GetAttachment(0, &pSsaoImageView);
		m_pShadowFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_SHADOW_BUFFER, &pShadowImageView);

		m_pLightingFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR, &pLightImageView);
		m_pLightingFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COMPOSITE_COLOR, &pColorImageView1);

		m_pBrightPassFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_BRIGHT_PASS_COLOR, &pBrightPassImageView);
		m_pBlurFrameBuffer[0]->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0, &pBlurImageView0);

		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0, &pPeImageView0);
		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_1, &pPeImageView1);
		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0, &pIeLDRImageView0);
		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_1, &pIeLDRImageView1);

		/**********/
		/* ブラー */
		/**********/

		// ガウシアン
		for (size_t i = 0; i < m_pBlurGaussianDescriptorSet.size(); i++)
		{
			for (size_t j = 0; j < m_pBlurGaussianDescriptorSet[i].size(); j++)
			{
				IV3DImageView* pImageView;

				m_pBlurFrameBuffer[i]->GetAttachment(TO_UI32(j), &pImageView);

				m_pBlurGaussianDescriptorSet[i][j]->SetImageViewAndSampler(0, pImageView, m_pDownSampler);
				m_pBlurGaussianDescriptorSet[i][j]->Update();

				SAFE_RELEASE(pImageView);
			}
		}

		/********/
		/* SSAO */
		/********/

		m_pSsaoDescriptorSet->SetImageViewAndSampler(0, pNormalImageView, m_pSsaoSampler);
		m_pSsaoDescriptorSet->SetImageViewAndSampler(1, pDataImageView, m_pSsaoSampler);
		m_pSsaoDescriptorSet->Update();

		m_pSsaoBlurDawnSamplingDescriptorSet->SetImageViewAndSampler(0, pSsaoImageView, m_pSsaoSampler);
		m_pSsaoBlurDawnSamplingDescriptorSet->Update();

		/****************/
		/* ライティング */
		/****************/

		// ディレクショナル
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(3, pShadowImageView, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->Update();

		// ポイント
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->Update();

		// フィニッシュ
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(0, pColorImageView0, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pBlurImageView0, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pLightImageView, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->Update();

		/************/
		/* ブルーム */
		/************/

		// ブライトパス
		m_pBrightPassDescriptorSet->SetImageViewAndSampler(0, pColorImageView1, m_pSimpleSampler);
		m_pBrightPassDescriptorSet->Update();

		// ブラー : ダウンサンプリング
		m_pBloomBlurDawnSamplingDescriptorSet[0]->SetImageViewAndSampler(0, pBrightPassImageView, m_pDownSampler);
		m_pBloomBlurDawnSamplingDescriptorSet[0]->Update();
		m_pBloomBlurDawnSamplingDescriptorSet[1]->SetImageViewAndSampler(0, pBlurImageView0, m_pDownSampler);
		m_pBloomBlurDawnSamplingDescriptorSet[1]->Update();

		// 合成
		for (size_t i = 0; i < m_pBlurGaussianDescriptorSet.size(); i++)
		{
			IV3DImageView* pImageView;

			m_pBlurFrameBuffer[i]->GetAttachment(0, &pImageView);

			m_pBloomComposite2DescriptorSet->SetImageViewAndSampler(TO_UI32(i), pImageView, m_pSimpleSampler);

			SAFE_RELEASE(pImageView);
		}
		m_pBloomComposite2DescriptorSet->Update();

		/**********************/
		/* イメージエフェクト */
		/**********************/

		m_pImageEffectDescriptorSet[0]->SetImageViewAndSampler(0, pPeImageView1, m_pSimpleSampler);
		m_pImageEffectDescriptorSet[0]->Update();

		m_pImageEffectDescriptorSet[1]->SetImageViewAndSampler(0, pPeImageView0, m_pSimpleSampler);
		m_pImageEffectDescriptorSet[1]->Update();

		m_pImageEffectDescriptorSet[2]->SetImageViewAndSampler(0, pIeLDRImageView0, m_pSimpleSampler);
		m_pImageEffectDescriptorSet[2]->Update();

		/****************/
		/* フィニッシュ */
		/****************/

		m_pOverlayCopyDescriptorSet->SetImageViewAndSampler(0, pIeLDRImageView1, m_pSimpleSampler);
		m_pOverlayCopyDescriptorSet->Update();

		SAFE_RELEASE(pIeLDRImageView1);
		SAFE_RELEASE(pIeLDRImageView0);
		SAFE_RELEASE(pPeImageView1);
		SAFE_RELEASE(pPeImageView0);
		SAFE_RELEASE(pBlurImageView0);
		SAFE_RELEASE(pBrightPassImageView);
		SAFE_RELEASE(pLightImageView);
		SAFE_RELEASE(pShadowImageView);
		SAFE_RELEASE(pSsaoImageView);
		SAFE_RELEASE(pDataImageView);
		SAFE_RELEASE(pNormalImageView);
		SAFE_RELEASE(pColorImageView1);
		SAFE_RELEASE(pColorImageView0);

		// ----------------------------------------------------------------------------------------------------
		// フィニッシュバリア
		// ----------------------------------------------------------------------------------------------------

		static constexpr V3DImageViewMemoryBarrier memoryBarrierOne =
		{
			V3D_ACCESS_SHADER_READ,
			V3D_ACCESS_COLOR_ATTACHMENT_WRITE,
			V3D_QUEUE_FAMILY_IGNORED,
			V3D_QUEUE_FAMILY_IGNORED,
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT,
			nullptr,
		};

		static V3DImageViewMemoryBarrier memoryBarriers[8] =
		{
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
			memoryBarrierOne,
		};

		static uint32_t memoryBarrierCount = _countof(memoryBarriers);

		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0, &memoryBarriers[0].pImageView);
		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_1, &memoryBarriers[1].pImageView);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0, &memoryBarriers[2].pImageView);
		m_pGeometryFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1, &memoryBarriers[3].pImageView);

		m_pSsaoFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_SSAO_COLOR, &memoryBarriers[4].pImageView);
		m_pShadowFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_SHADOW_BUFFER, &memoryBarriers[5].pImageView);

		m_pImageEffectFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_1, &memoryBarriers[6].pImageView);

		m_pBrightPassFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_BRIGHT_PASS_COLOR, &memoryBarriers[7].pImageView);

		V3DPipelineBarrier pipelineBarrier{};

		V3DBarrierSetDesc barrierSetDesc{};
		barrierSetDesc.pipeline.srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		barrierSetDesc.pipeline.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		barrierSetDesc.imageViewMemoryCount = memoryBarrierCount;
		barrierSetDesc.pImageViewMemories = memoryBarriers;

		V3D_RESULT result = Application::GetDevice()->CreateBarrierSet(barrierSetDesc, &m_pFinishBarrierSet, nullptr);
		if (result != V3D_OK)
		{
			return false;
		}

		for (uint32_t i = 0; i < memoryBarrierCount; i++)
		{
			SAFE_RELEASE(memoryBarriers[i].pImageView);
		}

		// ----------------------------------------------------------------------------------------------------
		// 並列処理用のデータを作成
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_DrawGeometryParallelData.commandBuffers.resize(swapChainDesc.imageCount);
		m_DrawGeometryParallelData.pRenderPass = m_GeometryRenderPassHandle->GetPtr();
		m_DrawGeometryParallelData.pFrameBuffer = m_pGeometryFrameBuffer;
		m_DrawGeometryParallelData.viewport.rect.x = 0;
		m_DrawGeometryParallelData.viewport.rect.y = 0;
		m_DrawGeometryParallelData.viewport.rect.width = swapChainDesc.imageWidth;
		m_DrawGeometryParallelData.viewport.rect.height = swapChainDesc.imageHeight;
		m_DrawGeometryParallelData.viewport.minDepth = 0.0f;
		m_DrawGeometryParallelData.viewport.maxDepth = 1.0f;
		m_DrawGeometryParallelData.scissor.x = 0;
		m_DrawGeometryParallelData.scissor.y = 0;
		m_DrawGeometryParallelData.scissor.width = swapChainDesc.imageWidth;
		m_DrawGeometryParallelData.scissor.height = swapChainDesc.imageHeight;

		m_DrawShadowParallelData.commandBuffers.resize(swapChainDesc.imageCount);
		m_DrawShadowParallelData.pRenderPass = m_ShadowRenderPassHandle->GetPtr();
		m_DrawShadowParallelData.pFrameBuffer = m_pShadowFrameBuffer;
		m_DrawShadowParallelData.viewport.rect.x = 0;
		m_DrawShadowParallelData.viewport.rect.y = 0;
		m_DrawShadowParallelData.viewport.rect.width = GraphicsManager::ShadowMapSize;
		m_DrawShadowParallelData.viewport.rect.height = GraphicsManager::ShadowMapSize;
		m_DrawShadowParallelData.viewport.minDepth = 0.0f;
		m_DrawShadowParallelData.viewport.maxDepth = 1.0f;
		m_DrawShadowParallelData.scissor.x = 0;
		m_DrawShadowParallelData.scissor.y = 0;
		m_DrawShadowParallelData.scissor.width = GraphicsManager::ShadowMapSize;
		m_DrawShadowParallelData.scissor.height = GraphicsManager::ShadowMapSize;

		m_DrawGeometryParallelData.commandBuffers.resize(swapChainDesc.imageCount);
		m_DrawShadowParallelData.commandBuffers.resize(swapChainDesc.imageCount);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			for (uint32_t j = 0; j < threadCount; j++)
			{
				IV3DCommandBuffer* pCommandBuffer;

				V3DCommandPoolDesc commandPoolDesc;
				commandPoolDesc.usageFlags = V3D_COMMAND_POOL_USAGE_RESET_COMMAND_BUFFER;
				commandPoolDesc.queueFamily = GetGraphicsQueueFamily();

//				V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(m_CommandPools[j], V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_DrawGeometryParallelData.commandBuffers[i].push_back(pCommandBuffer);

//				result = Application::GetDevice()->CreateCommandBuffer(m_CommandPools[j], V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				result = Application::GetDevice()->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_DrawShadowParallelData.commandBuffers[i].push_back(pCommandBuffer);
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// フォントを復帰
		// ----------------------------------------------------------------------------------------------------

		if (m_Font.Restore(m_OverlayRenderPassHandle->GetPtr(), 1, swapChainDesc.imageWidth, swapChainDesc.imageHeight) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------

		return true;
	}

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState)
	{
		if (keyCode == 'W')
		{
			RotateLightDirection(Vector3::XAxis(), +0.1f);
		}
		else if (keyCode == 'S')
		{
			RotateLightDirection(Vector3::XAxis(), -0.1f);
		}
		else if (keyCode == 'A')
		{
			RotateLightDirection(Vector3::ZAxis(), +0.1f);
		}
		else if (keyCode == 'D')
		{
			RotateLightDirection(Vector3::ZAxis(), -0.1f);
		}

		if (keyCode == 'N')
		{
			m_DirectionalLightUniform.shadowParam.z -= 0.0001f;
		}
		else if (keyCode == 'M')
		{
			m_DirectionalLightUniform.shadowParam.z += 0.0001f;
		}

		if (keyCode == 'Z')
		{
			m_UpdateParallelData.shadowEnable = !m_UpdateParallelData.shadowEnable;
		}

		if (keyCode == 'X')
		{
			m_DrawGeometryParallelData.sortDrawObjects = !m_DrawGeometryParallelData.sortDrawObjects;
			m_DrawShadowParallelData.sortDrawObjects = !m_DrawShadowParallelData.sortDrawObjects;
		}

		if (keyCode == 'F')
		{
			if (m_PeFxaaConstant.invTexSize.w < 0.5f)
			{
				m_PeFxaaConstant.invTexSize.w = 1.0f;
			}
			else
			{
				m_PeFxaaConstant.invTexSize.w = 0.0f;
			}
		}

		if (keyCode == 'T')
		{
			m_ToneMappingEnable = !m_ToneMappingEnable;
		}

		////////////////////////////////////////////////

		if (keyCode == 'Y')
		{
//			m_CorrectionConstant.invGamma -= 0.01f;
			m_SsaoRadius -= 0.01f;
		}
		else if (keyCode == 'U')
		{
//			m_CorrectionConstant.invGamma += 0.01f;
			m_SsaoRadius += 0.01f;
		}

		if (keyCode == 'I')
		{
//			m_CorrectionConstant.contrast -= 0.01f;
			m_SsaoThreshold -= 0.01f;
		}
		else if (keyCode == 'O')
		{
//			m_CorrectionConstant.contrast += 0.01f;
			m_SsaoThreshold += 0.01f;
		}

		if (keyCode == 'H')
		{
			m_SsaoDist -= 0.01f;
		}
		else if (keyCode == 'J')
		{
			m_SsaoDist += 0.01f;
		}

		if (keyCode == 'K')
		{
			m_SsaoIntensity -= 1.0f;
		}
		else if (keyCode == 'L')
		{
			m_SsaoIntensity += 1.0f;
		}

		if (keyCode == 'C')
		{
			m_SsaoEnable = !m_SsaoEnable;
		}

		if (keyCode == 'V')
		{
			m_BloomEnable = !m_BloomEnable;
		}

		if (keyCode == 'Q')
		{
			m_bQueryEnable = !m_bQueryEnable;
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

	void RotateLightDirection(const Vector3& axis, float angle)
	{
		Quaternion rot;
		rot.SetRotationAxis(axis, DEG_TO_RAD(angle));

		Vector3 sunPos = Matrix4x4(rot) * m_SkyDome.GetSunPosition();
		m_SkyDome.SetSunPosition(sunPos);

		m_DirectionalLightUniform.lightDir = m_SkyDome.GetSunDirection();
		m_DirectionalLightUniform.lightColor = m_SkyDome.GetSunColor();

		void* pMemory;
		if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(DirectionalLightingUniform), &pMemory) == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(DirectionalLightingUniform), &m_DirectionalLightUniform, sizeof(DirectionalLightingUniform));
			m_DirectionalLightingStage.pUniformBuffer->Unmap();
		}
	}

	void Update(uint32_t frame)
	{
		uint32_t threadCount = m_ParallelManager.GetThreadCount();
		uint32_t batchCount = 1000;
		uint32_t minBatchJobCount = batchCount * threadCount;

		m_UpdateParallelData.frustum.Update(m_Camera.GetViewProjectionMatrix());

//		size_t threadCount = m_UpdateParallelData.threadDatum.size();
		for (size_t i = 0; i < threadCount; i++)
		{
			m_UpdateParallelData.threadDatum[i].geometryObjects.clear();
			m_UpdateParallelData.threadDatum[i].shadowObjects.clear();
		}

#ifdef ENABLE_MULTITHREAD

		if (m_Meshes[0]->GetMeshNodeCount() == 1)
		{
			uint32_t subsetCount = m_Meshes[0]->GetMeshNode(0)->GetSubsetCount();

			if (batchCount <= subsetCount)
			{
				if (minBatchJobCount <= subsetCount)
				{
					m_ParallelManager.ExecuteBatch(UpdateSubsetParallel, subsetCount, batchCount, &m_UpdateParallelData);
				}
				else
				{
					m_ParallelManager.Execute(UpdateSubsetParallel, subsetCount, &m_UpdateParallelData);
				}
			}
			else
			{
				UpdateSubsetSingle(
					m_Meshes[0], 0, subsetCount,
					&m_UpdateParallelData.frustum,
					m_UpdateParallelData.shadowEnable,
					&m_UpdateParallelData.threadDatum[0].geometryObjects, &m_UpdateParallelData.threadDatum[0].shadowObjects);
			}
		}
		else
		{
			if (minBatchJobCount <= m_Meshes[0]->GetMeshNodeCount())
			{
				m_ParallelManager.ExecuteBatch(UpdateNodeParallel, m_Meshes[0]->GetMeshNodeCount(), batchCount, &m_UpdateParallelData);
			}
			else
			{
				m_ParallelManager.Execute(UpdateNodeParallel, m_Meshes[0]->GetMeshNodeCount(), &m_UpdateParallelData);
			}
		}

		std::vector<DrawObject>& geometryObjects = m_UpdateParallelData.threadDatum[0].geometryObjects;
		std::vector<DrawObject>& shadowObjects = m_UpdateParallelData.threadDatum[0].shadowObjects;

		for (size_t i = 1; i < threadCount; i++)
		{
			std::vector<DrawObject>& srcGeometryObjects = m_UpdateParallelData.threadDatum[i].geometryObjects;
			std::vector<DrawObject>& srcShadowObjects = m_UpdateParallelData.threadDatum[i].shadowObjects;

			std::copy(srcGeometryObjects.begin(), srcGeometryObjects.end(), std::back_inserter(geometryObjects));
			std::copy(srcShadowObjects.begin(), srcShadowObjects.end(), std::back_inserter(shadowObjects));
		}

#else ENABLE_MULTITHREAD

		UpdateNodeSingle(
			m_Meshes[0], 0, m_Meshes[0]->GetMeshNodeCount(),
			&m_UpdateParallelData.frustum,
			m_UpdateParallelData.shadowEnable,
			&m_UpdateParallelData.threadDatum[0].geometryObjects, &m_UpdateParallelData.threadDatum[0].shadowObjects);

		std::vector<DrawObject>& geometryObjects = m_UpdateParallelData.threadDatum[0].geometryObjects;
		std::vector<DrawObject>& shadowObjects = m_UpdateParallelData.threadDatum[0].shadowObjects;

#endif //ENABLE_MULTITHREAD

		m_DrawGeometryParallelData.frame = frame;
		m_DrawGeometryParallelData.pDrawObjects = &geometryObjects;
		m_DrawGeometryParallelData.constant.viewProjMatrix = m_Camera.GetViewProjectionMatrix();
#ifdef ENABLE_MULTITHREAD
		BeginParallelCommandBuffer(&m_DrawGeometryParallelData);
		if (minBatchJobCount <= TO_UI32(m_DrawGeometryParallelData.pDrawObjects->size()))
		{
			m_ParallelManager.ExecuteBatch(DrawObjectParallel, TO_UI32(m_DrawGeometryParallelData.pDrawObjects->size()), batchCount, &m_DrawGeometryParallelData);
		}
		else
		{
			m_DrawGeometryParallelData.test[0] = 0;
			m_DrawGeometryParallelData.test[1] = 0;
			m_DrawGeometryParallelData.test[2] = 0;
			m_DrawGeometryParallelData.test[3] = 0;
			m_ParallelManager.Execute(DrawObjectParallel, TO_UI32(m_DrawGeometryParallelData.pDrawObjects->size()), &m_DrawGeometryParallelData);
		}
		EndParallelCommandBuffer(&m_DrawGeometryParallelData);
#endif //ENABLE_MULTITHREAD

		m_DrawShadowParallelData.frame = frame;
		m_DrawShadowParallelData.pDrawObjects = &shadowObjects;
		if((m_UpdateParallelData.shadowEnable == true) && (m_DrawShadowParallelData.pDrawObjects->size() > 0))
		{
			const Matrix4x4& viewMat = m_Camera.GetViewMatrix();
			Vector3 eyeVec(viewMat.z.x, viewMat.z.y, viewMat.z.z);

			const Vector3& lightDir = m_DirectionalLightUniform.lightDir;

			Vector3 upVec;
			upVec = Vector3::Cross(eyeVec, lightDir);
			upVec = Vector3::Cross(lightDir, upVec);

			AABB aabb = m_Meshes[0]->GetBounds().aabb;
			aabb.UpdateCenterAndPoints();

			Matrix4x4 lightViewMat = Matrix4x4::LookAt(aabb.center - lightDir, aabb.center, upVec);

			Vector3 aabbMin(+FLT_MAX, +FLT_MAX, +FLT_MAX);
			Vector3 aabbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

			for (uint32_t i = 0; i < 8; i++)
			{
				Vector3 temp = lightViewMat * aabb.points[i];
				aabbMin = Vector3::Minimum(aabbMin, temp);
				aabbMax = Vector3::Maximum(aabbMax, temp);
			}

			float bias = 5.0f;

			aabbMin.z -= bias;
			aabbMax.z += bias;
			Matrix4x4 lightProjMatrix = Matrix4x4::OrthoOffCenter(aabbMin, aabbMax);

			m_DrawShadowParallelData.constant.viewProjMatrix = lightProjMatrix * lightViewMat;
#ifdef ENABLE_MULTITHREAD
			BeginParallelCommandBuffer(&m_DrawShadowParallelData);
			if (minBatchJobCount <= TO_UI32(m_DrawShadowParallelData.pDrawObjects->size()))
			{
				m_ParallelManager.ExecuteBatch(DrawObjectParallel, TO_UI32(m_DrawShadowParallelData.pDrawObjects->size()), batchCount, &m_DrawShadowParallelData);
			}
			else
			{
				m_ParallelManager.Execute(DrawObjectParallel, TO_UI32(m_DrawShadowParallelData.pDrawObjects->size()), &m_DrawShadowParallelData);
			}
			EndParallelCommandBuffer(&m_DrawShadowParallelData);
#endif //ENABLE_MULTITHREAD
		}
	}

	void DrawGeometry(IV3DCommandBuffer* pCommandBuffer)
	{
#ifdef ENABLE_MULTITHREAD

		uint32_t frame = GetSwapChain()->GetCurrentImageIndex();
		std::vector<IV3DCommandBuffer*>& commandBuffers = m_DrawGeometryParallelData.commandBuffers[frame];

		pCommandBuffer->BeginRenderPass(m_pGeometryFrameBuffer, false);
		pCommandBuffer->ExecuteCommandBuffer(TO_UI32(commandBuffers.size()), commandBuffers.data());

#else //ENABLE_MULTITHREAD

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(m_GeometryRenderPassHandle->GetPtr(), m_pGeometryFrameBuffer, true);

		DrawObjectSingle(
			pCommandBuffer,
			m_DrawGeometryParallelData.sortDrawObjects,
			&m_DrawGeometryParallelData.constant,
			&m_DrawGeometryParallelData.viewport, &m_DrawGeometryParallelData.scissor,
			m_DrawGeometryParallelData.pDrawObjects, 0, m_DrawGeometryParallelData.pDrawObjects->size());

#endif //ENABLE_MULTITHREAD

		pCommandBuffer->EndRenderPass();
	}

	void DrawDistantView(IV3DCommandBuffer* pCommandBuffer)
	{
		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(m_pDistantViewFrameBuffer, true);

		m_SkyDome.Draw(pCommandBuffer);

		pCommandBuffer->EndRenderPass();
	}

	void DrawIllumination(IV3DCommandBuffer* pCommandBuffer, uint32_t frame)
	{
		// ----------------------------------------------------------------------------------------------------
		// アンビエントオクルージョン
		// ----------------------------------------------------------------------------------------------------

		float color[4]{};

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Ssao", color));

		pCommandBuffer->BeginRenderPass(m_pSsaoFrameBuffer, true);

		if (m_SsaoEnable == true)
		{
			float invFarZ = 1.0f / 4000.0f;

			static SsaoConstant constant{};
			constant.params0.x = m_SsaoRadius * invFarZ;
			constant.params0.y = m_SsaoThreshold * invFarZ;
			constant.params0.z = (m_SsaoThreshold + m_SsaoDist) * invFarZ;
			constant.params0.w = m_SsaoIntensity;
			constant.params1.x = TO_F32(m_DrawGeometryParallelData.viewport.rect.width) / TO_F32(m_pSSAONoiseImage->GetImageSubresourceSize().width);
			constant.params1.y = TO_F32(m_DrawGeometryParallelData.viewport.rect.height) / TO_F32(m_pSSAONoiseImage->GetImageSubresourceSize().height);
			constant.params1.z = 0.1f;
			constant.params1.w = 4000.0f;

			pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
			pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

			pCommandBuffer->PushConstant(m_pSsaoPipelineLayout, 0, &constant);

			pCommandBuffer->BindPipeline(m_SsaoPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pSsaoPipelineLayout, 0, m_pSsaoDescriptorSet);
//			pCommandBuffer->PushDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pSsaoPipelineLayout, 0, m_pSsaoDescriptorSet);
			pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
			pCommandBuffer->Draw(4, 1, 0, 0);
		}

		pCommandBuffer->EndRenderPass();

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		/**********/
		/* ブラー */
		/**********/

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Ssao (Blur)", color));

		// サンプリング元のサイズ
		const V3DFrameBufferDesc& frameBufferDesc = m_pBlurFrameBuffer[0]->GetDesc();
		uint32_t dstWidth = frameBufferDesc.attachmentWidth;
		uint32_t dstHeight = frameBufferDesc.attachmentHeight;

		// サンプリング先のサイズ
		IV3DImageView* pSrcImageView;
		m_pSsaoBlurDawnSamplingDescriptorSet->GetImageView(0, &pSrcImageView);

		const V3DSize3D& srcImageSize = pSrcImageView->GetImageSubresourceSize();
		uint32_t srcWidth = srcImageSize.width;
		uint32_t srcHeight = srcImageSize.height;

		SAFE_RELEASE(pSrcImageView);

		// ダウンサンプリング
		SamplingConstant samplingConstant;
		samplingConstant.texelSize = SamplingConstant::CalcTexelSize(srcWidth, srcHeight);

		V3DViewport viewport{};
		viewport.rect.width = dstWidth;
		viewport.rect.height = dstHeight;

		pCommandBuffer->SetViewport(0, 1, &viewport);
		pCommandBuffer->SetScissor(0, 1, &viewport.rect);

		pCommandBuffer->BeginRenderPass(m_pBlurFrameBuffer[0], true);

		pCommandBuffer->PushConstant(m_pBlurDawnSamplingPipelineLayout, 0, &samplingConstant);
		pCommandBuffer->BindPipeline(m_BlurDawnSamplingPipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pBlurDawnSamplingPipelineLayout, 0, m_pSsaoBlurDawnSamplingDescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// ガウシアン
		static const Vector2 dir[2] =
		{
			Vector2(1.0f, 0.0f),
			Vector2(0.0f, 1.0f),
		};

		for (uint32_t i = 0; i < 2; i++)
		{
			GaussianBlurConstant constant{};
			constant.step = GaussianBlurConstant::CalcStep(dstWidth, dstHeight, dir[i]);
			constant.inc = GaussianBlurConstant::CalcInc(m_SsaoBlurRadius);
			constant.sampleCount = m_SsaoBlurSamplingCount;

			pCommandBuffer->NextSubpass();

			pCommandBuffer->PushConstant(m_pBlurGaussianPipelineLayout, 0, &constant);
			pCommandBuffer->BindPipeline(m_BlurGaussianPipelineHandle[i]->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pBlurGaussianPipelineLayout, 0, m_pBlurGaussianDescriptorSet[0][i]);
			pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
			pCommandBuffer->Draw(4, 1, 0, 0);
		}

		pCommandBuffer->EndRenderPass();

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// シャドウ
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Shadow", color));

		if ((m_UpdateParallelData.shadowEnable == true) && (m_DrawShadowParallelData.pDrawObjects->empty() == false))
		{

#ifdef ENABLE_MULTITHREAD

			std::vector<IV3DCommandBuffer*>& commandBuffers = m_DrawShadowParallelData.commandBuffers[frame];

			pCommandBuffer->BeginRenderPass(m_pShadowFrameBuffer, false);
			pCommandBuffer->ExecuteCommandBuffer(TO_UI32(commandBuffers.size()), commandBuffers.data());

#else //ENABLE_MULTITHREAD

			pCommandBuffer->BeginRenderPass(m_ShadowRenderPassHandle->GetPtr(), m_pShadowFrameBuffer, true);

			DrawObjectSingle(
				pCommandBuffer,
				m_DrawShadowParallelData.sortDrawObjects,
				&m_DrawShadowParallelData.constant,
				&m_DrawShadowParallelData.viewport, &m_DrawShadowParallelData.scissor,
				m_DrawShadowParallelData.pDrawObjects, 0, m_DrawShadowParallelData.pDrawObjects->size());

#endif //ENABLE_MULTITHREAD

			pCommandBuffer->EndRenderPass();
		}
		else
		{
			pCommandBuffer->BeginRenderPass(m_pShadowFrameBuffer, true);
			pCommandBuffer->EndRenderPass();
		}

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// ライティング
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Lighting", color));

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(m_pLightingFrameBuffer, true);

		// サブパス 0 : ディレクショナル
		pCommandBuffer->BindPipeline(m_DirectionalLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_DirectionalLightingStage.pPipelineLayout, 0, m_DirectionalLightingStage.pDescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 1 : ポイント
		pCommandBuffer->NextSubpass();
		pCommandBuffer->BindPipeline(m_PointLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_PointLightingStage.pPipelineLayout, 0, m_PointLightingStage.pDescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 2 : フィニッシュ
		pCommandBuffer->NextSubpass();
		pCommandBuffer->BindPipeline(m_FinishLightingStage.pipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_FinishLightingStage.pPipelineLayout, 0, 1, &m_FinishLightingStage.pDescriptorSet, 0, nullptr);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);

		pCommandBuffer->EndRenderPass();

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// 後処理
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Cleanup", color));

		// ブラーのイメージをカラーアタッチメントの書き込みアクセスへ移行
		V3DPipelineBarrier pipelineBarrier{};
		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;

		V3DImageViewMemoryBarrier memoryBarrier{};
		memoryBarrier.srcAccessMask = V3D_ACCESS_SHADER_READ;
		memoryBarrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_SHADER_READ_ONLY;
		memoryBarrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;

		m_pBlurFrameBuffer[0]->GetAttachment(0, &memoryBarrier.pImageView);

		pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

		SAFE_RELEASE(memoryBarrier.pImageView);

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());
	}

	void DrawBloom(IV3DCommandBuffer* pCommandBuffer)
	{
		if (m_BloomEnable == false)
		{
			return;
		}

		uint64_t simpleVertexOffset = 0;

		m_BrightPassConstant.threshold = 0.28f;
		m_BrightPassConstant.offset = 0.1f;

		// ----------------------------------------------------------------------------------------------------
		// ブライトパス
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(float color[4]{});

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Brightpass", color));

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->PushConstant(m_pBrightPassPipelineLayout, 0, &m_BrightPassConstant);
		pCommandBuffer->BeginRenderPass(m_pBrightPassFrameBuffer, true);
		pCommandBuffer->BindPipeline(m_BrightPassPipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pBrightPassPipelineLayout, 0, m_pBrightPassDescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
		pCommandBuffer->EndRenderPass();

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// ブラー
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Blur", color));

		for (uint32_t i = 0; i < 2; i++)
		{
			pCommandBuffer->BeginRenderPass(m_pBlurFrameBuffer[i], true);

			const V3DFrameBufferDesc& frameBufferDesc = m_pBlurFrameBuffer[i]->GetDesc();
			uint32_t dstWidth = frameBufferDesc.attachmentWidth;
			uint32_t dstHeight = frameBufferDesc.attachmentHeight;

			IV3DImageView* pSrcImageView;
			m_pBloomBlurDawnSamplingDescriptorSet[i]->GetImageView(0, &pSrcImageView);

			const V3DSize3D& srcImageSize = pSrcImageView->GetImageSubresourceSize();
			uint32_t srcWidth = srcImageSize.width;
			uint32_t srcHeight = srcImageSize.height;

			SAFE_RELEASE(pSrcImageView);

			// ダウンサンプリング
			SamplingConstant samplingConstant;
			samplingConstant.texelSize = SamplingConstant::CalcTexelSize(srcWidth, srcHeight);

			V3DViewport viewport{};
			viewport.rect.width = dstWidth;
			viewport.rect.height = dstHeight;

			pCommandBuffer->SetViewport(0, 1, &viewport);
			pCommandBuffer->SetScissor(0, 1, &viewport.rect);

			pCommandBuffer->PushConstant(m_pBlurDawnSamplingPipelineLayout, 0, &samplingConstant);
			pCommandBuffer->BindPipeline(m_BlurDawnSamplingPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pBlurDawnSamplingPipelineLayout, 0, m_pBloomBlurDawnSamplingDescriptorSet[i]);
			pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
			pCommandBuffer->Draw(4, 1, 0, 0);

			// ガウシアン
			static const Vector2 dir[2] =
			{
				Vector2(1.0f, 0.0f),
				Vector2(0.0f, 1.0f),
			};

			for (uint32_t j = 0; j < 2; j++)
			{
				GaussianBlurConstant constant{};
				constant.step = GaussianBlurConstant::CalcStep(dstWidth, dstHeight, dir[j]);
				constant.inc = GaussianBlurConstant::CalcInc(m_BloomBlurRadius);
				constant.sampleCount = m_BloomBlurSamplingCount;

				pCommandBuffer->NextSubpass();

				pCommandBuffer->PushConstant(m_pBlurGaussianPipelineLayout, 0, &constant);
				pCommandBuffer->BindPipeline(m_BlurGaussianPipelineHandle[j]->GetPtr());
				pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pBlurGaussianPipelineLayout, 0, m_pBlurGaussianDescriptorSet[i][j]);
				pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
				pCommandBuffer->Draw(4, 1, 0, 0);
			}

			pCommandBuffer->EndRenderPass();
		}

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// 合成
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Composite", color));

		Composite2Constant comp2Constant;
		comp2Constant.color0.Set(1.0f, 1.0f, 1.0f, 0.0f);
		comp2Constant.color1.Set(1.0f, 1.0f, 1.0f, 0.0f);

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->PushConstant(m_pComposite2PipelineLayout, 0, &comp2Constant);
		pCommandBuffer->BeginRenderPass(m_pCompositeFrameBuffer, true);
		pCommandBuffer->BindPipeline(m_Composite2PipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pComposite2PipelineLayout, 0, m_pBloomComposite2DescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
		pCommandBuffer->EndRenderPass();

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());

		// ----------------------------------------------------------------------------------------------------
		// 後処理
		// ----------------------------------------------------------------------------------------------------

		DEBUG_MARKER_CODE(pCommandBuffer->BeginDebugMarker("Cleanup", color));

		// ブラーのイメージをカラーアタッチメントの書き込みアクセスへ移行
		static constexpr V3DImageViewMemoryBarrier memoryBarrierOne =
		{
			V3D_ACCESS_SHADER_READ,
			V3D_ACCESS_COLOR_ATTACHMENT_WRITE,
			V3D_QUEUE_FAMILY_IGNORED,
			V3D_QUEUE_FAMILY_IGNORED,
			V3D_IMAGE_LAYOUT_SHADER_READ_ONLY,
			V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT,
			nullptr,
		};

		static V3DImageViewMemoryBarrier memoryBarriers[2] =
		{
			memoryBarrierOne,
			memoryBarrierOne,
		};

		static uint32_t memoryBarrierCount = _countof(memoryBarriers);

		for (uint32_t i = 0; i < 2; i++)
		{
			m_pBlurFrameBuffer[i]->GetAttachment(0, &memoryBarriers[i].pImageView);
		}

		V3DPipelineBarrier pipelineBarrier{};
		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_FRAGMENT_SHADER;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;

		pCommandBuffer->Barrier(pipelineBarrier, memoryBarrierCount, memoryBarriers);

		for (uint32_t i = 0; i < memoryBarrierCount; i++)
		{
			SAFE_RELEASE(memoryBarriers[i].pImageView);
		}

		DEBUG_MARKER_CODE(pCommandBuffer->EndDebugMarker());
	}

	void DrawImageEffect(IV3DCommandBuffer* pCommandBuffer)
	{
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		uint32_t page = 0;

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(m_pImageEffectFrameBuffer, true);

		// 補正 ( ガンマ、コントラスト )
		DrawImageEffect_Correction(pCommandBuffer, page++);

		// トーンマッピング ( HDR to LDR )
		pCommandBuffer->NextSubpass();
		DrawImageEffect_ToneMapping(pCommandBuffer, page++);

		// アンチエイリアシング ( FXAA )
		pCommandBuffer->NextSubpass();
		DrawImageEffect_Fxaa(pCommandBuffer, page++);

		pCommandBuffer->EndRenderPass();
	}

	void DrawImageEffect_Copy(IV3DCommandBuffer* pCommandBuffer, uint32_t page)
	{
		pCommandBuffer->BindPipeline(m_PeCopyPipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_CopyPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
	}

	void DrawImageEffect_Correction(IV3DCommandBuffer* pCommandBuffer, uint32_t page)
	{
		pCommandBuffer->PushConstant(m_pIE_CorrectionPipelineLayout, 0, &m_CorrectionConstant);
		pCommandBuffer->BindPipeline(m_PeCorrectionPipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_CorrectionPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
	}

	void DrawImageEffect_ToneMapping(IV3DCommandBuffer* pCommandBuffer, uint32_t page)
	{
		uint64_t simpleVertexOffset = 0;

		if (m_ToneMappingEnable == true)
		{
			pCommandBuffer->BindPipeline(m_PeToneMappingPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_ToneMappingPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		}
		else
		{
			pCommandBuffer->BindPipeline(m_PeCopyPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_CopyPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		}

		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
	}

	void DrawImageEffect_Fxaa(IV3DCommandBuffer* pCommandBuffer, uint32_t page)
	{
		if (m_FxaaEnable == true)
		{
			float invWidth = 1.0f / TO_F32(m_DrawGeometryParallelData.viewport.rect.width);
			float invHeight = 1.0f / TO_F32(m_DrawGeometryParallelData.viewport.rect.height);

			m_PeFxaaConstant.texelOffset0.x = -invWidth;
			m_PeFxaaConstant.texelOffset0.y = -invHeight;
			m_PeFxaaConstant.texelOffset0.z = +invWidth;
			m_PeFxaaConstant.texelOffset0.w = -invHeight;
			m_PeFxaaConstant.texelOffset1.x = -invWidth;
			m_PeFxaaConstant.texelOffset1.y = +invHeight;
			m_PeFxaaConstant.texelOffset1.z = +invWidth;
			m_PeFxaaConstant.texelOffset1.w = +invHeight;
			m_PeFxaaConstant.invTexSize.x = invWidth;
			m_PeFxaaConstant.invTexSize.y = invHeight;

			pCommandBuffer->PushConstant(m_pIE_FxaaPipelineLayout, 0, &m_PeFxaaConstant);
			pCommandBuffer->BindPipeline(m_PeFxaaPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_FxaaPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		}
		else
		{
			pCommandBuffer->BindPipeline(m_PeCopyPipelineHandle->GetPtr());
			pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pIE_CopyPipelineLayout, 0, m_pImageEffectDescriptorSet[page]);
		}

		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);
	}

	void DrawFinish(IV3DCommandBuffer* pCommandBuffer)
	{
		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();
		uint32_t currentImageIndex = GetSwapChain()->GetCurrentImageIndex();
		IV3DFrameBuffer* pFrameBuffer = m_OverlayFrameBuffers[currentImageIndex];

		pCommandBuffer->SetViewport(0, 1, &m_DrawGeometryParallelData.viewport);
		pCommandBuffer->SetScissor(0, 1, &m_DrawGeometryParallelData.scissor);

		pCommandBuffer->BeginRenderPass(pFrameBuffer, true);

		// 初期化
		pCommandBuffer->BindPipeline(m_OverlayCopyPipelineHandle->GetPtr());
		pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, m_pOverlayCopyPipelineLayout, 0, m_pOverlayCopyDescriptorSet);
		pCommandBuffer->BindVertexBuffer(0, m_pSimpleVertexBuffer);
		pCommandBuffer->Draw(4, 1, 0, 0);

		// オーバーレイ
		pCommandBuffer->NextSubpass();
		m_Font.Flush(pCommandBuffer);

		pCommandBuffer->EndRenderPass();

		// ----------------------------------------------------------------------------------------------------
		// クリーンアップ
		// ----------------------------------------------------------------------------------------------------

		pCommandBuffer->Barrier(m_pFinishBarrierSet);

		IV3DImageView* pBackBufferView;
		pFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER, &pBackBufferView);

		V3DPipelineBarrier pipelineBarrier{};
		pipelineBarrier.srcStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		pipelineBarrier.dstStageMask = V3D_PIPELINE_STAGE_BOTTOM_OF_PIPE;

		V3DImageViewMemoryBarrier memoryBarrier{};
		memoryBarrier.srcAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		memoryBarrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;// V3D_ACCESS_MEMORY_READ;
		memoryBarrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		memoryBarrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		memoryBarrier.srcLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		memoryBarrier.dstLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		memoryBarrier.pImageView = pBackBufferView;

		pCommandBuffer->Barrier(pipelineBarrier, memoryBarrier);

		SAFE_RELEASE(pBackBufferView);
	}

#ifdef ENABLE_MULTITHREAD
	static void __stdcall UpdateSubsetParallel(uint32_t thread, uint32_t page, uint32_t first, uint32_t count, void* pData)
	{
		UpdateParallelData* pParallelData = static_cast<UpdateParallelData*>(pData);
		UpdateThreadData* pThreadData = &pParallelData->threadDatum[page];

		UpdateSubsetSingle((*pParallelData->pMeshes)[0], first, count, &pParallelData->frustum, pParallelData->shadowEnable, &pThreadData->geometryObjects, &pThreadData->shadowObjects);
	}

	static void __stdcall UpdateNodeParallel(uint32_t thread, uint32_t page, uint32_t first, uint32_t count, void* pData)
	{
		UpdateParallelData* pParallelData = static_cast<UpdateParallelData*>(pData);
		UpdateThreadData* pThreadData = &pParallelData->threadDatum[page];

		UpdateNodeSingle((*pParallelData->pMeshes)[0], first, count, &pParallelData->frustum, pParallelData->shadowEnable, &pThreadData->geometryObjects, &pThreadData->shadowObjects);
	}

	static void __stdcall DrawObjectParallel(uint32_t thread, uint32_t page, uint32_t first, uint32_t count, void* pData)
	{
		DrawParallelData* pParallelData = static_cast<DrawParallelData*>(pData);
		IV3DCommandBuffer* pCommandBuffer = pParallelData->commandBuffers[pParallelData->frame][thread];

//		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallelData->pRenderPass, 0, pParallelData->pFrameBuffer);

		DrawObjectSingle(pCommandBuffer, pParallelData->sortDrawObjects, &pParallelData->constant, &pParallelData->viewport, &pParallelData->scissor, pParallelData->pDrawObjects, first, count);

		pParallelData->test[thread] += count;

//		pCommandBuffer->End();
	}
#endif //ENABLE_MULTITHREAD

	static void UpdateSubsetSingle(SkeletalMeshPtr mesh, uint32_t first, uint32_t count, Frustum* pFrustum, bool shadowEnable, std::vector<DrawObject>* pGeometryObjects, std::vector<DrawObject>* pShadowObjects)
	{
		uint64_t meshKey = mesh->GetKey();

		SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(0);

		for (uint32_t i = first; i < (first + count); i++)
		{
			const MeshSubset& subset = meshNode->GetSubset(i);
			MaterialPtr material = mesh->GetMaterial(subset.material);

			pGeometryObjects->push_back(DrawObject{});
			DrawObject& drawObject = pGeometryObjects->back();

			drawObject.pPipelineLayout = material->GetPipelineLayoutPtr(RENDER_TYPE_GEOMETRY);
			drawObject.pPipeline = material->GetPipelinePtr(RENDER_TYPE_GEOMETRY);

			drawObject.materialKey = material->GetKey();
			drawObject.pMaterialDescriptorSet = material->GetDescriptorSetPtr();
			drawObject.materialDynamicOffset = material->GetDynamicOffset();

			drawObject.meshKey = meshKey;
			drawObject.pMeshDescriptorSet = meshNode->GetDescriptorSetPtr();
			drawObject.meshDynamicOffset = meshNode->GetDynamicOffset();
			drawObject.pMeshBuffer = mesh->GetVertexIndexBuffer();
			drawObject.meshVertexOffset = mesh->GetVertexOffset();
			drawObject.meshIndexOffset = mesh->GetIndexOffset();
			drawObject.meshIndexType = mesh->GetIndexType();
			drawObject.meshSubset = subset;
		}

		if (shadowEnable == true)
		{
			for (uint32_t i = first; i < (first + count); i++)
			{
				const MeshSubset& subset = meshNode->GetSubset(i);
				MaterialPtr material = mesh->GetMaterial(subset.material);

				pShadowObjects->push_back(DrawObject());
				DrawObject& drawObject = pShadowObjects->back();

				drawObject.pPipelineLayout = material->GetPipelineLayoutPtr(RENDER_TYPE_SHADOW);
				drawObject.pPipeline = material->GetPipelinePtr(RENDER_TYPE_SHADOW);

				drawObject.materialKey = material->GetKey();
				drawObject.pMaterialDescriptorSet = material->GetDescriptorSetPtr();
				drawObject.materialDynamicOffset = material->GetDynamicOffset();

				drawObject.meshKey = meshKey;
				drawObject.pMeshDescriptorSet = meshNode->GetDescriptorSetPtr();
				drawObject.meshDynamicOffset = meshNode->GetDynamicOffset();
				drawObject.pMeshBuffer = mesh->GetVertexIndexBuffer();
				drawObject.meshVertexOffset = mesh->GetVertexOffset();
				drawObject.meshIndexOffset = mesh->GetIndexOffset();
				drawObject.meshIndexType = mesh->GetIndexType();
				drawObject.meshSubset = subset;
			}
		}
	}

	static void UpdateNodeSingle(SkeletalMeshPtr mesh, uint32_t first, uint32_t count, Frustum* pFrustum, bool shadowEnable, std::vector<DrawObject>* pGeometryObjects, std::vector<DrawObject>* pShadowObjects)
	{
		uint64_t meshKey = mesh->GetKey();

		for (uint32_t i = first; i < (first + count); i++)
		{
			SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);
			uint32_t subsetCount = meshNode->GetSubsetCount();

			if (pFrustum->Contains(meshNode->GetBounds().aabb) == true)
			{
				for (uint32_t j = 0; j < subsetCount; j++)
				{
					const MeshSubset& subset = meshNode->GetSubset(j);
					MaterialPtr material = mesh->GetMaterial(subset.material);

					pGeometryObjects->push_back(DrawObject{});
					DrawObject& drawObject = pGeometryObjects->back();

					drawObject.pPipelineLayout = material->GetPipelineLayoutPtr(RENDER_TYPE_GEOMETRY);
					drawObject.pPipeline = material->GetPipelinePtr(RENDER_TYPE_GEOMETRY);

					drawObject.materialKey = material->GetKey();
					drawObject.pMaterialDescriptorSet = material->GetDescriptorSetPtr();
					drawObject.materialDynamicOffset = material->GetDynamicOffset();

					drawObject.meshKey = meshKey;
					drawObject.pMeshDescriptorSet = meshNode->GetDescriptorSetPtr();
					drawObject.meshDynamicOffset = meshNode->GetDynamicOffset();
					drawObject.pMeshBuffer = mesh->GetVertexIndexBuffer();
					drawObject.meshVertexOffset = mesh->GetVertexOffset();
					drawObject.meshIndexOffset = mesh->GetIndexOffset();
					drawObject.meshIndexType = mesh->GetIndexType();
					drawObject.meshSubset = subset;
				}
			}
		}

		if (shadowEnable == true)
		{
			for (uint32_t i = first; i < (first + count); i++)
			{
				SkeletalMeshNodePtr meshNode = mesh->GetMeshNode(i);
				uint32_t subsetCount = meshNode->GetSubsetCount();

				if (pFrustum->Contains(meshNode->GetBounds().aabb) == true)
				{
					for (uint32_t j = 0; j < subsetCount; j++)
					{
						const MeshSubset& subset = meshNode->GetSubset(j);
						MaterialPtr material = mesh->GetMaterial(subset.material);

						pShadowObjects->push_back(DrawObject());
						DrawObject& drawObject = pShadowObjects->back();

						drawObject.pPipelineLayout = material->GetPipelineLayoutPtr(RENDER_TYPE_SHADOW);
						drawObject.pPipeline = material->GetPipelinePtr(RENDER_TYPE_SHADOW);

						drawObject.materialKey = material->GetKey();
						drawObject.pMaterialDescriptorSet = material->GetDescriptorSetPtr();
						drawObject.materialDynamicOffset = material->GetDynamicOffset();

						drawObject.meshKey = meshKey;
						drawObject.pMeshDescriptorSet = meshNode->GetDescriptorSetPtr();
						drawObject.meshDynamicOffset = meshNode->GetDynamicOffset();
						drawObject.pMeshBuffer = mesh->GetVertexIndexBuffer();
						drawObject.meshVertexOffset = mesh->GetVertexOffset();
						drawObject.meshIndexOffset = mesh->GetIndexOffset();
						drawObject.meshIndexType = mesh->GetIndexType();
						drawObject.meshSubset = subset;
					}
				}
			}
		}
	}

	static void DrawObjectSingle(IV3DCommandBuffer* pCommandBuffer, bool sortDrawObject, const SceneConstant* pConstant, const V3DViewport* pViewport, const V3DRectangle2D* pScissor, std::vector<DrawObject>* pDrawObjects, size_t first, size_t count)
	{
		if (count == 0)
		{
			return;
		}

		if (sortDrawObject == true)
		{
			std::vector<DrawObject>::iterator it_begin = pDrawObjects->begin() + first;
			std::vector<DrawObject>::iterator it_end = it_begin + count;

			std::sort(it_begin, it_end, [](const DrawObject& lh, const DrawObject& rh) { return lh.materialKey > rh.materialKey; });
		}

		DrawObject* pDrawObject = &(*pDrawObjects)[first];
		DrawObject* pDrawObjectEnd = pDrawObject + count;

		IV3DPipeline* pPrePipeline = nullptr;
		uint64_t preMaterialKey = 0;
		uint64_t preMeshKey = 0;

		pCommandBuffer->PushConstant(pDrawObject->pPipelineLayout, 0, pConstant);

		pCommandBuffer->SetViewport(0, 1, pViewport);
		pCommandBuffer->SetScissor(0, 1, pScissor);

		uint32_t dynOffsets[2];
		IV3DDescriptorSet* descSets[2];

		while (pDrawObject != pDrawObjectEnd)
		{
			const MeshSubset& meshSubset = pDrawObject->meshSubset;

			if (pDrawObject->pPipeline != pPrePipeline)
			{
				pCommandBuffer->BindPipeline(pDrawObject->pPipeline);
				pPrePipeline = pDrawObject->pPipeline;
			}

			descSets[0] = pDrawObject->pMeshDescriptorSet;

			if (pDrawObject->materialKey != preMaterialKey)
			{
				descSets[0] = pDrawObject->pMeshDescriptorSet;
				descSets[1] = pDrawObject->pMaterialDescriptorSet;

				dynOffsets[0] = pDrawObject->meshDynamicOffset;
				dynOffsets[1] = pDrawObject->materialDynamicOffset;

				pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, pDrawObject->pPipelineLayout, 0, 2, descSets, 2, dynOffsets);
//				pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, pDrawObject->pPipelineLayout, 0, pDrawObject->pMeshDescriptorSet, 1, &pDrawObject->meshDynamicOffset);
//				pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, pDrawObject->pPipelineLayout, 1, pDrawObject->pMaterialDescriptorSet, 1, &pDrawObject->materialDynamicOffset);

				preMaterialKey = pDrawObject->materialKey;
			}
			else
			{
				pCommandBuffer->BindDescriptorSet(V3D_PIPELINE_TYPE_GRAPHICS, pDrawObject->pPipelineLayout, 0, pDrawObject->pMeshDescriptorSet, 1, &pDrawObject->meshDynamicOffset);
			}

			if (pDrawObject->meshKey != preMeshKey)
			{
//				pCommandBuffer->BindVertexBuffers(0, 1, &pDrawObject->pMeshBuffer, &pDrawObject->meshVertexOffset);
				pCommandBuffer->BindVertexBuffer(0, pDrawObject->pMeshBuffer, pDrawObject->meshVertexOffset);
				pCommandBuffer->BindIndexBuffer(pDrawObject->pMeshBuffer, pDrawObject->meshIndexOffset, pDrawObject->meshIndexType);
				preMeshKey = pDrawObject->meshKey;
			}

			pCommandBuffer->DrawIndexed(meshSubset.indexCount, 1, meshSubset.firstIndex, 0, 0);

			pDrawObject++;
		}
	}

private:
	struct UpdateThreadData
	{
		std::vector<DrawObject> geometryObjects;
		std::vector<DrawObject> shadowObjects;
	};

	struct UpdateParallelData
	{
		bool shadowEnable;

		Frustum frustum;
		std::vector<SkeletalMeshPtr>* pMeshes;

		std::vector<UpdateThreadData> threadDatum;
	};

	struct DrawParallelData
	{
		bool sortDrawObjects;

		uint32_t frame;
		std::vector<DrawObject>* pDrawObjects;

		SceneConstant constant;

		IV3DRenderPass* pRenderPass;
		IV3DFrameBuffer* pFrameBuffer;

		V3DViewport viewport;
		V3DRectangle2D scissor;

		std::vector<std::vector<IV3DCommandBuffer*>> commandBuffers;

		uint32_t test[4];
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

	IV3DBuffer* m_pSimpleVertexBuffer = nullptr;
	IV3DSampler* m_pSimpleSampler = nullptr;
	IV3DSampler* m_pDownSampler = nullptr;

	std::vector<SkeletalMeshPtr> m_Meshes;
	SkyDome m_SkyDome;

	UpdateParallelData m_UpdateParallelData = {};
	DrawParallelData m_DrawGeometryParallelData = {};
	DrawParallelData m_DrawShadowParallelData = {};

	Quaternion m_CameraRot;
	float m_CameraDistance = 10.0f;
	Vector3 m_CameraAt;
	Camera m_Camera;

	bool m_bDrag = false;
	POINT m_MousePos = {};

	float m_CameraMoveCoiffe = 10.0f;
	float m_CameraRotCoiffe = 1.0f;

	IV3DQueryPool* m_pQueryPool = nullptr;
	bool m_bQueryActive = false;
	bool m_bQueryEnable = false;

	/****************/
	/* ブライトパス */
	/****************/

	BrightPassConstant m_BrightPassConstant = {};
	GraphicsRenderPassHandle m_BrightPassRenderPassHandle = nullptr;
	GraphicsPipelineHandle m_BrightPassPipelineHandle = nullptr;
	IV3DPipelineLayout* m_pBrightPassPipelineLayout = nullptr;
	IV3DDescriptorSet* m_pBrightPassDescriptorSet = nullptr;
	IV3DFrameBuffer* m_pBrightPassFrameBuffer = nullptr;

	/**********/
	/* ブラー */
	/**********/

	GraphicsRenderPassHandle m_BlurRenderPassHandle = nullptr;

	// ダウンサンプリング
	IV3DPipelineLayout* m_pBlurDawnSamplingPipelineLayout = nullptr;
	GraphicsPipelineHandle m_BlurDawnSamplingPipelineHandle = nullptr;

	// ガウシアン
	IV3DPipelineLayout* m_pBlurGaussianPipelineLayout = nullptr;
	Array1<IV3DFrameBuffer*, 2> m_pBlurFrameBuffer;
	Array1<GraphicsPipelineHandle, 2> m_BlurGaussianPipelineHandle;
	Array2<IV3DDescriptorSet*, 2, 2> m_pBlurGaussianDescriptorSet;

	/********/
	/* 合成 */
	/********/

	GraphicsRenderPassHandle m_CompositeRenderPassHandle = nullptr;
	IV3DFrameBuffer* m_pCompositeFrameBuffer = nullptr;

	// ２枚
	IV3DPipelineLayout* m_pComposite2PipelineLayout = nullptr;
	GraphicsPipelineHandle m_Composite2PipelineHandle = nullptr;

	/**************/
	/* ジオメトリ */
	/**************/

	GraphicsRenderPassHandle m_GeometryRenderPassHandle = nullptr;
	IV3DPipelineLayout* m_pGeometryPipelineLayout = nullptr;
	IV3DFrameBuffer* m_pGeometryFrameBuffer = nullptr;

	/********/
	/* 遠景 */
	/********/

	GraphicsRenderPassHandle m_DistantViewRenderPassHandle = nullptr;
	IV3DFrameBuffer* m_pDistantViewFrameBuffer = nullptr;

	/*********/
	/* SSAO */
	/*********/

	bool m_SsaoEnable = true;

	float m_SsaoRadius = 0.05f;
	float m_SsaoThreshold = 0.02f;
	float m_SsaoDist = 0.3f;
	float m_SsaoIntensity = 1.5f;

	float m_SsaoBlurRadius = 10.0f;
	uint32_t m_SsaoBlurSamplingCount = 8;

	IV3DImageView* m_pSSAONoiseImage = nullptr;
	GraphicsRenderPassHandle m_SsaoRenderPassHandle = nullptr;
	GraphicsPipelineHandle m_SsaoPipelineHandle = nullptr;
	IV3DFrameBuffer* m_pSsaoFrameBuffer = nullptr;
	IV3DPipelineLayout* m_pSsaoPipelineLayout = nullptr;
	IV3DDescriptorSet* m_pSsaoDescriptorSet = nullptr;
	IV3DSampler* m_pSsaoSampler = nullptr;

	IV3DDescriptorSet* m_pSsaoBlurDawnSamplingDescriptorSet = nullptr;

	/************/
	/* シャドウ */
	/************/

	GraphicsRenderPassHandle m_ShadowRenderPassHandle = nullptr;
	IV3DPipelineLayout* m_pShadowPipelineLayout = nullptr;
	IV3DFrameBuffer* m_pShadowFrameBuffer = nullptr;

	/****************/
	/* ライティング */
	/****************/

	GraphicsRenderPassHandle m_LightingRenderPassHandle = nullptr;
	IV3DFrameBuffer* m_pLightingFrameBuffer = nullptr;
	DirectionalLightingUniform m_DirectionalLightUniform = {};
	LightingStage m_DirectionalLightingStage = {};
	LightingStage m_PointLightingStage = {};
	LightingStage m_FinishLightingStage = {};

	/************/
	/* ブルーム */
	/************/

	bool m_BloomEnable = true;
	float m_BloomBlurRadius = 100.0f;
	uint32_t m_BloomBlurSamplingCount = 16;

	Array1<IV3DDescriptorSet*, 2> m_pBloomBlurDawnSamplingDescriptorSet;
	IV3DDescriptorSet* m_pBloomComposite2DescriptorSet = nullptr;

	/**********************/
	/* イメージエフェクト */
	/**********************/

	GraphicsRenderPassHandle m_ImageEffectRenderPassHandle = nullptr;
	IV3DFrameBuffer* m_pImageEffectFrameBuffer;
	Array1<IV3DDescriptorSet*, 3> m_pImageEffectDescriptorSet;

	// コピー
	IV3DPipelineLayout* m_pIE_CopyPipelineLayout = nullptr;
	GraphicsPipelineHandle m_PeCopyPipelineHandle = nullptr;

	// 補正
	CorrectionConstant m_CorrectionConstant = {};
	IV3DPipelineLayout* m_pIE_CorrectionPipelineLayout = nullptr;
	GraphicsPipelineHandle m_PeCorrectionPipelineHandle = nullptr;

	// トーンマッピング
	bool m_ToneMappingEnable = true;
	IV3DPipelineLayout* m_pIE_ToneMappingPipelineLayout = nullptr;
	GraphicsPipelineHandle m_PeToneMappingPipelineHandle = nullptr;

	// FXAA
	bool m_FxaaEnable = true;
	FxaaConstant m_PeFxaaConstant = {};
	IV3DPipelineLayout* m_pIE_FxaaPipelineLayout = nullptr;
	GraphicsPipelineHandle m_PeFxaaPipelineHandle = nullptr;

	/****************/
	/* オーバーレイ */
	/****************/

	GraphicsRenderPassHandle m_OverlayRenderPassHandle = nullptr;
	std::vector<IV3DFrameBuffer*> m_OverlayFrameBuffers;

	// コピー
	GraphicsPipelineHandle m_OverlayCopyPipelineHandle = nullptr;
	IV3DPipelineLayout* m_pOverlayCopyPipelineLayout = nullptr;
	IV3DDescriptorSet* m_pOverlayCopyDescriptorSet = nullptr;

	IV3DBarrierSet* m_pFinishBarrierSet = nullptr;

	void BeginParallelCommandBuffer(const DrawParallelData* pData)
	{
		const std::vector<IV3DCommandBuffer*>& commandBuffer = pData->commandBuffers[GetSwapChain()->GetCurrentImageIndex()];

		std::vector<IV3DCommandBuffer*>::const_iterator it_begin = commandBuffer.begin();
		std::vector<IV3DCommandBuffer*>::const_iterator it_end = commandBuffer.end();
		std::vector<IV3DCommandBuffer*>::const_iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			(*it)->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pData->pRenderPass, 0, pData->pFrameBuffer);
		}
	}

	void EndParallelCommandBuffer(const DrawParallelData* pData)
	{
		const std::vector<IV3DCommandBuffer*>& commandBuffer = pData->commandBuffers[GetSwapChain()->GetCurrentImageIndex()];

		std::vector<IV3DCommandBuffer*>::const_iterator it_begin = commandBuffer.begin();
		std::vector<IV3DCommandBuffer*>::const_iterator it_end = commandBuffer.end();
		std::vector<IV3DCommandBuffer*>::const_iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			(*it)->End();
		}
	}
};

class TestApp : public Application
{
public:
	TestApp() {}
	virtual ~TestApp() {}

	virtual void OnPreCreate(ApplicationDesc& desc) override
	{
#ifdef DEBUG_MARKER_ENABLE

//		desc.layer = V3D_LAYER_RENDERDOC;
		desc.layer = V3D_LAYER_NSIGHT;

#else //DEBUG_MARKER_ENABLE

#ifdef _DEBUG
		desc.layer = V3D_LAYER_VALIDATION;
//		desc.layer = V3D_LAYER_OPTIMAL;
#else //_DEBUG
		desc.layer = V3D_LAYER_OPTIMAL;
#endif //_DEBUG

#endif //DEBUG_MARKER_ENABLE

		desc.fps = 0;
	}

	virtual bool OnInitialize() override
	{
		::srand((unsigned)time(nullptr));

//		uint32_t queueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 2);
		uint32_t queueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 1);
		if (queueFamily == ~0U)
		{
			return false;
		}

		IV3DQueue* pWorkQueue;
		IV3DQueue* pGraphicsQueue;

		Application::GetDevice()->GetQueue(queueFamily, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(queueFamily, 0, &pGraphicsQueue);
//		Application::GetDevice()->GetQueue(queueFamily, 1, &pGraphicsQueue);

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
