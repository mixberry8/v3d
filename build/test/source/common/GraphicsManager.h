#pragma once

// ----------------------------------------------------------------------------------------------------
// 構造体
// ----------------------------------------------------------------------------------------------------

// サンプリング
struct SamplingConstant
{
	Vector2 texelSize;

	static Vector2 CalcTexelSize(uint32_t width, uint32_t height)
	{
		ASSERT(width > 0);
		ASSERT(height > 0);

		Vector2 texelSize;

		texelSize.x = 1.0f / TO_F32(width);
		texelSize.y = 1.0f / TO_F32(height);

		return texelSize;
	}
};

// ブライトパス
struct BrightPassConstant
{
	float threshold;
	float offset;
};

// ガウシアンブラー
struct GaussianBlurConstant
{
	Vector2 step;
	float reserve1[2];
	Vector3 inc;
	float reserve2[1];
	uint32_t sampleCount;

	static Vector2 CalcStep(uint32_t width, uint32_t height, const Vector2& dir)
	{
		ASSERT(width > 0);
		ASSERT(height > 0);

		Vector2 texelSize;

		texelSize.x = 1.0f / TO_F32(width);
		texelSize.y = 1.0f / TO_F32(height);

		return texelSize * dir;
	}

	static Vector3 CalcInc(float radius)
	{
		ASSERT(radius > 0.0f);

		static const float TWO_PI_SQR = sqrtf(6.2831853071795f);

		float sigma = radius / 8.0f;
		float sigma2 = sigma * sigma;

		Vector3 ret;

		ret.x = 1.0f / (TWO_PI_SQR * sigma);
		ret.y = expf(-0.5f / sigma2);
		ret.z = ret.y * ret.y;

		return ret;
	}
};

// ２枚を合成して出力
struct Composite2Constant
{
	Vector4 color0;
	Vector4 color1;
};

// SSAO
struct SsaoConstant
{
	Vector4 params0; // x=radius y=threshold z=depth w=intensity
	Vector4 params1; // xy=noiseTextureAspect
};

// 補正
struct CorrectionConstant
{
	float invGamma;
	float contrast;
	float reserve[2];
};

// FXAA
struct FxaaConstant
{
	Vector4 texelOffset0;
	Vector4 texelOffset1;
	Vector4 invTexSize;
};

// シーン
struct SceneConstant
{
	Matrix4x4 viewProjMatrix;
};

// スカイドーム
struct SkyDomeVertConstant
{
	Matrix4x4 worldViewProjMatrix;
	Matrix4x4 worldMatrix;
};

struct SkyDomeFragUniform
{
	Vector3 eyePos;
	float reserve1;
	Vector3 sunDir;
	float reserve2;
	Vector4 sunColor;
	Vector4 nightColor;
	Vector4 henyeyGreenstein;
	Vector3 betaDashR;
	float reserve3;
	Vector3 betaDashM;
	float reserve4;
	Vector3 betaRM;
	float reserve5;
	Vector4 oneOverBetaRM;
	Vector4 mulitpliers;
};

// ディレクショナルライト
struct DirectionalLightingUniform
{
	Vector3 eyeDir;
	float reserve0;

	Vector3 lightDir;
	float reserve1;
	Vector4 lightColor;

	Matrix4x4 invViewProjMatrix;
	Matrix4x4 lightMatrix;
	Vector4 shadowParam;
};

// ポイントライト
struct PointLight
{
	Vector4 color;
	Vector4 posAndDist;
};

static auto constexpr POINT_LIGHT_MAX = 32;

struct PointLightingUniform
{
	Vector3 eyePos;
	uint32_t reserve0[1];

	Matrix4x4 invViewProjMat;

	uint32_t pointLightCount;
	uint32_t reserve[3];

	PointLight pointLights[POINT_LIGHT_MAX];
};

// ----------------------------------------------------------------------------------------------------
// アタッチメントタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_ATTACHMENT_TYPE
{																  
	// ジオメトリ - GRAPHICS_RENDERPASS_TYPE_GEOMETRY
	GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_COLOR = 0,
	GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_0 = 1,
	GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_BUFFER_1 = 2,
	GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_DEPTH_STENCIL = 3,
	GRAPHICS_ATTACHMENT_TYPE_GEOMETRY_LIGHT = 4,

	// 遠景 - GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW
	GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_COLOR = 0,
	GRAPHICS_ATTACHMENT_TYPE_DISTANT_VIEW_DEPTH_STENCIL = 1,

	// SSAO : GRAPHICS_RENDERPASS_TYPE_SSAO
	GRAPHICS_ATTACHMENT_TYPE_SSAO_COLOR = 0, // color

	// シャドウプロジェクション - GRAPHICS_RENDERPASS_TYPE_SHADOW
	GRAPHICS_ATTACHMENT_TYPE_SHADOW_BUFFER = 0,        // color
	GRAPHICS_ATTACHMENT_TYPE_SHADOW_DEPTH_STENCIL = 1, // depthStencil

	// ライティング - GRAPHICS_RENDERPASS_TYPE_LIGHTING
	GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COLOR = 0,           // input & color
	GRAPHICS_ATTACHMENT_TYPE_LIGHTING_COMPOSITE_COLOR = 1, // color

	// イメージエフェクト
	GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_0 = 0, // color
	GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_COLOR_1 = 1, // color
	GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_0 = 2, // color
	GRAPHICS_ATTACHMENT_TYPE_IMAGE_EFFECT_LDR_COLOR_1 = 3, // color

	// フィニッシュ
	GRAPHICS_ATTACHMENT_TYPE_FINISH_BACK_BUFFER = 0, // color

	// ブライトパス - GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS
	GRAPHICS_ATTACHMENT_TYPE_BRIGHT_PASS_COLOR = 0,

	// ブラー - GRAPHICS_RENDERPASS_TYPE_BLUR
	GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_0 = 0, // input & color
	GRAPHICS_ATTACHMENT_TYPE_BLUR_COLOR_1 = 1, // input & color

	// 合成 : GRAPHICS_RENDERPASS_TYPE_COMPOSITE
	GRAPHICS_ATTACHMENT_TYPE_COMPOSITE_COLOR = 0, // color
};

// ----------------------------------------------------------------------------------------------------
// レンダーパスタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_RENDERPASS_TYPE
{
	GRAPHICS_RENDERPASS_TYPE_GEOMETRY = 0,     // ジオメトリ
	GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW = 1, // 遠景 ( カラーのみの出力 深度は書き込まず、テストのみ )
	GRAPHICS_RENDERPASS_TYPE_SSAO = 2,         // SSAO
	GRAPHICS_RENDERPASS_TYPE_SHADOW = 3,       // シャドウ
	GRAPHICS_RENDERPASS_TYPE_LIGHTING = 4,     // ライティング
	GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT = 5, // イメージエフェクト
	GRAPHICS_RENDERPASS_TYPE_FINISH = 6,       // フィニッシュ

	GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS = 7,  // ブライトパス
	GRAPHICS_RENDERPASS_TYPE_BLUR = 8,         // ブラー ( 0:ダウンサンプリング 1:横ブラー 2:縦ブラー )
	GRAPHICS_RENDERPASS_TYPE_COMPOSITE = 9,    // イメージを合成して出力
};

static auto constexpr GRAPHICS_RENDERPASS_TYPE_COUNT = 10;

// ----------------------------------------------------------------------------------------------------
// パイプラインタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_PIPELINE_TYPE
{
	// GRAPHICS_RENDERPASS_TYPE_GEOMETRY
	GRAPHICS_PIPELINE_TYPE_GEOMETRY = 0,

	// GRAPHICS_RENDERPASS_TYPE_DISTANT_VIEW
	GRAPHICS_PIPELINE_TYPE_DISTANT_VIEW_SKYDOME = 1,

	// GRAPHICS_RENDERPASS_TYPE_SSAO
	GRAPHICS_PIPELINE_TYPE_SSAO = 2,

	// GRAPHICS_RENDERPASS_TYPE_SHADOW
	GRAPHICS_PIPELINE_TYPE_SHADOW = 3,

	// GRAPHICS_RENDERPASS_TYPE_LIGHTING
	GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING = 4,
	GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING = 5,
	GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING = 6,

	// GRAPHICS_RENDERPASS_TYPE_IMAGE_EFFECT
	GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY = 7,
	GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_CORRECTION = 8,
	GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING = 9,
	GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA = 10,

	// GRAPHICS_RENDERPASS_TYPE_FINISH
	GRAPHICS_PIPELINE_TYPE_FINISH_INIT = 11,

	// GRAPHICS_RENDERPASS_TYPE_BRIGHT_PASS
	GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS = 12,

	// GRAPHICS_RENDERPASS_TYPE_BLUR
	GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING = 13,
	GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN = 14,

	// GRAPHICS_RENDERPASS_TYPE_COMPOSITE
	GRAPHICS_PIPELINE_TYPE_COMPOSITE_1 = 15,
	GRAPHICS_PIPELINE_TYPE_COMPOSITE_2 = 16,
};

static auto constexpr GRAPHICS_PIPELINE_TYPE_COUNT = 17;

// ----------------------------------------------------------------------------------------------------
// デスクリプタセットタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_DESCRIPTOR_SET_TYPE
{
	// GRAPHICS_PIPELINE_TYPE_GEOMETRY
	// GRAPHICS_PIPELINE_TYPE_SHADOW_PROJECTION
	GRAPHICS_DESCRIPTOR_SET_TYPE_MESH = 0,
	GRAPHICS_DESCRIPTOR_SET_TYPE_MATERIAL = 1,

	// GRAPHICS_PIPELINE_TYPE_GEOMETRY
	GRAPHICS_DESCRIPTOR_SET_TYPE_SKYDOME = 0,

	// GRAPHICS_PIPELINE_TYPE_SSAO
	GRAPHICS_DESCRIPTOR_SET_TYPE_SSAO = 0,

	// GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING = 0,

	// GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING = 0,

	// GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING = 0,

	// GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_COPY
	// GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_TONE_MAPPING
	// GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA
	GRAPHICS_DESCRIPTOR_SET_TYPE_IMAGE_EFFECT_FXAA = 0,

	// GRAPHICS_PIPELINE_TYPE_FINISH_INIT
	GRAPHICS_DESCRIPTOR_SET_TYPE_OVERLAY_COPY = 0,

	// GRAPHICS_PIPELINE_TYPE_BRIGHT_PASS
	GRAPHICS_DESCRIPTOR_SET_TYPE_BRIGHT_PASS = 0,

	// GRAPHICS_PIPELINE_TYPE_BLUR_DOWN_SAMPLING
	GRAPHICS_DESCRIPTOR_SET_TYPE_BLUR_DAWN_SAMPLING = 0,

	// GRAPHICS_PIPELINE_TYPE_BLUR_GAUSSIAN
	GRAPHICS_DESCRIPTOR_SET_TYPE_BLUR_GAUSSIAN = 0,

	// GRAPHICS_PIPELINE_TYPE_COMPOSITE_2
	GRAPHICS_DESCRIPTOR_SET_TYPE_COMPOSITE = 0,
};

static auto constexpr GRAPHICS_DESCRIPTOR_SET_TYPE_COUNT = 2;

// ----------------------------------------------------------------------------------------------------
// グラフィックスパイプラインの記述
// ----------------------------------------------------------------------------------------------------

struct GraphicsPipelienDesc
{
	V3D_POLYGON_MODE polygonMode;
	V3D_CULL_MODE cullMode;
	bool depthTestEnable;
	bool depthWriteEnable;
	BLEND_MODE blendMode;

	bool operator < (const GraphicsPipelienDesc& rh) const
	{
		if (polygonMode > rh.polygonMode)
		{
			return false;
		}
		else if (polygonMode < rh.polygonMode)
		{
			return true;
		}

		if (cullMode > rh.cullMode)
		{
			return false;
		}
		else if (cullMode < rh.cullMode)
		{
			return true;
		}

		if (depthTestEnable > rh.depthTestEnable)
		{
			return false;
		}
		else if (depthTestEnable < rh.depthTestEnable)
		{
			return true;
		}

		if (depthWriteEnable > rh.depthWriteEnable)
		{
			return false;
		}
		else if (depthWriteEnable < rh.depthWriteEnable)
		{
			return true;
		}

		if (blendMode > rh.blendMode)
		{
			return false;
		}
		else if (blendMode < rh.blendMode)
		{
			return true;
		}

		return false;
	}
};

// ----------------------------------------------------------------------------------------------------
// レンダーパスのハンドル
// ----------------------------------------------------------------------------------------------------

typedef struct GraphicsRenderPassHandleT
{
public:
	IV3DRenderPass* GetPtr() { return m_pRenderPass; }

private:
	IV3DRenderPass* m_pRenderPass;

	GraphicsRenderPassHandleT() {}
	~GraphicsRenderPassHandleT() {}

	friend class GraphicsManager;

}*GraphicsRenderPassHandle;

// ----------------------------------------------------------------------------------------------------
// パイプラインのハンドル
// ----------------------------------------------------------------------------------------------------

typedef struct GraphicsPipelineHandleT
{
public:
	IV3DPipeline* GetPtr() { return m_pPipeline; }

private:
	IV3DPipeline* m_pPipeline;

	GraphicsPipelineHandleT() {}
	~GraphicsPipelineHandleT() {}

	friend class GraphicsManager;

}*GraphicsPipelineHandle;

// ----------------------------------------------------------------------------------------------------
// グラフィックスマネージャー
// ----------------------------------------------------------------------------------------------------

class GraphicsManager
{
public:
	static constexpr uint32_t ShadowMapSize = 1024;

	GraphicsManager();
	~GraphicsManager();

	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DQueue* pQueue, IV3DSwapChain* pSwapChain);
	void Finalize();

	void Lost();
	V3D_RESULT Restore();

	void GetPipelineLayout(
		GRAPHICS_PIPELINE_TYPE pipelineType,
		IV3DPipelineLayout** ppPipelineLayout);

	V3D_RESULT GetRenderPassHandle(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		GraphicsRenderPassHandle* pRenderPassHandle);

	V3D_RESULT GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		uint32_t subpass,
		GRAPHICS_PIPELINE_TYPE pipelineType,
		const GraphicsPipelienDesc& desc,
		GraphicsPipelineHandle* pPipelineHandle);

	V3D_RESULT CreateFrameBuffer(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		uint32_t index,
		IV3DFrameBuffer** ppFrameBuffer);

	V3D_RESULT CreateDescriptorSet(
		GRAPHICS_PIPELINE_TYPE pipelineType,
		GRAPHICS_DESCRIPTOR_SET_TYPE descriptorSetType,
		IV3DDescriptorSet** ppDescriptorSet);

private:
	static constexpr uint32_t DRMeshDescPoolSize = 256;
	static constexpr uint32_t DRMaterialDescPoolSize = 256;
	static constexpr uint32_t DRLightingDescPoolSize = 2;
	static constexpr char* BasicEntryPointName = "main";

	enum SHADER_TYPE
	{
		SHADER_TYPE_VERT_SIMPLE = 0,
		SHADER_TYPE_FRAG_SIMPLE = 1,
		SHADER_TYPE_FRAG_DOWN_SAMPLING_2x2 = 2,

		SHADER_TYPE_FRAG_GAUSSIAN_BLUR = 6,

		SHADER_TYPE_FRAG_BRIGHT_PASS = 8,
		SHADER_TYPE_FRAG_COMPOSITE_2 = 9,

		SHADER_TYPE_VERT_SHADOW_MESH = 10,
		SHADER_TYPE_FRAG_SHADOW_MESH = 11,

		SHADER_TYPE_VERT_MESH = 12,
		SHADER_TYPE_FRAG_MESH_D = 13,

		SHADER_TYPE_VERT_SKYDOME = 14,
		SHADER_TYPE_FRAG_SKYDOME = 15,

		SHADER_TYPE_FRAG_DIRECTIONAL_LIGHTING_D = 16,
		SHADER_TYPE_FRAG_POINT_LIGHTING_D = 17,
		SHADER_TYPE_FRAG_FINISH_LIGHTING_D = 18,

		SHADER_TYPE_FRAG_SSAO = 20,
		SHADER_TYPE_FRAG_CORRECTION = 21,
		SHADER_TYPE_FRAG_TONE_MAPPING = 22,
		SHADER_TYPE_FRAG_FXAA = 23,
	};

	static auto constexpr SHADER_TYPE_COUNT = 24;

	enum IMAGE_TYPE
	{
		IMAGE_TYPE_GEOMETRY_COLOR_0 = 0,
		IMAGE_TYPE_GEOMETRY_COLOR_1 = 1,
		IMAGE_TYPE_GEOMETRY_BUFFER_0 = 2,
		IMAGE_TYPE_GEOMETRY_BUFFER_1 = 3,
		IMAGE_TYPE_GEOMETRY_DEPTH_STENCIL = 4,

		IMAGE_TYPE_SSAO_COLOR = 5,

		IMAGE_TYPE_SHADOW_BUFFER = 6,
		IMAGE_TYPE_SHADOW_DEPTH_STENCIL = 7,

		IMAGE_TYPE_LIGHT_COLOR = 8,

		IMAGE_TYPE_LDR_COLOR_0 = 9,
		IMAGE_TYPE_LDR_COLOR_1 = 10,

		IMAGE_TYPE_BRIGHT_PASS_COLOR = 11,

		IMAGE_TYPE_BLUR_2_0 = 12,
		IMAGE_TYPE_BLUR_2_1 = 13,
		IMAGE_TYPE_BLUR_4_0 = 14,
		IMAGE_TYPE_BLUR_4_1 = 15,

		IMAGE_TYPE_BACK_BUFFER = 16,
	};

	static auto constexpr IMAGE_TYPE_COUNT = 16; // バックバッファの数は含めない

	enum IMAGE_SIZE_TYPE
	{
		IMAGE_SIZE_DEFAULT = 0, // スクリーンサイズ
		IMAGE_SIZE_HALF = 1, // スクリーンサイズの 1/2
		IMAGE_SIZE_QUARTER = 2, // スクリーンサイズの 1/4
		IMAGE_SIZE_RESERVED = 3, // サイズは指定されている
	};

	enum DESCRIPTOR_SET_TYPE
	{
		// GRAPHICS_PIPELINE_TYPE_IMAGE_EFFECT_FXAA
		DESCRIPTOR_SET_TYPE_SIMPLE_SAMPLING = 0,

		// GRAPHICS_PIPELINE_TYPE_GEOMETRY
		// GRAPHICS_PIPELINE_TYPE_SHADOW_PROJECTION
		DESCRIPTOR_SET_TYPE_MESH = 1,
		DESCRIPTOR_SET_TYPE_MATERIAL = 2,

		// GRAPHICS_PIPELINE_TYPE_GEOMETRY
		DESCRIPTOR_SET_TYPE_SKYDOME = 3,

		// GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING
		DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING = 4,
		// GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING
		DESCRIPTOR_SET_TYPE_POINT_LIGHTING = 5,
		// GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING
		DESCRIPTOR_SET_TYPE_FINISH_LIGHTING = 6,

		// GRAPHICS_PIPELINE_TYPE_COMPOSITE_2
		DESCRIPTOR_SET_TYPE_COMPOSITE_2 = 7,

		// GRAPHICS_PIPELINE_TYPE_SSAO
		DESCRIPTOR_SET_TYPE_SSAO = 8,
	};

	static auto constexpr DESCRIPTOR_SET_TYPE_COUNT = 9;

	struct Attachment
	{
		std::wstring name;

		V3DImageDesc imageDesc;
		IV3DImage* pImage;
		V3DFlags imageStageMask;
		V3DFlags imageAccessMask;
		V3D_IMAGE_LAYOUT imageLayout;
		GraphicsManager::IMAGE_SIZE_TYPE imageSizeType;

		V3DImageViewDesc imageViewDesc;
		IV3DImageView* pImageView;
	};

	struct PipelineDesc : public V3DGraphicsPipelineDesc
	{
	public:
		void Allocate(uint32_t vertexElementCount, uint32_t vertexLayoutCount, uint32_t attachmentCount)
		{
			m_VertexElements.resize(vertexElementCount, V3DPipelineVertexElement{});
			this->vertexInput.elementCount = vertexElementCount;
			this->vertexInput.pElements = m_VertexElements.data();

			m_VertexLayouts.resize(vertexLayoutCount, V3DPipelineVertexLayout{});
			this->vertexInput.layoutCount = vertexLayoutCount;
			this->vertexInput.pLayouts = m_VertexLayouts.data();

			depthStencil.stencilTestEnable = false;
			depthStencil.stencilFront.failOp = V3D_STENCIL_OP_KEEP;
			depthStencil.stencilFront.passOp = V3D_STENCIL_OP_KEEP;
			depthStencil.stencilFront.depthFailOp = V3D_STENCIL_OP_KEEP;
			depthStencil.stencilFront.compareOp = V3D_COMPARE_OP_ALWAYS;
			depthStencil.stencilFront.readMask = 0x000000FF;
			depthStencil.stencilFront.writeMask = 0x000000FF;
			depthStencil.stencilBack = depthStencil.stencilFront;
			depthStencil.depthBoundsTestEnable = false;
			depthStencil.minDepthBounds = 0.0f;
			depthStencil.maxDepthBounds = 1.0f;

			m_Attachments.resize(attachmentCount, V3DPipelineColorBlendAttachment{});
			this->colorBlend.attachmentCount = attachmentCount;
			this->colorBlend.pAttachments = m_Attachments.data();
		}

	private:
		std::vector<V3DPipelineVertexElement> m_VertexElements;
		std::vector<V3DPipelineVertexLayout> m_VertexLayouts;
		std::vector<V3DViewport> m_Viewports;
		std::vector<V3DRectangle2D> m_Scissors;
		std::vector<V3DPipelineColorBlendAttachment> m_Attachments;
	};

	struct RenderPassDesc
	{
	public:
		struct Subpass
		{
			std::vector<V3DAttachmentReference> inputAttachments;
			std::vector<V3DAttachmentReference> colorAttachments;
			V3DAttachmentReference depthStencilAttachment;
			std::vector<uint32_t> preserveAttachments;
		};

		uint32_t attachmentCount;
		V3DAttachmentDesc* pAttachments;

		uint32_t subpassCount;
		V3DSubpassDesc* pSubpasses;

		uint32_t subpassDependencyCount;
		V3DSubpassDependencyDesc* pSubpassDependencies;

		void Allocate(uint32_t attachmentCount, uint32_t subpassCount, uint32_t subpassDependencyCount)
		{
			m_Attachments.resize(attachmentCount);
			m_Subpasses.resize(subpassCount);
			m_InternbalSubpasses.resize(subpassCount);
			m_SubpassDependencies.resize(subpassDependencyCount);

			this->attachmentCount = TO_UI32(m_Attachments.size());
			pAttachments = m_Attachments.data();

			this->subpassCount = TO_UI32(m_Subpasses.size());
			pSubpasses = m_Subpasses.data();

			this->subpassDependencyCount = TO_UI32(m_SubpassDependencies.size());
			pSubpassDependencies = m_SubpassDependencies.data();
		}

		RenderPassDesc::Subpass* AllocateSubpass(uint32_t subpass, uint32_t inputAttachmentCount, uint32_t colorAttachmentCount, bool depthStencilEnable, uint32_t preserveAttachmentCount)
		{
			m_InternbalSubpasses[subpass].inputAttachments.resize(inputAttachmentCount);
			m_InternbalSubpasses[subpass].colorAttachments.resize(colorAttachmentCount);
			m_InternbalSubpasses[subpass].preserveAttachments.resize(preserveAttachmentCount);

			m_Subpasses[subpass].inputAttachmentCount = TO_UI32(m_InternbalSubpasses[subpass].inputAttachments.size());
			m_Subpasses[subpass].pInputAttachments = m_InternbalSubpasses[subpass].inputAttachments.data();
			m_Subpasses[subpass].colorAttachmentCount = TO_UI32(m_InternbalSubpasses[subpass].colorAttachments.size());
			m_Subpasses[subpass].pColorAttachments = m_InternbalSubpasses[subpass].colorAttachments.data();
			m_Subpasses[subpass].pResolveAttachments = nullptr;
			m_Subpasses[subpass].pDepthStencilAttachment = (depthStencilEnable == true) ? &m_InternbalSubpasses[subpass].depthStencilAttachment : nullptr;
			m_Subpasses[subpass].preserveAttachmentCount = TO_UI32(m_InternbalSubpasses[subpass].preserveAttachments.size());
			m_Subpasses[subpass].pPreserveAttachments = m_InternbalSubpasses[subpass].preserveAttachments.data();

			return &m_InternbalSubpasses[subpass];
		}

	private:
		std::vector<V3DAttachmentDesc> m_Attachments;
		std::vector<RenderPassDesc::Subpass> m_InternbalSubpasses;
		std::vector<V3DSubpassDesc> m_Subpasses;
		std::vector<V3DSubpassDependencyDesc> m_SubpassDependencies;
	};

	static auto constexpr MAX_SUBPASS = 4;
	typedef std::map<GraphicsPipelienDesc, GraphicsPipelineHandle> PipelineHandleMap;

	struct Stage
	{
		GraphicsRenderPassHandle renderPassHandle;
		Array2<GraphicsManager::PipelineHandleMap, GraphicsManager::MAX_SUBPASS, GRAPHICS_PIPELINE_TYPE_COUNT> pipelineHandleMaps;
	};

	IV3DDevice* m_pDevice;
	IV3DQueue* m_pQueue;
	IV3DCommandBuffer* m_pCommandBuffer;
	IV3DFence* m_pFence;
	IV3DSwapChain* m_pSwapChain;
	bool m_Lost;

	Array1<IV3DShaderModule*, GraphicsManager::SHADER_TYPE_COUNT> m_pShaderModules;
	Array1<IV3DDescriptorSetLayout*, GraphicsManager::DESCRIPTOR_SET_TYPE_COUNT> m_pDescriptorSetLayouts;
	Array1<IV3DPipelineLayout*, GRAPHICS_PIPELINE_TYPE_COUNT> m_pPipelineLayouts;

	std::vector<GraphicsManager::Attachment> m_Attachments;
	Array1<GraphicsManager::RenderPassDesc, GRAPHICS_RENDERPASS_TYPE_COUNT> m_RenderPassDescs;
	Array2<GraphicsManager::PipelineDesc, GRAPHICS_RENDERPASS_TYPE_COUNT, GRAPHICS_PIPELINE_TYPE_COUNT> m_PipelineDescs;

	Array1<GraphicsManager::Stage, GRAPHICS_RENDERPASS_TYPE_COUNT> m_Stages;

	V3D_RESULT CreateNativeRenderPass(uint32_t renderpassType, IV3DRenderPass** ppRenderPass);
	V3D_RESULT CreateNativePipeline(uint32_t renderpassType, uint32_t subpass, uint32_t pipelineType, const GraphicsPipelienDesc& desc, IV3DPipeline** ppPipeline);

	// ----------------------------------------------------------------------------------------------------

	IV3DDevice* GetDevicePtr();
	IV3DQueue* GetQueuePtr();
	IV3DCommandBuffer* GetCommandBufferPtr();
	IV3DFence* GetFencePtr();

	IV3DCommandBuffer* ResetCommandBuffer();
	bool FlushCommandBuffer();

	friend class TextureManager;
	friend class MaterialManager;
	friend class Mesh;
	friend class SkyDome;
	friend class StaticMesh;
	friend class MeshManager;
};
