#include "ParallelManager.h"
#include <process.h>

bool ParallelManager::Initialize()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	//	sysInfo.dwNumberOfProcessors = 1;

	m_ThreadData.wakeupSemaphore = ::CreateSemaphore(nullptr, 0, sysInfo.dwNumberOfProcessors, nullptr);
	::InitializeCriticalSection(&m_ThreadData.threadEventCS);
	m_ThreadData.threadEventArgs.reserve(sysInfo.dwNumberOfProcessors);

	m_ThreadData.compleateEventHandles.resize(sysInfo.dwNumberOfProcessors);
	m_ThreadData.threadHandles.resize(sysInfo.dwNumberOfProcessors);

	for (uint32_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		DWORD mask = 1 << i;

		m_ThreadData.compleateEventHandles[i] = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

		uintptr_t handle = ::_beginthreadex(nullptr, 0, ParallelManager::WorkThread, &m_ThreadData, 0, nullptr);
		if (handle == -1)
		{
			return false;
		}

		m_ThreadData.threadHandles[i] = reinterpret_cast<HANDLE>(handle);

		::SetThreadAffinityMask(m_ThreadData.threadHandles[i], mask);
	}

	return true;
}

void ParallelManager::Finalize()
{
	// スレッドに終了を通知
	::EnterCriticalSection(&m_ThreadData.threadEventCS);
	m_ThreadData.threadEventArgs.clear();
	::LeaveCriticalSection(&m_ThreadData.threadEventCS);

	::ReleaseSemaphore(m_ThreadData.wakeupSemaphore, static_cast<LONG>(m_ThreadData.threadHandles.size()), nullptr);
	::WaitForMultipleObjects(static_cast<DWORD>(m_ThreadData.threadHandles.size()), m_ThreadData.threadHandles.data(), TRUE, INFINITE);

	//解放
	for (size_t i = 0; i < m_ThreadData.threadHandles.size(); i++)
	{
		::CloseHandle(m_ThreadData.threadHandles[i]);
	}
	m_ThreadData.threadHandles.clear();

	for (size_t i = 0; i < m_ThreadData.compleateEventHandles.size(); i++)
	{
		::CloseHandle(m_ThreadData.compleateEventHandles[i]);
	}
	m_ThreadData.compleateEventHandles.clear();

	::DeleteCriticalSection(&m_ThreadData.threadEventCS);

	::CloseHandle(m_ThreadData.wakeupSemaphore);
}

void ParallelManager::Execute(PParallelFunction function, uint32_t count, void* pData)
{
	uint32_t numThread = static_cast<uint32_t>(m_ThreadData.threadHandles.size());
	uint32_t wakeupCount = (numThread < count) ? numThread : count;

	::EnterCriticalSection(&m_ThreadData.threadEventCS);

	m_ThreadData.threadEventArgs.clear();
	m_ThreadData.threadEventArgs.resize(wakeupCount);

	ParallelManager::THREAD_EVENT_ARGS* eventArgs = m_ThreadData.threadEventArgs.data();
	uint32_t batchCount = count / numThread;
	uint32_t batchRemainder = count % numThread;
	uint32_t batchFirst = 0;

	for (uint32_t i = 0; i < wakeupCount; i++)
	{
		eventArgs[i].thread = i;
		eventArgs[i].first = batchFirst;
		eventArgs[i].count = batchCount;

		if (batchRemainder > 0)
		{
			eventArgs[i].count += 1;
			batchRemainder--;
		}

		batchFirst += eventArgs[i].count;
	}

	::LeaveCriticalSection(&m_ThreadData.threadEventCS);

	m_ThreadData.function = function;
	m_ThreadData.pData = pData;

	::ReleaseSemaphore(m_ThreadData.wakeupSemaphore, wakeupCount, nullptr);
	::WaitForMultipleObjects(wakeupCount, m_ThreadData.compleateEventHandles.data(), TRUE, INFINITE);
}

uint32_t ParallelManager::GetThreadCount() const
{
	return static_cast<uint32_t>(m_ThreadData.threadHandles.size());
}

unsigned __stdcall ParallelManager::WorkThread(void* pData)
{
#ifdef _DEBUG
	::OutputDebugString(L"ParallelManager : Start thread\n");
#endif //_DEBUG

	ParallelManager::THREAD_DATA* pThreadData = static_cast<ParallelManager::THREAD_DATA*>(pData);
	ParallelManager::THREAD_EVENT_ARGS args;
	bool continueLoop = true;

	do
	{
		::WaitForSingleObject(pThreadData->wakeupSemaphore, INFINITE);

		::EnterCriticalSection(&pThreadData->threadEventCS);
		if (pThreadData->threadEventArgs.size() > 0)
		{
			args = pThreadData->threadEventArgs.back();
			pThreadData->threadEventArgs.pop_back();
		}
		else
		{
			continueLoop = false;
		}
		::LeaveCriticalSection(&pThreadData->threadEventCS);

		if (continueLoop == true)
		{
			pThreadData->function(args.thread, args.first, args.count, pThreadData->pData);
			SetEvent(pThreadData->compleateEventHandles[args.thread]);
		}

	} while (continueLoop == true);

#ifdef _DEBUG
	::OutputDebugString(L"ParallelManager : End thread\n");
#endif //_DEBUG

	return 0;
}
