#pragma once

class ObjParser
{
public:
	ObjParser(size_t memorySize, const void* pMemory) :
		m_Pos(0)
	{
		m_MemorySize = memorySize;
		m_pMemory = static_cast<const char*>(pMemory);
	}

	bool Read()
	{
		if (m_MemorySize == m_Pos)
		{
			return false;
		}

		m_LineLength = 0;
		m_LinePos = 0;

		// ----------------------------------------------------------------------------------------------------
		// 長さを図る
		// ----------------------------------------------------------------------------------------------------

		const char* pCode = &m_pMemory[m_Pos];
		uint32_t slashCount = 0;

		do
		{
			if (*pCode == '/')
			{
				slashCount++;
			}

			m_LineLength++;

		} while ((*pCode++ != '\n') && (m_MemorySize != (m_Pos + m_LineLength)));

		// ----------------------------------------------------------------------------------------------------
		// 一行読み込む
		// ----------------------------------------------------------------------------------------------------

		m_Temp.clear();
		m_Temp.resize(m_LineLength + 1 + slashCount * 2, '\0');
		memcpy_s(m_Temp.data(), m_LineLength, &m_pMemory[m_Pos], m_LineLength);

		m_Pos += m_LineLength;

		// ----------------------------------------------------------------------------------------------------
		// 不要な文字コードを '\0' で塗りつぶす
		// ----------------------------------------------------------------------------------------------------

		char* pDstBegin = m_Temp.data();
		char* pDst = pDstBegin;
		char* pDstEnd = pDst + m_Temp.size();

		uint32_t top = 0;

		while (pDst != pDstEnd)
		{
			if ((*pDst == '\r') || (*pDst == '\n') || (*pDst == ' ') || (*pDst == '\t'))
			{
				*pDst = '\0';
			}
			else if (*pDst == '/')
			{
				size_t copySize = m_LineLength - top;
				memcpy_s(pDst + 1, copySize, pDst, copySize);

				copySize = m_LineLength - top - 1;
				memcpy_s(pDst + 2, copySize, pDst + 1, copySize);

				*pDst = '\0';
				*(pDst + 2) = '\0';

				pDst += 2;
			}

			pDst++;
			top++;
 		}

		// ----------------------------------------------------------------------------------------------------

		m_LineLength += slashCount * 2;

		return true;
	}

	const char* Pop()
	{
		if (m_LinePos == m_LineLength)
		{
			return nullptr;
		}

		const char* pSrc = m_Temp.data() + m_LinePos;
		const char* pSrcEnd = pSrc + (m_Temp.size() - m_LinePos);

		const char* pRet = nullptr;

		while (pSrc != pSrcEnd)
		{
			if (*pSrc != '\0')
			{
				pRet = pSrc;
				break;
			}

			m_LinePos++;
			pSrc++;
		}

		while ((*pSrc != '\0') && (pSrc != pSrcEnd))
		{
			m_LinePos++;
			pSrc++;
		}

		return pRet;
	}

	const char* Look()
	{
		if (m_LinePos == m_LineLength)
		{
			return nullptr;
		}

		const char* pSrc = m_Temp.data() + m_LinePos;
		const char* pSrcEnd = pSrc + (m_Temp.size() - m_LinePos);

		while (pSrc != pSrcEnd)
		{
			if (*pSrc != '\0')
			{
				return pSrc;
			}

			pSrc++;
		}

		return nullptr;
	}

private:
	size_t m_Pos;
	size_t m_MemorySize;
	const char* m_pMemory;
	std::vector<char> m_Temp;
	size_t m_LinePos;
	size_t m_LineLength;
};
