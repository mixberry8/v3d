#pragma once

// ----------------------------------------------------------------------------------------------------
// 構造体
// ----------------------------------------------------------------------------------------------------

// ディレクショナルライト
struct DirectionalLight
{
	Vector4 color;
	Vector3 dir;
	uint32_t reserve[1];
};

static auto constexpr DIRECTIONAL_LIGHT_MAX = 32;

struct DirectionalLightingUniform
{
	Vector3 eyePos;
	uint32_t reserve0[1];

	uint32_t directionalLightCount;
	uint32_t reserve1[3];

	DirectionalLight directionalLights[DIRECTIONAL_LIGHT_MAX];
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
// レンダーパスタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_RENDERPASS_TYPE
{
	GRAPHICS_RENDERPASS_TYPE_DFFERED_GEOMETRY = 0, // ディファード - ジオメトリ
};

static auto constexpr GRAPHICS_RENDERPASS_TYPE_COUNT = 1;

// ----------------------------------------------------------------------------------------------------
// アタッチメントタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_ATTACHMENT_TYPE
{
	// ディファード - ジオメトリ
	GRAPHICS_ATTACHMENT_TYPE_DG_COLOR = 0,
	GRAPHICS_ATTACHMENT_TYPE_DG_BUFFER_0 = 1,
	GRAPHICS_ATTACHMENT_TYPE_DG_BUFFER_1 = 2,
	GRAPHICS_ATTACHMENT_TYPE_DG_DEPTH_STENCIL = 3,
	GRAPHICS_ATTACHMENT_TYPE_DG_LIGHT = 4,
	GRAPHICS_ATTACHMENT_TYPE_DG_BACK_BUFFER = 5,
};

// ----------------------------------------------------------------------------------------------------
// パイプラインタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_PIPELINE_TYPE
{
	GRAPHICS_PIPELINE_TYPE_SIMPLE = 0,
	GRAPHICS_PIPELINE_TYPE_GEOMETRY = 1,
	GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING = 2,
	GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING = 3,
	GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING = 4,
};

static auto constexpr GRAPHICS_PIPELINE_TYPE_COUNT = 5;

// ----------------------------------------------------------------------------------------------------
// デスクリプタセットタイプ
// ----------------------------------------------------------------------------------------------------

enum GRAPHICS_DESCRIPTOR_SET_TYPE
{
	// GRAPHICS_PIPELINE_TYPE_GEOMETRY
	GRAPHICS_DESCRIPTOR_SET_TYPE_MESH = 0,
	GRAPHICS_DESCRIPTOR_SET_TYPE_MATERIAL = 1,

	// GRAPHICS_PIPELINE_TYPE_DIRECTIONAL_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_DIRECTIONAL_LIGHTING = 0,

	// GRAPHICS_PIPELINE_TYPE_POINT_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_POINT_LIGHTING = 0,

	// GRAPHICS_PIPELINE_TYPE_FINISH_LIGHTING
	GRAPHICS_DESCRIPTOR_SET_TYPE_FINISH_LIGHTING = 0,
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
		if (polygonMode < rh.polygonMode)
		{
			return true;
		}

		if (cullMode < rh.cullMode)
		{
			return true;
		}

		if (depthTestEnable < rh.depthTestEnable)
		{
			return true;
		}

		if (depthWriteEnable < rh.depthWriteEnable)
		{
			return true;
		}

		if (blendMode < rh.blendMode)
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
	GraphicsManager();
	~GraphicsManager();

	V3D_RESULT Initialize(IV3DDevice* pDevice, IV3DQueue* pQueue, IV3DSwapChain* pSwapChain);
	void Finalize();

	void Lost();
	V3D_RESULT Restore();

	void GetPipelineLayout(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		GRAPHICS_PIPELINE_TYPE pipelineType,
		IV3DPipelineLayout** ppPipelineLayout);

	V3D_RESULT GetPipelineHandle(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		GRAPHICS_PIPELINE_TYPE pipelineType,
		const GraphicsPipelienDesc& desc,
		GraphicsPipelineHandle* pPipelineHandle);

	V3D_RESULT GetRenderPassHandle(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		GraphicsRenderPassHandle* pRenderPassHandle);

	V3D_RESULT CreateFrameBuffer(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
		uint32_t index,
		IV3DFrameBuffer** ppFrameBuffer);

	V3D_RESULT CreateDescriptorSet(
		GRAPHICS_RENDERPASS_TYPE renderpassType,
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
		SHADER_TYPE_VERT_MESH = 2,
		SHADER_TYPE_FRAG_MESH_D = 3,
		SHADER_TYPE_FRAG_DIRECTIONAL_LIGHTING_D = 4,
		SHADER_TYPE_FRAG_POINT_LIGHTING_D = 5,
		SHADER_TYPE_FRAG_FINISH_LIGHTING_D = 6,
	};
	static auto constexpr SHADER_TYPE_COUNT = 7;

	enum ATTACHMENT_TYPE
	{
		ATTACHMENT_TYPE_COLOR = 0,
		ATTACHMENT_TYPE_BUFFER_0 = 1,
		ATTACHMENT_TYPE_BUFFER_1 = 2,
		ATTACHMENT_TYPE_DEPTH_STENCIL = 3,

		ATTACHMENT_TYPE_LIGHT = 4,

		ATTACHMENT_TYPE_BACK_BUFFER = 5,
	};
	static auto constexpr ATTACHMENT_TYPE_COUNT = 5; // バックバッファの数は含めない

	struct Attachment
	{
		V3DImageDesc imageDesc;
		IV3DImage* pImage;
		V3D_ACCESS_FLAG imageAccessType;
		V3D_IMAGE_LAYOUT imageLayout;

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
			depthStencil.stencilFront.compareOP = V3D_COMPARE_OP_ALWAYS;
			depthStencil.stencilFront.readMask = 0x000000FF;
			depthStencil.stencilFront.writeMask = 0x000000FF;
			depthStencil.stencilFront.reference = 0;
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

	typedef std::map<GraphicsPipelienDesc, GraphicsPipelineHandle> PipelineMap;

	IV3DDevice* m_pDevice;
	IV3DQueue* m_pQueue;
	IV3DCommandBuffer* m_pCommandBuffer;
	IV3DFence* m_pFence;
	IV3DSwapChain* m_pSwapChain;
	bool m_Lost;

	Array1<IV3DShaderModule*, GraphicsManager::SHADER_TYPE_COUNT> m_pShaderModules;
	Array3<IV3DDescriptorSetLayout*, GRAPHICS_RENDERPASS_TYPE_COUNT, GRAPHICS_PIPELINE_TYPE_COUNT, GRAPHICS_DESCRIPTOR_SET_TYPE_COUNT> m_pDescriptorSetLayouts;
	Array2<IV3DPipelineLayout*, GRAPHICS_RENDERPASS_TYPE_COUNT, GRAPHICS_PIPELINE_TYPE_COUNT> m_pPipelineLayouts;

	std::vector<GraphicsManager::Attachment> m_Attachments;

	Array1<GraphicsRenderPassHandle, GRAPHICS_RENDERPASS_TYPE_COUNT> m_RenderPasses;

	Array2<GraphicsManager::PipelineDesc, GRAPHICS_RENDERPASS_TYPE_COUNT, GRAPHICS_PIPELINE_TYPE_COUNT> m_PipelineDescs;
	Array2<GraphicsManager::PipelineMap, GRAPHICS_RENDERPASS_TYPE_COUNT, GRAPHICS_PIPELINE_TYPE_COUNT> m_PipelineMap;

	V3D_RESULT CreateNativeRenderPass(uint32_t renderpassType, IV3DRenderPass** ppRenderPass);
	V3D_RESULT CreateNativePipeline(uint32_t renderpassType, uint32_t pipelineType, const GraphicsPipelienDesc& desc, IV3DPipeline** ppPipeline);

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
	friend class MeshManager;
};
