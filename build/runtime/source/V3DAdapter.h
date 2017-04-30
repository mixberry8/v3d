#pragma once

class V3DAdapter : public IV3DAdapter
{
public:
	struct Source
	{
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProps;
	};

	static V3DAdapter* Create();
	V3D_RESULT Initialize(const VkPhysicalDevice physicalDevice);
	V3DAdapter::Source& GetSource();

	uint32_t Vulkan_GetMemoryTypeIndex(VkMemoryPropertyFlags memoryPropertyFlags);

	/**************************/
	/* override - IV3DAdapter */
	/**************************/
	virtual const V3DAdapterDesc& GetDesc() const override;
	virtual uint32_t GetMemoryHeapCount() const override;
	virtual const V3DMemoryHeapDesc& GetMemoryHeapDesc(uint32_t memoryHeap) const override;
	virtual uint32_t GetMemoryTypeCount() const override;
	virtual const V3DMemoryTypeDesc& GetMemoryTypeDesc(uint32_t memoryType) const override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	ReferenceCounter m_RefCounter;
	V3DAdapterDesc m_Desc;
	STLVector<V3DMemoryHeapDesc> m_MemoryHeaps;
	STLVector<V3DMemoryTypeDesc> m_MemoryTypes;
	V3DAdapter::Source m_Source;

	V3DAdapter();
	virtual ~V3DAdapter();

	V3D_DECLARE_ALLOCATOR
};
