#pragma once

class V3DDevice;
class V3DBackBuffer;

class V3DSwapChain : public IV3DSwapChain
{
public:
	struct Source
	{
		VkSurfaceKHR surface;
		VkSwapchainCreateInfoKHR swapChainCreateInfo;
		VkSwapchainKHR swapChain;
		VkSemaphore presentCompleteSemaphore;
		VkSemaphore renderingCompleteSemaphore;
		uint32_t currentImageIndex;
		VkPipelineStageFlags waitDstStageMask;
	};

	static V3DSwapChain* Create();
	V3D_RESULT Initialize(IV3DDevice* pDevice, const V3DSwapChainDesc& swapChainDesc, const V3DSwapChainCallbacks& swapChainCallbacks, const wchar_t* pDebugName);
	const V3DSwapChain::Source& GetSource() const;
	V3D_RESULT RecreateSwapChain();
	V3D_RESULT RecreateSurfaceAndSwapChain();
	V3D_RESULT SetIconic(bool bIconic);
	V3D_RESULT ResizeImage();

	/*****************/
	/* IV3DSwapChain */
	/*****************/
	virtual const V3DSwapChainDesc& GetDesc() const override;
	virtual V3D_RESULT AcquireNextImage() override;
	virtual uint32_t GetCurrentImageIndex() const override;
	virtual void GetImage(uint32_t imageIndex, IV3DImage** ppImage) override;
	virtual bool GetFullscreenState() const override;
	virtual V3D_RESULT SetFullscreenState(bool bFullscreen) override;

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
	struct WindowedInfo
	{
		HMENU menuHandle;
		uint32_t style;
		RECT rect;
		uint32_t imageWidth;
		uint32_t imageHeight;
	};

	ReferenceCounter m_RefCounter;
	V3DDevice* m_pDevice;
	V3DSwapChainDesc m_InitialDesc;
	V3DSwapChainDesc m_Desc;
	V3DSwapChainCallbacks m_Callbacks;
	STLVector<V3DBackBuffer*> m_Images;
	V3DSwapChain::Source m_Source;
	V3DSwapChain::WindowedInfo m_WindowedInfo;

#ifdef _DEBUG
	STLStringW m_DebugName;
#endif //_DEBUG

	V3DSwapChain();
	virtual ~V3DSwapChain();

	V3D_RESULT CreateSurfaceAndSwapChain();
	V3D_RESULT ChangeFullscreen(bool saveWindowInfo);

	V3D_DECLARE_ALLOCATOR
};
