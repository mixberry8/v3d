#pragma once

class Font
{
public:
	Font();
	~Font();

	bool Initialize(IV3DDevice* pDevice, const wchar_t* pFontName, int32_t height, int32_t weight);
	void Dispose();

	void Lost();
	bool Restore(IV3DRenderPass* pRenderPass, uint32_t subpass);

	void Reset();
	bool Bake(IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence); // レンダーパス内では実行しないこと
	void Flush(IV3DCommandBuffer* pCommandBuffer, uint32_t width, uint32_t height);

	bool AddText(int32_t x, int32_t y, const wchar_t* pText);

private:
	static constexpr uint32_t DescriptorSetPoolSize = 4;
	static constexpr uint32_t MaxCharacter = 4096;
	static constexpr uint32_t ImageSize = 512;
	static constexpr float InvImageSizeF = 1.0f / static_cast<float>(ImageSize);
	static constexpr int32_t TabSize = 4;
	static constexpr int32_t Margin = 2;
	static constexpr MAT2 Matrix = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };

	struct Vertex
	{
		Vector4 pos;
		Vector2 uv;
	};

	struct Character
	{
		wchar_t code;
		int32_t dx;
		int32_t dy;
		int32_t sx;
		int32_t sy;
		GLYPHMETRICS glyphMetrics;
		uint32_t glyphBufferSize;
	};

	struct Block
	{
		bool isInit;

		IV3DImage* pHostImage;
		IV3DImage* pDeviceImage;
		IV3DImageView* pDeviceImageView;
		IV3DDescriptorSet* pDescriptorSet;

		IV3DBuffer* pVertexBuffer;
		IV3DBufferView* pVertexBufferView;

		std::vector<Character> characters;

		int32_t imageX;
		int32_t imageY;
		int32_t imageLine;

		uint32_t vertexCount;
	};

	HFONT m_Handle;

	size_t m_BlockCharacterMax;

	int32_t m_Height;
	int32_t m_HalfHeight;
	V3D_FORMAT m_ImageFormat;

	std::vector<uint8_t> m_GlyphBuffer;
	std::vector<Block> m_Blocks;
	uint32_t m_BlockCount;

	IV3DDevice* m_pDevice;
	IV3DShaderModule* m_pVertShaderModule;
	IV3DShaderModule* m_pFragShaderModule;
	IV3DDescriptorSetLayout* m_pDescriptorSetLayout;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DPipeline* m_pPipeline;
	IV3DSampler* m_pSampler;

	Font::Block* GetCurrentBlock();
	Font::Block* AddBlock();
};
