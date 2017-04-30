#pragma once

#include "IInputStream.h"

class MemoryInputStream : public IInputStream
{
public:
	MemoryInputStream(uint64_t memorySize, const void* pMemory) :
		m_MemorySize(memorySize),
		m_pMemory(reinterpret_cast<const uint8_t*>(pMemory)),
		m_Position(0)
	{
	}

	virtual ~MemoryInputStream() {}

	virtual uint64_t Seek(SEEK_METHOD method, uint64_t offset) override
	{
		switch (method)
		{
		case SEEK_METHOD_BEGIN:
			if (m_MemorySize >= offset)
			{
				m_Position = offset;
			}
			else
			{
				m_Position = m_MemorySize;
			}
			break;

		case SEEK_METHOD_CURRENT:
			if ((m_Position + offset) <= m_MemorySize)
			{
				m_Position += offset;
			}
			else
			{
				m_Position = m_MemorySize;
			}
			break;

		case SEEK_METHOD_END:
			if (m_Position >= offset)
			{
				m_Position -= offset;
			}
			else
			{
				m_Position = 0;
			}
			break;
		}

		return m_Position;
	}

	virtual uint64_t Read(uint64_t readSize, void* pReadMemory) override
	{
		uint64_t nextPosition = m_Position + readSize;
		if (m_MemorySize < nextPosition)
		{
			readSize -= (nextPosition - m_MemorySize);
		}

		memcpy_s(pReadMemory, readSize, &m_pMemory[m_Position], readSize);
		m_Position += readSize;

		return readSize;
	}

private:
	uint64_t m_MemorySize;
	const uint8_t* m_pMemory;
	uint64_t m_Position;
};
