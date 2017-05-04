#include "ParallelManager.h"
#include <process.h>

bool ParallelManager::Initialize()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	//	sysInfo.dwNumberOfProcessors = 1;

	m_ThreadData.wakeupSemaphore = ::CreateSemaphore(nullptr, 0, sysInfo.dwNumberOfProcessors, nullptr);
	m_ThreadData.compleateEventHandle = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

	::InitializeCriticalSection(&m_ThreadData.eventSync);
	m_ThreadData.threadEventArgs.reserve(sysInfo.dwNumberOfProcessors);

	m_ThreadData.threadHandles.resize(sysInfo.dwNumberOfProcessors);

	for (uint32_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		DWORD mask = 1 << i;

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
	m_ThreadData.threadEventArgs.clear();

	::ReleaseSemaphore(m_ThreadData.wakeupSemaphore, static_cast<LONG>(m_ThreadData.threadHandles.size()), nullptr);
	::WaitForMultipleObjects(static_cast<DWORD>(m_ThreadData.threadHandles.size()), m_ThreadData.threadHandles.data(), TRUE, INFINITE);

	//解放
	if (m_ThreadData.threadHandles.empty() == false)
	{
		for (size_t i = 0; i < m_ThreadData.threadHandles.size(); i++)
		{
			::CloseHandle(m_ThreadData.threadHandles[i]);
		}
		m_ThreadData.threadHandles.clear();
	}

	::DeleteCriticalSection(&m_ThreadData.eventSync);
	::CloseHandle(m_ThreadData.compleateEventHandle);
	::CloseHandle(m_ThreadData.wakeupSemaphore);
}

void ParallelManager::Execute(PParallelFunction function, uint32_t count, void* pData)
{
	uint32_t numThread = static_cast<uint32_t>(m_ThreadData.threadHandles.size());
	uint32_t wakeupCount = (numThread < count) ? numThread : count;

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

	m_ThreadData.function = function;
	m_ThreadData.pData = pData;

	m_ThreadData.remainingCount = wakeupCount;
	::ReleaseSemaphore(m_ThreadData.wakeupSemaphore, wakeupCount, nullptr);
	::WaitForSingleObject(m_ThreadData.compleateEventHandle, INFINITE);
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

		::EnterCriticalSection(&pThreadData->eventSync);
		if (pThreadData->threadEventArgs.size() > 0)
		{
			args = pThreadData->threadEventArgs.back();
			pThreadData->threadEventArgs.pop_back();
		}
		else
		{
			continueLoop = false;
		}
		::LeaveCriticalSection(&pThreadData->eventSync);

		if (continueLoop == true)
		{
			pThreadData->function(args.thread, args.first, args.count, pThreadData->pData);

			if (--pThreadData->remainingCount == 0)
			{
				SetEvent(pThreadData->compleateEventHandle);
			}
		}

	} while (continueLoop == true);

#ifdef _DEBUG
	::OutputDebugString(L"ParallelManager : End thread\n");
#endif //_DEBUG

	return 0;
}
