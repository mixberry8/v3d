#pragma once

class NamingService
{
public:
	NamingService();
	~NamingService();

	void Add(const wchar_t* pName, std::wstring& newName);
	void Remove(const wchar_t* pName);

private:
	struct Info
	{
		uint64_t count;
		std::vector<uint64_t> unusedNumbers;
	};

	typedef std::map<std::wstring, NamingService::Info> InfoMap;
	NamingService::InfoMap m_InfoMap;
};
