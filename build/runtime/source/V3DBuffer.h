#pragma once

class V3DDevice;
class V3DResourceMemory;

class V3DBuffer : public IV3DBuffer
{
public:
	struct Source
	{
		VkBuffer buffer;
		VkDeviceSize memoryOffset;
		VkDeviceSize memorySize;
	};

	static V3DBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DBufferDesc& desc, const wchar_t* pDebugName);
	const V3DBuffer::Source& GetSource() const;

	bool CheckBindMemory() const;
	V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset);

#ifdef _DEBUG
	const wchar_t* GetDebugName() const;
#endif //_DEBUG

	/**************/
	/* IV3DBuffer */
	/**************/
	virtual const V3DBufferDesc& GetDesc() const override;

	/****************/
	/* IV3DResource */
	/****************/
	virtual const V3DResourceDesc& GetResourceDesc() const override;
	virtual V3D_RESULT GetResourceMemory(IV3DResourceMemory** ppMemory) override;
	virtual V3D_RESULT Map(uint64_t offset, uint64_t size, void** ppMemory) override;
	virtual V3D_RESULT Unmap() override;

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
	static constexpr V3DFlags StandardBufferUsageMask = V3D_BUFFER_USAGE_TRANSFER_SRC | V3D_BUFFER_USAGE_TRANSFER_DST | V3D_BUFFER_USAGE_INDEX | V3D_BUFFER_USAGE_VERTEX;
	static constexpr V3DFlags TexelBufferUsageMask = V3D_BUFFER_USAGE_UNIFORM_TEXEL | V3D_BUFFER_USAGE_STORAGE_TEXEL;
	static constexpr V3DFlags UniformBufferUsageMask = V3D_BUFFER_USAGE_UNIFORM;
	static constexpr V3DFlags StorageBufferUsageMask = V3D_BUFFER_USAGE_STORAGE;

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DResourceMemory* m_pMemory;
	V3DResourceDesc m_ResourceDesc;
	V3DBufferDesc m_Desc;
	V3DBuffer::Source m_Source;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DBuffer();
	virtual ~V3DBuffer();

	V3D_DECLARE_ALLOCATOR
};
