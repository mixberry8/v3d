#pragma once

enum SEEK_METHOD
{
	SEEK_METHOD_BEGIN = 0,
	SEEK_METHOD_CURRENT = 1,
	SEEK_METHOD_END = 2,
};

class IInputStream
{
public:
	virtual uint64_t Seek(SEEK_METHOD method, uint64_t offset) = 0;
	virtual uint64_t Read(uint64_t readSize, void* pReadBuffer) = 0;

protected:
	virtual ~IInputStream() {}
};
