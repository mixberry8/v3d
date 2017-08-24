#pragma once

class V3DResourceMemory;

class IV3DImageBase : public IV3DImage
{
public:
	struct Source
	{
		VkImage image;
		VkImageAspectFlags aspectFlags;
		uint32_t levelCount;
		uint32_t layerCount;
		VkDeviceSize memoryOffset;
	};

	virtual const IV3DImageBase::Source& GetSource() const = 0;

	virtual bool CheckBindMemory() const = 0;
	virtual V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset) = 0;

#ifdef V3D_DEBUG
	virtual const wchar_t* GetDebugName() const = 0;
#endif //V3D_DEBUG

protected:
	virtual ~IV3DImageBase() {}
};
