#include "Application.h"
#include <Window.h>
#include <mmsystem.h>
#include <vector>
#ifdef _DEBUG
#include <crtdbg.h>
#endif //_DEBUG

Application* Application::s_pThis = nullptr;

Application::Application() :
m_SetTimePeriod(false),
m_ReqResetFps(true),
m_DefTimeHead(0),
m_SumTimes(0),
m_Fps(60.0),
m_DeltaTime(1.0 / m_Fps),
m_pInstance(nullptr),
m_pAdapter(nullptr),
m_pDevice(nullptr)
{
	Application::s_pThis = this;

	for (uint32_t i = 0; i < Application::DefTimeSize; i++)
	{
		m_DefTimes[i] = 0.0;
	}
}

Application::~Application()
{
}

int32_t Application::Execute()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif //_DEBUG

	// ----------------------------------------------------------------------------------------------------
	// アプリケーションの作成の記述
	// ----------------------------------------------------------------------------------------------------

	ApplicationDesc desc{};
#ifdef _DEBUG
	desc.layerType = V3D_LAYER_STANDARD_VALIDATION;
#else //_DEBUG
	desc.layerType = V3D_LAYER_OPTIMUS;
#endif //_DEBUG
	desc.fps = 60;

	OnPreCreate(desc);

	// ----------------------------------------------------------------------------------------------------
	// インスタンスの作成
	// ----------------------------------------------------------------------------------------------------

	V3DInstanceDesc instanceDesc{};
	instanceDesc.layerType = desc.layerType;
	instanceDesc.log.flags = V3D_LOG_PERFORMANCE_WARNING | V3D_LOG_WARNING | V3D_LOG_ERROR;
	instanceDesc.log.pFunction = Application::LogFunction;

	V3D_RESULT result = V3DCreateInstance(instanceDesc, &m_pInstance);
	if (result != V3D_OK)
	{
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// デバイスの作成
	// ----------------------------------------------------------------------------------------------------

	if (m_pInstance->GetAdapterCount() == 0)
	{
		SAFE_RELEASE(m_pInstance);
		return -1;
	}

	m_pInstance->GetAdapter(0, &m_pAdapter);

	result = m_pInstance->CreateDevice(m_pAdapter, &m_pDevice);
	if (result != V3D_OK)
	{
		SAFE_RELEASE(m_pAdapter);
		SAFE_RELEASE(m_pInstance);
		return -1;
	}

	// ----------------------------------------------------------------------------------------------------
	// 初期化
	// ----------------------------------------------------------------------------------------------------

	if (OnInitialize() == false)
	{
		OnFinalize();

		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pAdapter);
		SAFE_RELEASE(m_pInstance);

		return -1;
	}

	bool continueLoop = true;
	MSG msg{};

	timeBeginPeriod(1);

	while(continueLoop == true)
	{
		// ----------------------------------------------------------------------------------------------------
		// FPS
		// ----------------------------------------------------------------------------------------------------

		LARGE_INTEGER perFreq;
		::QueryPerformanceFrequency(&perFreq);

		LARGE_INTEGER curPerfCount;
		::QueryPerformanceCounter(&curPerfCount);

		LARGE_INTEGER framePeriodCount;
		framePeriodCount.QuadPart = (desc.fps > 0)? (perFreq.QuadPart / desc.fps) : 0;

		// Fps : リセット処理
		if (m_ReqResetFps == true)
		{
			m_StartFrameCount.QuadPart = curPerfCount.QuadPart - framePeriodCount.QuadPart;
			m_ReqResetFps = false;
		}

		if (desc.fps > 0)
		{
			// FPS : スリープ処理
			LONGLONG frameSleepCount = framePeriodCount.QuadPart - (curPerfCount.QuadPart - m_StartFrameCount.QuadPart) + m_SleepErrorCount;
			if (frameSleepCount > 0)
			{
				uint32_t frameSleepTime = static_cast<uint32_t>(frameSleepCount * 1000 / perFreq.QuadPart);

				LARGE_INTEGER sleepStartCount;
				LARGE_INTEGER sleepEndCount;

				::QueryPerformanceCounter(&sleepStartCount);
				::Sleep(frameSleepTime);
				::QueryPerformanceCounter(&sleepEndCount);

				// スリープの誤差
				// 指定したスリープ時間よりも長くスリープした場合、値はマイナスになる
				m_SleepErrorCount = frameSleepCount - (sleepEndCount.QuadPart - sleepStartCount.QuadPart);
			}
			else
			{
				::Sleep(0);
				m_SleepErrorCount = 0;
			}
		}

		// Fps : 更新処理
		::QueryPerformanceCounter(&curPerfCount);
		LONGLONG defCount = curPerfCount.QuadPart - m_StartFrameCount.QuadPart;
		double defTime = static_cast<double>(defCount * 1000) / static_cast<double>(perFreq.QuadPart);
		if (defTime < FLOAT_EPSILON)
		{
			m_Fps = 0.0;
		}
		else
		{
			// 古いデータを削除
			m_DefTimeHead = (m_DefTimeHead + 1) % Application::DefTimeSize;

			// 新しいデータを追加
			uint32_t defTimeTail = (m_DefTimeHead + Application::Application::DefTimeSize - 1) % Application::Application::DefTimeSize;
			m_DefTimes[defTimeTail] = defTime;

			double aveDef = (m_SumTimes + defTime) / Application::Application::DefTimeSize;
			if (aveDef > FLOAT_EPSILON)
			{
				m_Fps = 1000.0 / aveDef;
			}

			// 共通加算部分の更新
			m_SumTimes += defTime - m_DefTimes[m_DefTimeHead];
		}

		m_DeltaTime = (m_Fps > FLOAT_EPSILON) ? FLOAT_RECIPROCAL(m_Fps) : 1.0;

		::QueryPerformanceCounter(&m_StartFrameCount);

		// ----------------------------------------------------------------------------------------------------
		// メッセージポンプ
		// ----------------------------------------------------------------------------------------------------

		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE)
		{
			if (GetMessage(&msg, NULL, 0, 0) == TRUE)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				continueLoop = false;
				break;
			}
		}

		if (continueLoop == true)
		{
			OnIdle();

			std::vector<Window*>::iterator it_window_begin = m_Windows.begin();
			std::vector<Window*>::iterator it_window_end = m_Windows.end();
			std::vector<Window*>::iterator it_window;

			for (it_window = it_window_begin; it_window != it_window_end; ++it_window)
			{
				(*it_window)->Process();
			}
		}
	}

	timeEndPeriod(1);

	// ----------------------------------------------------------------------------------------------------

	OnFinalize();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pAdapter);
	SAFE_RELEASE(m_pInstance);

	return static_cast<int32_t>(msg.wParam);
}

IV3DInstance* Application::GetInstance()
{
	return s_pThis->m_pInstance;
}

IV3DDevice* Application::GetDevice()
{
	return s_pThis->m_pDevice;
}

IV3DAdapter* Application::GetAdapter()
{
	return s_pThis->m_pAdapter;
}

double Application::GetDeltaTime()
{
	return s_pThis->m_DeltaTime;
}

uint32_t Application::FindQueueFamily(V3DFlags queueFlags, uint32_t minQueueCount)
{
	uint32_t queueFamilyCount = s_pThis->m_pDevice->GetQueueFamilyCount();

	// ----------------------------------------------------------------------------------------------------
	// 指定したキューのみを持つファミリを探してみる
	// ----------------------------------------------------------------------------------------------------

	uint32_t queueFamily = ~0U;

	for (uint32_t i = 0; (i < queueFamilyCount) && (queueFamily == ~0U); i++)
	{
		const V3DDeviceQueueFamilyDesc& desc = s_pThis->m_pDevice->GetQueueFamilyDesc(i);

		if (((desc.queueFlags & ~queueFlags) == 0) && (desc.queueCount >= minQueueCount))
		{
			queueFamily = i;
		}
	}

	if (queueFamily != ~0U)
	{
		return queueFamily;
	}

	// ----------------------------------------------------------------------------------------------------
	// 指定したキューを持っているファミリを探してみる ( 他のキューもサポートしている )
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; (i < queueFamilyCount) && (queueFamily == ~0U); i++)
	{
		const V3DDeviceQueueFamilyDesc& desc = s_pThis->m_pDevice->GetQueueFamilyDesc(i);

		if (((desc.queueFlags & queueFlags) == queueFlags) && (desc.queueCount >= minQueueCount))
		{
			queueFamily = i;
		}
	}

	return queueFamily;
}

double Application::GetFps()
{
	return s_pThis->m_Fps;
}

void V3D_CALLBACK Application::LogFunction(V3D_LOG_FLAG type, const wchar_t* pMessage, void* pUserData)
{
	wchar_t flagText[32];
	switch (type)
	{
	case V3D_LOG_INFORMATION:
		::wcscpy_s(flagText, L"( infomation ) : ");
		break;
	case V3D_LOG_WARNING:
		::wcscpy_s(flagText, L"( warning ) : ");
		break;
	case V3D_LOG_PERFORMANCE_WARNING:
		::wcscpy_s(flagText, L"( performance - warning ) : ");
		break;
	case V3D_LOG_ERROR:
		::wcscpy_s(flagText, L"( error ) : ");
		break;
	case V3D_LOG_DEBUG:
		::wcscpy_s(flagText, L"( debug ) : ");
		break;
	}

	::OutputDebugStringW(flagText);
	::OutputDebugStringW(pMessage);
}

bool Application::AddWindow(Window* pWindow)
{
	std::vector<Window*>::iterator it_window = std::find(s_pThis->m_Windows.begin(), s_pThis->m_Windows.end(), pWindow);
	if (it_window != s_pThis->m_Windows.end())
	{
		return false;
	}

	s_pThis->m_Windows.push_back(pWindow);

	return true;
}

bool Application::RemoveWindow(Window* pWindow)
{
	std::vector<Window*>::iterator it_window = std::find(s_pThis->m_Windows.begin(), s_pThis->m_Windows.end(), pWindow);
	if (it_window != s_pThis->m_Windows.end())
	{
		s_pThis->m_Windows.erase(it_window);
	}

	return s_pThis->m_Windows.size() > 0;
}
