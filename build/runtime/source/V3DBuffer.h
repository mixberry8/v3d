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

	struct Subresource
	{
		uint32_t index;
		uint64_t memoryAlignment;
		V3DBufferSubresourceLayout layout;
	};

	static V3DBuffer* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, uint32_t subresourceCount, const V3DBufferSubresourceDesc* pSubresources);

	const Subresource* GetSubresourcesPtr() const;

	const V3DBuffer::Source& GetSource() const;

	bool CheckBindMemory() const;
	V3D_RESULT BindMemory(V3DResourceMemory* pMemory, uint64_t memoryOffset);

	/**************/
	/* IV3DBuffer */
	/**************/
	virtual V3DFlags GetUsageFlags() const override;
	virtual uint32_t GetSubresourceCount() const override;
	virtual const V3DBufferSubresourceLayout& GetSubresourceLayout(uint32_t layer) const override;

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
	V3DFlags m_UsageFlags;
	V3DBuffer::Source m_Source;
	STLVector<V3DBuffer::Subresource> m_Subresources;
	STLVector<uint32_t> m_SubresourcesIndices;

	V3DBuffer();
	virtual ~V3DBuffer();

	V3D_DECLARE_ALLOCATOR
};
