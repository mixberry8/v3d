#pragma once

class DynamicOffsetHeap
{
public:
	struct Handle
	{
	public:
		Handle() :
			m_pParent(nullptr),
			m_Offset(DynamicOffsetHeap::INVALID_OFFSET)
		{
		}

		uint32_t GetOffset() const { return m_Offset; }

	private:
		void* m_pParent;
		uint32_t m_Offset;

		friend class DynamicOffsetHeap;
	};

	DynamicOffsetHeap();
	~DynamicOffsetHeap();

	void Initialize(uint32_t count, uint32_t stride);

	bool Retain(DynamicOffsetHeap::Handle* pHandle);
	void Release(DynamicOffsetHeap::Handle* pHandle);

private:
	static constexpr uint32_t INVALID_OFFSET = ~0U;

	uint32_t m_Count;
	uint32_t m_Stride;
	uint32_t m_InuseOffsetCount;
	std::vector<uint32_t> m_UnuseOffsets;
};