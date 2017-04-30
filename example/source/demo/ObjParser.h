#pragma once

class ObjParser
{
public:
	struct ReadResult
	{
		const char* pToken;
		char separator;
	};

	ObjParser(size_t memorySize, const void* pMemory) :
		m_Pos(0)
	{
		m_MemorySize = memorySize;
		m_pMemory = static_cast<const char*>(pMemory);
	}

	ObjParser::ReadResult Read()
	{
		ObjParser::ReadResult result{};

		if (m_MemorySize == m_Pos)
		{
			return result;
		}

		bool continueLoop = false;

		do
		{
			if (m_pMemory[m_Pos] == '#')
			{
				while ((m_MemorySize != m_Pos) && (m_pMemory[m_Pos] != '\n'))
				{
					m_Pos++;
				}

				if (m_MemorySize != m_Pos)
				{
					continueLoop = true;
					m_Pos++;
				}
			}
			else
			{
				size_t nextPos = m_Pos;

				while ((m_MemorySize != nextPos) && ((m_pMemory[nextPos] != ' ') && (m_pMemory[nextPos] != '/') && (m_pMemory[nextPos] != '\n')))
				{
					nextPos++;
				}

				if (m_Pos != nextPos)
				{
					size_t tokenSize = nextPos - m_Pos;

					m_Temp.resize(tokenSize + 1);
					memcpy_s(m_Temp.data(), m_Temp.size(), &m_pMemory[m_Pos], tokenSize);
					m_Temp[tokenSize] = '\0';

					result.pToken = m_Temp.data();
					result.separator = (m_MemorySize != nextPos) ? m_pMemory[nextPos] : '\n';

					continueLoop = false;
				}
				else
				{
					if (m_MemorySize != nextPos)
					{
						continueLoop = true;
					}
				}

				m_Pos = (m_MemorySize != nextPos) ? (nextPos + 1) : nextPos;
			}
		}
		while (continueLoop == true);

		return result;
	}

private:
	size_t m_Pos;
	size_t m_MemorySize;
	const char* m_pMemory;
	std::vector<char> m_Temp;
};
