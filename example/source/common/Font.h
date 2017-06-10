#pragma once

class Font
{
public:
	Font();
	~Font();

	bool Initialize(IV3DDevice* pDevice, IV3DQueue* pQueue, IV3DCommandBuffer* pCommandBuffer, IV3DFence* pFence, const wchar_t* pFontName, int32_t height, int32_t weight);
	void Dispose();

	void Lost();
	bool Restore(IV3DRenderPass* pRenderPass, uint32_t subpass, uint32_t screenWidth, uint32_t screenHeight);

	void Reset();
	bool Bake();
	void Flush(IV3DCommandBuffer* pCommandBuffer);

	bool AddText(int32_t x, int32_t y, const wchar_t* pText);

private:
	static constexpr uint32_t DescriptorSetPoolSize = 4;
	static constexpr uint32_t MaxCharacter = 4096;
	static constexpr uint32_t ImageSize = 512;
	static constexpr float InvImageSizeF = 1.0f / static_cast<float>(ImageSize);
	static constexpr int32_t TabSize = 4;
	static constexpr int32_t Margin = 2;
	static constexpr MAT2 Matrix = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
	static constexpr wchar_t FirstCode = 0x0021;
	static constexpr wchar_t LastCode = 0x007E;

	struct Vertex
	{
		Vector4 pos;
		Vector2 uv;
	};

	struct Glyph
	{
		wchar_t code;

		float left;
		float right;
		float top;
		float bottom;

		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;

		int32_t cellWidth;
	};

	struct Character
	{
		int32_t dx;
		int32_t dy;
		const Font::Glyph* pGlyph;
	};

	struct Block
	{
		uint32_t vertexCount;
		uint64_t vertexBufferOffset;
		IV3DBuffer* pVertexBuffer;

		std::vector<Character> characters;
	};

	std::vector<Font::Glyph> m_Glyphs;

	size_t m_BlockCharacterMax;

	int32_t m_Height;
	int32_t m_HalfHeight;

	std::vector<Block> m_Blocks;
	uint32_t m_BlockCount;

	IV3DDevice* m_pDevice;
	IV3DShaderModule* m_pVertShaderModule;
	IV3DShaderModule* m_pFragShaderModule;
	IV3DDescriptorSet* m_pDescriptorSet;
	IV3DPipelineLayout* m_pPipelineLayout;
	IV3DPipeline* m_pPipeline;

	uint32_t m_ScreenWidth;
	uint32_t m_ScreenHeight;

	Font::Block* GetCurrentBlock();
	Font::Block* AddBlock();
};
