#pragma once

class Window;

struct ApplicationDesc
{
	V3D_LAYER_TYPE layerType;
	int32_t fps;
};

class Application
{
public:
	Application();
	virtual ~Application();

	virtual void OnPreCreate(ApplicationDesc& desc) {}
	virtual bool OnInitialize() { return true; }
	virtual void OnFinalize() {}
	virtual void OnIdle() {}

	int32_t Execute();

	static IV3DInstance* GetInstance();
	static IV3DAdapter* GetAdapter();
	static IV3DDevice* GetDevice();

	static uint32_t FindQueueFamily(V3DFlags queueFlags, uint32_t minQueueCount);

	static double GetFps();
	static double GetDeltaTime();

private:
	static constexpr uint32_t DefTimeSize = 10;

	static Application* s_pThis;

	bool m_SetTimePeriod;
	bool m_ReqResetFps;
	LARGE_INTEGER m_StartFrameCount;
	LONGLONG m_SleepErrorCount;
	uint32_t m_DefTimeHead;
	double m_DefTimes[DefTimeSize];
	double m_SumTimes;
	double m_Fps;
	double m_DeltaTime;

	IV3DInstance* m_pInstance;
	IV3DAdapter* m_pAdapter;
	IV3DDevice* m_pDevice;

	std::vector<Window*> m_Windows;
	
	static void V3D_CALLBACK LogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData);

private:
	static bool AddWindow(Window* pWindow);
	static bool RemoveWindow(Window* pWindow);

	friend class Window;
};
