#pragma once

class V3DInstance;

class V3DAdapter : public IV3DAdapter
{
public:
	struct Source
	{
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProps;
	};

	static V3DAdapter* Create();
	V3D_RESULT Initialize(V3DInstance* pInstance, const VkPhysicalDevice physicalDevice);
	V3DAdapter::Source& GetSource();

	uint32_t Vulkan_GetMemoryTypeIndex(VkMemoryPropertyFlags memoryPropertyFlags);

	/**************************/
	/* override - IV3DAdapter */
	/**************************/
	const V3DAdapterDesc& GetDesc() const override;
	uint32_t GetMemoryHeapCount() const override;
	const V3DMemoryHeapDesc& GetMemoryHeapDesc(uint32_t memoryHeap) const override;
	uint32_t GetMemoryTypeCount() const override;
	const V3DMemoryTypeDesc& GetMemoryTypeDesc(uint32_t memoryType) const override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	int64_t GetRefCount() const override;
	void AddRef() override;
	void Release() override;

private:
	ReferenceCounter m_RefCounter;
	V3DAdapterDesc m_Desc;
	STLVector<V3DMemoryHeapDesc> m_MemoryHeaps;
	STLVector<V3DMemoryTypeDesc> m_MemoryTypes;
	V3DAdapter::Source m_Source;

#ifdef V3D_DEBUG
	V3DInstance* m_pInstance;
#endif //V3D_DEBUG

	V3DAdapter();
	virtual ~V3DAdapter();

	V3D_DECLARE_ALLOCATOR
};
