#pragma once

class V3DResourceMemory;

class IV3DImageBase : public IV3DImage
{
public:
	struct Source
	{
		VkImage image;
		VkImageAspectFlags aspectFlags;
		VkDeviceSize memoryOffset;
	};

	virtual const IV3DImageBase::Source& GetSource() const = 0;

	virtual bool CheckBindMemory() const = 0;
	virtual V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset) = 0;

protected:
	virtual ~IV3DImageBase() {}
};
