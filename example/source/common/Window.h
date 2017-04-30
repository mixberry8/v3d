#pragma once

class Window
{
public:
	Window();
	virtual ~Window();

	virtual bool Initialize(const wchar_t* pCaption, uint32_t width, uint32_t height, IV3DQueue* pWorkQueue, IV3DQueue* pGraphicsQueue);

	bool Restart();
	void Resize(uint32_t width, uint32_t height);

	HWND GetHandle() const;

	IV3DSwapChain* GetSwapChain() const;

	uint32_t GetImageCount() const;
	IV3DImage* GetImage(uint32_t index) const;
	IV3DImage* GetCurrentImage() const;
	bool CreateImageView(uint32_t index, IV3DImageView** ppImageView);

	IV3DQueue* GetWorkQueue();
	IV3DCommandBuffer* GetWorkCommandBuffer();
	IV3DFence* GetWorkFence();

	IV3DCommandBuffer* GetCurrentGraphicsCommandBuffer();

	// ----------------------------------------------------------------------------------------------------
	// Work
	// ----------------------------------------------------------------------------------------------------

	IV3DCommandBuffer* BeginWork();
	bool EndWork();

	void SaveScreenshot(const wchar_t* pFileName);

	// ----------------------------------------------------------------------------------------------------
	// Graphics
	// ----------------------------------------------------------------------------------------------------

	IV3DCommandBuffer* BeginGraphics();
	bool EndGraphics();

	IV3DCommandBuffer* BeginGraphicsEx();
	bool EndGraphicsEx();

protected:
	virtual bool OnInitialize() { return true; }
	virtual void OnFinalize() {}
	virtual bool OnIdle() { return true; }
	virtual void OnLostSwapChain() {}
	virtual bool OnRestoreSwapChain() { return true; }
	virtual bool OnMessage(UINT message, WPARAM wparam, LPARAM lparam);

	// keyFlags
	//   MK_CONTROL
	//   MK_LBUTTON
	//   MK_MBUTTON
	//   MK_RBUTTON
	//   MK_SHIFT
	//   MK_XBUTTON1
	//   MK_XBUTTON2
	virtual bool OnMouseButtonDown(WPARAM keyFlags, const POINT& pos) { return true; }
	virtual bool OnMouseButtonUp(WPARAM keyFlags, const POINT& pos) { return true; }
	virtual bool OnMouseMove(WPARAM keyFlags, const POINT& pos) { return true; }
	virtual bool OnMouseWheel(WPARAM keyFlags, int16_t delta, const POINT& pos) { return true; }

	virtual bool OnKeyDown(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) { return true; }
	virtual bool OnKeyUp(WPARAM keyCode, uint16_t repeatCount, uint8_t scanCode, bool extendedKey, bool prevKeyState) { return true; }

private:
	static constexpr uint32_t BufferCount = 2;

	struct Frame
	{
		IV3DCommandBuffer* pGraphicsCommandBuffer;
		IV3DFence* pGraphicsFence;
		bool initFence;
	};

	HWND m_WindowHandle;
	IV3DSwapChain* m_pSwapChain;

	IV3DQueue* m_pWorkQueue;
	IV3DCommandPool* m_pWorkCommandPool;
	IV3DCommandBuffer* m_pWorkCommandBuffer;
	IV3DFence* m_pWorkFence;

	IV3DQueue* m_pGraphicsQueue;
	IV3DCommandPool* m_pGraphicsCommandPool;
	std::vector<Window::Frame> m_Frames;

	std::vector<IV3DImage*> m_Images;

	bool m_ScreenshotRequest;
	std::wstring m_ScreenshotFilePath;

	void Halt();
	void Dispose();

	V3D_RESULT CreateSwapChainResources();
	void ReleaseSwapChainResources();

	void LostSwapChain();
	void RestoreSwapChain();

	void SaveScreenshot();

	static LRESULT CALLBACK WindowProc(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);
	static void V3D_CALLBACK LostSwapChainFunction(void* pUserData);
	static void V3D_CALLBACK RestoreSwapChainFunction(void* pUserData);

private:
	void Process();

	friend class Application;
};
