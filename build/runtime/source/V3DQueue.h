#pragma once

class V3DDevice;

class V3DQueue : public IV3DQueue
{
public:
	struct Source
	{
		VkQueue queue;
	};

	static V3DQueue* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, uint32_t family, VkQueue queue, const wchar_t* pDebugName);
	void Dispose();

	const V3DQueue::Source& GetSource() const;

	/*******************************/
	/* override - IV3DQueue */
	/*******************************/
	virtual uint32_t GetFamily() const override;
	virtual V3D_RESULT Submit(uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) override;
	virtual V3D_RESULT Submit(IV3DSwapChain* pSwapChain, uint32_t commandBufferCount, IV3DCommandBuffer** ppCommandBuffers, IV3DFence* pFence) override;
	virtual V3D_RESULT Present(IV3DSwapChain* pSwapChain) override;
	virtual V3D_RESULT WaitIdle() override;

	/******************************/
	/* override - IV3DDeviceChild */
	/******************************/
	virtual void GetDevice(IV3DDevice** ppDevice) override;

	/*************************/
	/* override - IV3DObject */
	/*************************/
	virtual int64_t GetRefCount() const override;
	virtual void AddRef() override;
	virtual void Release() override;

private:
	struct Temp
	{
		STLVector<VkCommandBuffer> commandBuffers;
	};

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	uint32_t m_Family;
	V3DQueue::Source m_Source;
	V3DQueue::Temp m_Temp;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DQueue();
	virtual ~V3DQueue();

	V3D_DECLARE_ALLOCATOR
};