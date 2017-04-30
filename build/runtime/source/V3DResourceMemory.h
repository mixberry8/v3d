#pragma once

class V3DDevice;
class V3DBuffer;

class V3DResourceMemory : public IV3DResourceMemory
{
public:
	struct Source
	{
		VkDeviceMemory deviceMemory;
		VkMemoryPropertyFlags memoryPropertyFlags;
		VkMappedMemoryRange memoryMappedRange;
	};

	static V3DResourceMemory* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint64_t size);
	V3D_RESULT Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint32_t resourceCount, IV3DResource** ppResources);
	const V3DResourceMemory::Source& GetSource() const;

	V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory);
	V3D_RESULT Unmap();

	/**********************/
	/* IV3DResourceMemory */
	/**********************/
	virtual const V3DResourceMemoryDesc& GetDesc() const override;
	virtual V3D_RESULT BeginMap() override;
	virtual V3D_RESULT EndMap() override;

	/*******************/
	/* IV3DDeviceChild */
	/*******************/
	virtual void GetDevice(IV3DDevice** ppDevice) override;

	/**************/
	/* IV3DObject */
	/**************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DResourceMemoryDesc m_Desc;
	V3DResourceMemory::Source m_Source;
	uint8_t* m_pMemory;

	V3DResourceMemory();
	virtual ~V3DResourceMemory();

	V3D_DECLARE_ALLOCATOR
};
