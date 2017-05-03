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

// ----------------------------------------------------------------------------------------------------

static auto constexpr SCREEN_WIDTH = 1024;
static auto constexpr SCREEN_HEIGHT = 768;
static auto constexpr BUFFER_COUNT = 2;

static auto constexpr MESH_SIDE_COUNT = 20;
static auto constexpr POINT_LIGHT_SIDE_COUNT = 3;

static auto constexpr MATERIAL_MAX = 100;
static auto constexpr MESH_MAX = MESH_SIDE_COUNT * MESH_SIDE_COUNT * MESH_SIDE_COUNT;
static auto constexpr MESH_COUNT = MESH_MAX;
static auto constexpr POINT_LIGHT_COUNT = POINT_LIGHT_SIDE_COUNT * POINT_LIGHT_SIDE_COUNT * POINT_LIGHT_SIDE_COUNT;


static constexpr uint32_t SPACE_SIZE_I = 50;
static constexpr uint32_t SPACE_ACC_I = 10;
static constexpr uint32_t SPACE_SIZE_ACC_I = SPACE_SIZE_I * SPACE_ACC_I;
static constexpr float SPACE_HALF_SIZE = static_cast<float>(SPACE_SIZE_I) * 0.5f;
static constexpr float SPACE_ACC = static_cast<float>(SPACE_ACC_I);

#define ENABLE_MULTITHREAD

// ----------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------

class TestWindow : public Window
{
public:
	TestWindow() :
		m_RenderPassHandle(nullptr),
		m_pSimpleVertexBuffer(nullptr),
		m_pSimpleSampler(nullptr),
		m_ParallelData({}),
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
		// いろいろ初期化
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

		if (m_MaterialManager.Initialize(&m_GraphicsManager, MATERIAL_MAX) != V3D_OK)
		{
			return false;
		}

		if (m_MeshManager.Initialize(&m_GraphicsManager, &m_TextureManager, &m_MaterialManager, MESH_MAX) != V3D_OK)
		{
			return false;
		}

		if (m_Font.Initialize(Application::GetDevice(), L"Arial", 16, FW_NORMAL) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// レンダーパスを取得
		// ----------------------------------------------------------------------------------------------------

		m_GraphicsManager.GetRenderPassHandle(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, &m_RenderPassHandle);

		// ----------------------------------------------------------------------------------------------------
		// グラフィックスパイプラインを作成
		// ----------------------------------------------------------------------------------------------------

		// ディレクショナルライティング
		{
			GraphicsPipelienDesc desc;
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_ADDITION;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
				GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING,
				desc,
				&m_DirectionalLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}

		}

		// ポイントライティング
		{
			GraphicsPipelienDesc desc;
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_ADDITION;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
				GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING,
				desc,
				&m_PointLightingStage.pipelineHandle) != V3D_OK)
			{
				return false;
			}
		}

		// ライティング合成
		{
			GraphicsPipelienDesc desc;
			desc.polygonMode = V3D_POLYGON_MODE_FILL;
			desc.cullMode = V3D_CULL_MODE_NONE;
			desc.depthTestEnable = false;
			desc.depthWriteEnable = false;
			desc.blendMode = BLEND_MODE_COPY;

			if (m_GraphicsManager.GetPipelineHandle(
				GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
				GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING,
				desc,
				&m_FinishLightingStage.pipelineHandle) != V3D_OK)
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
			samplerDesc.addressModeU = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.addressModeV = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.addressModeW = V3D_ADDRESS_MODE_REPEAT;
			samplerDesc.mipLodBias = 0.0f;
			samplerDesc.anisotropyEnable = false;
			samplerDesc.maxAnisotropy = 0.0f;
			samplerDesc.compareOp = V3D_COMPARE_OP_NEVER;
			samplerDesc.minLod = 0.0f;
			samplerDesc.maxLod = 1.0f;
			samplerDesc.compareEnable = false;
			samplerDesc.borderColor = V3D_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

			if (Application::GetDevice()->CreateSampler(samplerDesc, &m_pSimpleSampler) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// メッシュを作成
		// ----------------------------------------------------------------------------------------------------

		{
//			float scale = 1.0f;
//			bool flipFace = true;
//			bool invertTexV = true;
//			bool invertNormal = true;
			float scale = 0.02f;
			bool flipFace = false;
			bool invertTexV = false;
			bool invertNormal = true;

			Matrix4x4 scaleMat;
			scaleMat.SetScale(Vector3(scale, scale, scale));

			Mesh::LoadDesc meshLoadDesc{};
			meshLoadDesc.rotation = Matrix4x4(Quaternion(Vector3::ZAxis(), DEG_TO_RAD(180.0f))) * scaleMat;
			meshLoadDesc.invertTex[0] = false;
			meshLoadDesc.invertTex[1] = invertTexV;
			meshLoadDesc.invertNormal = invertNormal;
			meshLoadDesc.flipFace = flipFace;
			meshLoadDesc.isSmoosing = true;
			meshLoadDesc.smoosingAngle = DEG_TO_RAD(80.0f);

			MeshPtr mesh = m_MeshManager.Load(L"data\\mesh\\teapot.obj", meshLoadDesc);
			if (mesh == nullptr)
			{
				return false;
			}

			mesh->GetMaterial(0)->SetCullMode(V3D_CULL_MODE_NONE);
			mesh->GetMaterial(0)->Update();

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
				const float meshInterval = 5.0f;
				const float meshBasePos = -(meshInterval * MESH_SIDE_COUNT) / 2.0f;

				uint32_t meshIndex = 0;

				for (int32_t x = 0; x < MESH_SIDE_COUNT; x++)
				{
					float wx = meshBasePos +  meshInterval * TO_F32(x);

					for (int32_t y = 0; y < MESH_SIDE_COUNT; y++)
					{
						float wy = meshBasePos + meshInterval * TO_F32(y);

						for (int32_t z = 0; z < MESH_SIDE_COUNT; z++)
						{
							float wz = meshBasePos + meshInterval * TO_F32(z);

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
		// サブパスで使用するユニフォームバッファを作成
		// ----------------------------------------------------------------------------------------------------

		// ディレクショナルライティング
		{
			V3DBufferDesc bufferDesc;
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
			bufferDesc.size = sizeof(DirectionalLightingUniform);

			if (Application::GetDevice()->CreateBuffer(bufferDesc, &m_DirectionalLightingStage.pUniformBuffer) != V3D_OK)
			{
				return false;
			}
		}

		// ポイントライティング
		{
			PointLightingUniform uniform{};
			uniform.pointLightCount = POINT_LIGHT_COUNT;

			const float interval = 50.0f;
			const float basePos = -(interval * 3) / 2.0f;

			uint32_t lightIndex = 0;

			for (int32_t x = 0; x < POINT_LIGHT_SIDE_COUNT; x++)
			{
				float wx = basePos + interval * TO_F32(x);

				for (int32_t y = 0; y < POINT_LIGHT_SIDE_COUNT; y++)
				{
					float wy = basePos + interval * TO_F32(y);

					for (int32_t z = 0; z < POINT_LIGHT_SIDE_COUNT; z++)
					{
						float wz = basePos + interval * TO_F32(z);

						PointLight& pointLight = uniform.pointLights[lightIndex];

						float r = static_cast<float>(rand() % 100) / 100.0f;
						float g = static_cast<float>(rand() % 100) / 100.0f;
						float b = static_cast<float>(rand() % 100) / 100.0f;
						float dist = static_cast<float>(rand() % 100) / 10.0f + 100.0f;

						pointLight.color.Set(r, g, b, 1.0f);
						pointLight.posAndDist.Set(wx, wy, wz, FLOAT_RECIPROCAL(dist));

						lightIndex++;
					}
				}
			}

			V3DBufferDesc bufferDesc;
			bufferDesc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
			bufferDesc.size = sizeof(PointLightingUniform);

			if (Application::GetDevice()->CreateBuffer(bufferDesc, &m_PointLightingStage.pUniformBuffer) != V3D_OK)
			{
				return false;
			}
		}

		// メモリにバインド
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

		// ----------------------------------------------------------------------------------------------------
		// ディレクショナルライティング
		// ----------------------------------------------------------------------------------------------------

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING, &m_DirectionalLightingStage.pPipelineLayout);

		// ユニフォームバッファーを更新
		{
			void* pMemory;
			if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(DirectionalLightingUniform), &pMemory) == V3D_OK)
			{
				DirectionalLightingUniform uniform{};
				uniform.directionalLightCount = 1;
				uniform.directionalLights[0].color.Set(0.5f, 0.6f, 0.7f, 1.0f);
				//uniform.directionalLights[0].color.Set(1.0f, 1.0f, 1.0f, 1.0f);
				uniform.directionalLights[0].dir.Set(0.0f, -1.0f, 0.0f);

				memcpy_s(pMemory, sizeof(DirectionalLightingUniform), &uniform, sizeof(DirectionalLightingUniform));

				m_DirectionalLightingStage.pUniformBuffer->Unmap();
			}
		}

		// デスクリプタセットを作成
		if (m_GraphicsManager.CreateDescriptorSet(
			GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
			GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING,
			GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING,
			&m_DirectionalLightingStage.pDescriptorSet) != V3D_OK)
		{
			return false;
		}

		// イメージの設定、更新は OnRestoreSwapChain で行う
		m_DirectionalLightingStage.pDescriptorSet->SetBuffer(0, m_DirectionalLightingStage.pUniformBuffer, 0, sizeof(DirectionalLightingUniform));

		// ----------------------------------------------------------------------------------------------------
		// ポイントライティング
		// ----------------------------------------------------------------------------------------------------

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING, &m_PointLightingStage.pPipelineLayout);

		// ユニフォームバッファを更新
		{
			void* pMemory;
			if (m_PointLightingStage.pUniformBuffer->Map(0, sizeof(PointLightingUniform), &pMemory) == V3D_OK)
			{
				PointLightingUniform uniform{};
				uniform.pointLightCount = POINT_LIGHT_COUNT;

				for (uint32_t i = 0; i < POINT_LIGHT_COUNT; i++)
				{
					PointLight& pointLight = uniform.pointLights[i];

					float r = static_cast<float>(rand() % 100) / 100.0f;
					float g = static_cast<float>(rand() % 100) / 100.0f;
					float b = static_cast<float>(rand() % 100) / 100.0f;

					pointLight.color.Set(r, g, b, 1.0f);
					//pointLight.color.Set(0.0f, 1.0f, 0.0f, 1.0f);

					float x = static_cast<float>(rand() % SPACE_SIZE_ACC_I) / SPACE_ACC - SPACE_HALF_SIZE;
					float y = static_cast<float>(rand() % SPACE_SIZE_ACC_I) / SPACE_ACC - SPACE_HALF_SIZE;
					float z = static_cast<float>(rand() % SPACE_SIZE_ACC_I) / SPACE_ACC - SPACE_HALF_SIZE;
					float dist = static_cast<float>(rand() % 100) / 10.0f + 5.0f;

					pointLight.posAndDist.Set(x, y, z, FLOAT_RECIPROCAL(dist));
				}

				memcpy_s(pMemory, sizeof(PointLightingUniform), &uniform, sizeof(PointLightingUniform));

				m_PointLightingStage.pUniformBuffer->Unmap();
			}
		}

		// デスクリプタセットを作成
		if (m_GraphicsManager.CreateDescriptorSet(
			GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
			GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING,
			GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING,
			&m_PointLightingStage.pDescriptorSet) != V3D_OK)
		{
			return false;
		}

		// イメージの設定、更新は OnRestoreSwapChain で行う
		m_PointLightingStage.pDescriptorSet->SetBuffer(0, m_PointLightingStage.pUniformBuffer, 0, sizeof(PointLightingUniform));

		// ----------------------------------------------------------------------------------------------------
		// フィニッシュライティング
		// ----------------------------------------------------------------------------------------------------

		// パイプラインレイアウトを取得
		m_GraphicsManager.GetPipelineLayout(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING, &m_FinishLightingStage.pPipelineLayout);

		// デスクリプタセットを作成
		if (m_GraphicsManager.CreateDescriptorSet(
			GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY,
			GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING,
			GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING,
			&m_FinishLightingStage.pDescriptorSet) != V3D_OK)
		{
			return false;
		}

		// イメージの設定、更新は OnRestoreSwapChain で行う
		;

		// ----------------------------------------------------------------------------------------------------
		// 並列データの初期化
		// ----------------------------------------------------------------------------------------------------

		m_ParallelData.pMeshes = &m_Meshes;

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

		SAFE_RELEASE(m_FinishLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_FinishLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_PointLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_PointLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_PointLightingStage.pPipelineLayout);

		SAFE_RELEASE(m_DirectionalLightingStage.pDescriptorSet);
		SAFE_RELEASE(m_DirectionalLightingStage.pUniformBuffer);
		SAFE_RELEASE(m_DirectionalLightingStage.pPipelineLayout);

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
		IV3DFrameBuffer* pFrameBuffer = m_pFrameBuffer[currentImageIndex];

		// ----------------------------------------------------------------------------------------------------
		// テキストを更新
		// ----------------------------------------------------------------------------------------------------

		std::wstringstream text;

		m_Font.Reset();

		text << L"Fps " << Application::GetFps();
		m_Font.AddText(16, 16, text.str().c_str());

//		const V3DDeviceStatistics& devStatistics = Application::GetDevice()->GetStatistics();
//		text.str(L"");
//		text << L"SamplerCount " << devStatistics.samplerCount << L"\n" << L"ResourceMemoryCount " << devStatistics.resourceMemoryCount;
//		m_Font.AddText(16, 64, text.str().c_str());

		// ----------------------------------------------------------------------------------------------------
		// シーンを更新
		// ----------------------------------------------------------------------------------------------------

		// カメラ
		float aspectRatio = (swapChainDesc.windowed == false) ? (16.0f / 9.0f) : (float)swapChainDesc.imageWidth / (float)swapChainDesc.imageHeight;

		m_Camera.SetProjection(DEG_TO_RAD(45.0f), aspectRatio, 0.1f, 100.0f);
		m_Camera.SetView(m_CameraRot, m_CameraAt, m_CameraDistance);
		m_Camera.Update();

		// ライト
		Vector3 eyeDir = m_Camera.GetEye().ToNormalize();

		void * pMemory;

		if (m_DirectionalLightingStage.pUniformBuffer->Map(0, sizeof(Vector3), &pMemory) == V3D_OK)
		{
			memcpy_s(pMemory, sizeof(Vector3), &eyeDir, sizeof(Vector3));

			m_DirectionalLightingStage.pUniformBuffer->Unmap();
		}

		if (m_PointLightingStage.pUniformBuffer->Map(0, sizeof(Vector4) + sizeof(Matrix4x4), &pMemory) == V3D_OK)
		{
			uint8_t* pAddr = static_cast<uint8_t*>(pMemory);

			memcpy_s(pAddr, sizeof(Vector3), &eyeDir, sizeof(Vector3));

			const Matrix4x4& invViewProjMat = m_Camera.GetInverseViewProjectionMatrix();
			memcpy_s(pAddr + sizeof(Vector4), sizeof(Matrix4x4), &invViewProjMat, sizeof(Matrix4x4));

			m_PointLightingStage.pUniformBuffer->Unmap();
		}

		// 並列データ
		m_ParallelData.frame = currentImageIndex;
		m_ParallelData.viewProjMatrix = m_Camera.GetViewProjectionMatrix();
		m_ParallelData.pFrameBuffer = pFrameBuffer;

		// ----------------------------------------------------------------------------------------------------
		// 描画前の処理
		// ----------------------------------------------------------------------------------------------------

		m_Font.Bake(GetWorkQueue(), GetWorkCommandBuffer(), GetWorkFence());

#ifdef ENABLE_MULTITHREAD
		// メッシュの更新
		m_MeshManager.BeginUpdate();
		m_ParallelManager.Execute(ParallelUpdate, TO_UI32(m_Meshes.size()), &m_ParallelData);
		m_MeshManager.EndUpdate();

		// サブパス 0 - メッシュの描画コマンドを記録
		m_ParallelManager.Execute(ParallelDraw, TO_UI32(m_Meshes.size()), &m_ParallelData);

#else //ENABLE_MULTITHREAD

		const Matrix4x4& viewProjMat = m_Camera.GetViewProjectionMatrix();

		std::vector<MeshPtr>::iterator it_mesh_begin = m_Meshes.begin();
		std::vector<MeshPtr>::iterator it_mesh_end = m_Meshes.end();
		std::vector<MeshPtr>::iterator it_mesh;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			(*it_mesh)->Update(viewProjMat);
		}

#endif //ENABLE_MULTITHREAD

		// ----------------------------------------------------------------------------------------------------
		// 描画
		// ----------------------------------------------------------------------------------------------------

		IV3DCommandBuffer* pGraphicsCommandBuffer = BeginGraphics();
		if (pGraphicsCommandBuffer == nullptr)
		{
			return false;
		}

		// バックバッファをバリア
		IV3DImageView* pBackBuffer;
		pFrameBuffer->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_DG_BACK_BUFFER, &pBackBuffer);

		V3DBarrierImageDesc barrier{};
		barrier.srcStageMask = V3D_PIPELINE_STAGE_TOP_OF_PIPE;
		barrier.dstStageMask = V3D_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
		barrier.srcAccessMask = V3D_ACCESS_MEMORY_READ;
		barrier.dstAccessMask = V3D_ACCESS_COLOR_ATTACHMENT_WRITE;
		barrier.srcQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamily = V3D_QUEUE_FAMILY_IGNORED;
		barrier.srcLayout = V3D_IMAGE_LAYOUT_PRESENT_SRC;
		barrier.dstLayout = V3D_IMAGE_LAYOUT_COLOR_ATTACHMENT;
		pGraphicsCommandBuffer->BarrierImageView(pBackBuffer, barrier);

		SAFE_RELEASE(pBackBuffer);

		V3DViewport viewport{};
		viewport.rect.width = swapChainDesc.imageWidth;
		viewport.rect.height = swapChainDesc.imageHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		pGraphicsCommandBuffer->SetViewport(0, 1, &viewport);
		pGraphicsCommandBuffer->SetScissor(0, 1, &viewport.rect);

#ifdef ENABLE_MULTITHREAD

		// レンダーパスを開始
		pGraphicsCommandBuffer->BeginRenderPass(m_RenderPassHandle->GetPtr(), pFrameBuffer, false);

		// サブパス 0 : ジオメトリバッファ
		pGraphicsCommandBuffer->ExecuteCommandBuffers(m_ParallelManager.GetThreadCount(), m_ParallelData.commandBuffer[m_ParallelData.frame].data());

#else //ENABLE_MULTITHREAD

		// レンダーパスを開始
		pGraphicsCommandBuffer->BeginRenderPass(m_RenderPassHandle->GetPtr(), pFrameBuffer, true);

		// サブパス 0 : ジオメトリバッファ
		uint64_t prevMeshKey = 0;
		uint64_t prevMaterialKey = 0;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			Mesh* pMesh = (*it_mesh).get();
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
				const Mesh::Subset& subset = pMesh->GetSubset(i);

				MaterialPtr material = pMesh->GetMaterial(subset.material);
				uint64_t materialKey = material->GetKey();

				if (materialKey != prevMaterialKey)
				{
					material->Bind(pGraphicsCommandBuffer);
					prevMaterialKey = materialKey;
				}

				pGraphicsCommandBuffer->DrawIndexed(subset.indexCount, 1, subset.firstIndex, 0, 0);
			}
		}
#endif //ENABLE_MULTITHREAD

		uint64_t simpleVertexOffset = 0;

		// サブパス 1 : ディレクショナルライティング
		pGraphicsCommandBuffer->NextSubpass();
		pGraphicsCommandBuffer->BindPipeline(m_DirectionalLightingStage.pipelineHandle->GetPtr());
		pGraphicsCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_DirectionalLightingStage.pPipelineLayout, 0, 1, &m_DirectionalLightingStage.pDescriptorSet, 0, nullptr);
		pGraphicsCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pGraphicsCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 2 : ポイントライティング
		pGraphicsCommandBuffer->NextSubpass();
		pGraphicsCommandBuffer->BindPipeline(m_PointLightingStage.pipelineHandle->GetPtr());
		pGraphicsCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_PointLightingStage.pPipelineLayout, 0, 1, &m_PointLightingStage.pDescriptorSet, 0, nullptr);
		pGraphicsCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pGraphicsCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 2 : フィニッシュライティング
		pGraphicsCommandBuffer->NextSubpass();
		pGraphicsCommandBuffer->BindPipeline(m_FinishLightingStage.pipelineHandle->GetPtr());
		pGraphicsCommandBuffer->BindDescriptorSets(V3D_PIPELINE_TYPE_GRAPHICS, m_FinishLightingStage.pPipelineLayout, 0, 1, &m_FinishLightingStage.pDescriptorSet, 0, nullptr);
		pGraphicsCommandBuffer->BindVertexBuffers(0, 1, &m_pSimpleVertexBuffer, &simpleVertexOffset);
		pGraphicsCommandBuffer->Draw(4, 1, 0, 0);

		// サブパス 3 : テキスト
		pGraphicsCommandBuffer->NextSubpass();
		m_Font.Flush(pGraphicsCommandBuffer, swapChainDesc.imageWidth, swapChainDesc.imageHeight);

		// レンダーパスを終了
		pGraphicsCommandBuffer->EndRenderPass();

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
		if (m_ParallelData.commandBuffer.empty() == false)
		{
			for (size_t i = 0; i < m_ParallelData.commandBuffer.size(); i++)
			{
				for (size_t j = 0; j < m_ParallelData.commandBuffer[i].size(); j++)
				{
					SAFE_RELEASE(m_ParallelData.commandBuffer[i][j]);
				}

				m_ParallelData.commandBuffer[i].clear();
			}

			m_ParallelData.commandBuffer.clear();
		}

		if (m_pFrameBuffer.empty() == false)
		{
			for (size_t i = 0; i < m_pFrameBuffer.size(); i++)
			{
				SAFE_RELEASE(m_pFrameBuffer[i]);
			}
			m_pFrameBuffer.clear();
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

		if (m_Font.Restore(m_RenderPassHandle->GetPtr(), 4) == false)
		{
			return false;
		}

		// ----------------------------------------------------------------------------------------------------
		// フレームバッファーを作成
		// ----------------------------------------------------------------------------------------------------

		const V3DSwapChainDesc& swapChainDesc = GetSwapChain()->GetDesc();

		m_pFrameBuffer.resize(swapChainDesc.imageCount);

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			if (m_GraphicsManager.CreateFrameBuffer(GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY, i, &m_pFrameBuffer[i]) != V3D_OK)
			{
				return false;
			}
		}

		// ----------------------------------------------------------------------------------------------------
		// イメージを再設定
		// ----------------------------------------------------------------------------------------------------

		IV3DImageView* pColorImageView;
		IV3DImageView* pLightImageView;
		IV3DImageView* pNormalImageView;
		IV3DImageView* pDataImageView;

		m_pFrameBuffer[0]->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_DG_COLOR, &pColorImageView);
		m_pFrameBuffer[0]->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_DG_BUFFER_0, &pNormalImageView);
		m_pFrameBuffer[0]->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_DG_BUFFER_1, &pDataImageView);
		m_pFrameBuffer[0]->GetAttachment(GRAPHICS_ATTACHMENT_TYPE_DG_LIGHT, &pLightImageView);

		// ディレクショナルライティング
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_DirectionalLightingStage.pDescriptorSet->Update();

		// ポイントライティング
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pNormalImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->SetImageViewAndSampler(2, pDataImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_PointLightingStage.pDescriptorSet->Update();

		// フィニッシュライティング
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(0, pColorImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->SetImageViewAndSampler(1, pLightImageView, V3D_IMAGE_LAYOUT_SHADER_READ_ONLY, m_pSimpleSampler);
		m_FinishLightingStage.pDescriptorSet->Update();

		SAFE_RELEASE(pLightImageView);
		SAFE_RELEASE(pDataImageView);
		SAFE_RELEASE(pNormalImageView);
		SAFE_RELEASE(pColorImageView);

		// ----------------------------------------------------------------------------------------------------
		// 並列処理用のデータを作成
		// ----------------------------------------------------------------------------------------------------

		uint32_t threadCount = m_ParallelManager.GetThreadCount();

		m_ParallelData.commandBuffer.resize(swapChainDesc.imageCount);
		m_ParallelData.pRenderPass = m_RenderPassHandle->GetPtr();
		m_ParallelData.viewport.rect.x = 0;
		m_ParallelData.viewport.rect.y = 0;
		m_ParallelData.viewport.rect.width = swapChainDesc.imageWidth;
		m_ParallelData.viewport.rect.height = swapChainDesc.imageHeight;
		m_ParallelData.viewport.minDepth = 0.0f;
		m_ParallelData.viewport.maxDepth = 1.0f;
		m_ParallelData.scissor.x = 0;
		m_ParallelData.scissor.y = 0;
		m_ParallelData.scissor.width = swapChainDesc.imageWidth;
		m_ParallelData.scissor.height = swapChainDesc.imageHeight;

		for (uint32_t i = 0; i < swapChainDesc.imageCount; i++)
		{
			V3DCommandPoolDesc commandPoolDesc{};
			commandPoolDesc.propertyFlags = V3D_COMMAND_POOL_PROPERTY_RESET_COMMAND_BUFFER;
			commandPoolDesc.queueFamily = 0;

			m_ParallelData.commandBuffer[i].reserve(threadCount);

			for (uint32_t j = 0; j < threadCount; j++)
			{
				IV3DCommandBuffer* pCommandBuffer;
				V3D_RESULT result = Application::GetDevice()->CreateCommandBuffer(commandPoolDesc, V3D_COMMAND_BUFFER_TYPE_SECONDARY, &pCommandBuffer);
				if (result != V3D_OK)
				{
					return false;
				}

				m_ParallelData.commandBuffer[i].push_back(pCommandBuffer);
			}
		}

		// ----------------------------------------------------------------------------------------------------

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
				m_CameraDistance += static_cast<float>(vec.y) * -0.05f;
				m_CameraDistance = MAXIMUM(1.0f, m_CameraDistance);
			}
			else if (keyFlags & MK_SHIFT)
			{
				Matrix4x4 mat = m_CameraRot;

				Vector3 dirX(1.0f, 0.0f, 0.0f);
				Vector3 dirY(0.0f, 1.0f, 0.0f);

				dirX = mat * dirX;
				dirY = mat * dirY;

				m_CameraAt += dirX * static_cast<float>(vec.x) * -0.015f;
				m_CameraAt += dirY * static_cast<float>(vec.y) * -0.015f;
			}
			else
			{
				Quaternion rot;

				rot.SetRotationAxis(Vector3::YAxis(), DEG_TO_RAD(static_cast<float>(vec.x) * -0.25f));
				rot.RotationAxis(Vector3::XAxis(), DEG_TO_RAD(static_cast<float>(vec.y) * 0.25f));

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
			m_CameraDistance -= 0.5f;
		}
		else if (delta < 0)
		{
			m_CameraDistance += 0.5f;
		}

		return true;
	}

	static void __stdcall ParallelUpdate(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		ParallelData* pParallel = static_cast<ParallelData*>(pData);

		std::vector<MeshPtr>::iterator it_mesh_begin = pParallel->pMeshes->begin() + first;
		std::vector<MeshPtr>::iterator it_mesh_end = it_mesh_begin + count;
		std::vector<MeshPtr>::iterator it_mesh;

		const Matrix4x4& viewProjMatrix = pParallel->viewProjMatrix;

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			(*it_mesh)->Update(viewProjMatrix);
		}
	}
		
	static void __stdcall ParallelDraw(uint32_t thread, uint32_t first, uint32_t count, void* pData)
	{
		ParallelData* pParallel = static_cast<ParallelData*>(pData);
		IV3DCommandBuffer* pCommandBuffer = pParallel->commandBuffer[pParallel->frame][thread];

		std::vector<MeshPtr>::iterator it_mesh_begin = pParallel->pMeshes->begin() + first;
		std::vector<MeshPtr>::iterator it_mesh_end = it_mesh_begin + count;
		std::vector<MeshPtr>::iterator it_mesh;

		uint64_t prevMeshKey = 0;
		uint64_t prevMaterialKey = 0;

		pCommandBuffer->Begin(V3D_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE, pParallel->pRenderPass, 0, pParallel->pFrameBuffer);

		for (it_mesh = it_mesh_begin; it_mesh != it_mesh_end; ++it_mesh)
		{
			Mesh* pMesh = (*it_mesh).get();
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
				const Mesh::Subset& subset = pMesh->GetSubset(i);

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
		}

		pCommandBuffer->End();
	}

private:
	struct ParallelData
	{
		uint32_t frame;
		std::vector<std::vector<IV3DCommandBuffer*>> commandBuffer;
		IV3DRenderPass* pRenderPass;
		IV3DFrameBuffer* pFrameBuffer;
		V3DViewport viewport;
		V3DRectangle2D scissor;
		Matrix4x4 viewProjMatrix;
		std::vector<MeshPtr>* pMeshes;
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

	ParallelData m_ParallelData;

	GraphicsRenderPassHandle m_RenderPassHandle;

	std::vector<IV3DFrameBuffer*> m_pFrameBuffer;

	IV3DBuffer* m_pSimpleVertexBuffer;
	IV3DSampler* m_pSimpleSampler;

	std::vector<MeshPtr> m_Meshes;

	LightingStage m_DirectionalLightingStage;
	LightingStage m_PointLightingStage;
	LightingStage m_FinishLightingStage;

	Quaternion m_CameraRot;
	float m_CameraDistance;
	Vector3 m_CameraAt;
	Camera m_Camera;

	bool m_bDrag;
	POINT m_MousePos;
};

class TestApp : public Application
{
public:
	TestApp() {}
	virtual ~TestApp() {}

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

		if (m_Window.Initialize(L"test", SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_BUFFERING_TYPE_FAKE, pWorkQueue, pGraphicsQueue) == false)
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
