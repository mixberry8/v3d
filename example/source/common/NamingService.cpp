#include "NamingService.h"

NamingService::NamingService()
{
}

NamingService::~NamingService()
{
}

void NamingService::Add(const wchar_t* pName, std::wstring& newName)
{
	int32_t charCount = static_cast<int32_t>(wcslen(pName));
	int32_t charIndex = 0;

	for (charIndex = (charCount - 1); charIndex >= 0; charIndex--)
	{
		if (pName[charIndex] == L'|')
		{
			break;
		}
	}

	ASSERT(charIndex != (charCount - 1));

	std::wstring name = pName;

	if (charIndex >= 0)
	{
		name[charIndex] = L'\0';
	}

	NamingService::Info* pInfo;

	NamingService::InfoMap::iterator it = m_InfoMap.find(name.c_str());
	if (it != m_InfoMap.end())
	{
		pInfo = &it->second;
	}
	else
	{
		m_InfoMap[name.c_str()] = NamingService::Info{};
		pInfo = &m_InfoMap.find(pName)->second;
	}

	uint64_t number;

	if (pInfo->unusedNumbers.empty() == false)
	{
		number = pInfo->unusedNumbers.back();
		pInfo->unusedNumbers.pop_back();
	}
	else
	{
		number = pInfo->count++;
	}

	if (number != 0)
	{
		std::wstringstream stringStream;
		stringStream << name.c_str() << L"|" << number;

		newName = stringStream.str();
	}
	else
	{
		newName = name;
	}
}

void NamingService::Remove(const wchar_t* pName)
{
	int32_t charCount = static_cast<int32_t>(wcslen(pName));
	int32_t charIndex = 0;

	for (charIndex = (charCount - 1); charIndex >= 0; charIndex--)
	{
		if (pName[charIndex] == L'|')
		{
			break;
		}
	}

	ASSERT(charIndex != (charCount - 1));

	std::wstring name = pName;

	if (charIndex >= 0)
	{
		name[charIndex] = L'\0';
	}

	NamingService::InfoMap::iterator it = m_InfoMap.find(name.c_str());
	if (it == m_InfoMap.end())
	{
		ASSERT(0);
		return;
	}

	uint64_t number;

	if (charIndex >= 0)
	{
		std::wstring temp = &pName[charIndex + 1];
		number = _wtoi64(&pName[charIndex + 1]);
		ASSERT(number != 0);
	}
	else
	{
		number = 0;
	}

	it->second.unusedNumbers.push_back(number);
}
