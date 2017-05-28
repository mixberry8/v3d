#include "ParallelManager.h"
#include <process.h>

//#define PARALLEL_FAKE

ParallelManager::ParallelManager() :
	m_Init(false)
{
}

ParallelManager::~ParallelManager()
{
}

bool ParallelManager::Initialize()
{
	SYSTEM_INFO sysInfo;
	::GetSystemInfo(&sysInfo);

	//	sysInfo.dwNumberOfProcessors = 1;

#ifdef PARALLEL_FAKE

	m_ThresdHandles.resize(sysInfo.dwNumberOfProcessors);

#else //PARALLEL_FAKE

	m_ThreadShareData.wakeupSemaphore = ::CreateSemaphore(nullptr, 0, LONG_MAX, nullptr);

	::InitializeCriticalSection(&m_ThreadShareData.requestSync);
	m_ThreadShareData.requests.reserve(sysInfo.dwNumberOfProcessors);

	m_ThreadDatum.resize(sysInfo.dwNumberOfProcessors);
	m_ThresdHandles.resize(sysInfo.dwNumberOfProcessors);

	for (uint32_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		DWORD mask = 1 << i;

		m_ThreadShareData.compleateEventHandles.push_back(::CreateEvent(nullptr, FALSE, FALSE, nullptr));

		m_ThreadDatum[i].pShare = &m_ThreadShareData;
		m_ThreadDatum[i].id = i;

		uintptr_t handle = ::_beginthreadex(nullptr, 0, ParallelManager::WorkThread, &m_ThreadDatum[i], CREATE_SUSPENDED, nullptr);
		if (handle == -1)
		{
			return false;
		}

		m_ThresdHandles[i] = reinterpret_cast<HANDLE>(handle);

		::SetThreadAffinityMask(m_ThresdHandles[i], mask);

		ResumeThread(m_ThresdHandles[i]);
	}

#endif //PARALLEL_FAKE

	m_FunctionJobPool.resize(sysInfo.dwNumberOfProcessors);

	m_Init = true;

	return true;
}

void ParallelManager::Finalize()
{
	if (m_Init == false)
	{
		return;
	}

#ifndef PARALLEL_FAKE

	// スレッドに終了を通知
	uint32_t threadCount = GetThreadCount();

	for (uint32_t i = 0; i < threadCount; i++)
	{
		m_ThreadShareData.requests.push_back(ParallelManager::THREAD_REQUEST(nullptr, i, i));
	}

	::ReleaseSemaphore(m_ThreadShareData.wakeupSemaphore, threadCount, nullptr);
	::WaitForMultipleObjects(static_cast<DWORD>(m_ThresdHandles.size()), m_ThresdHandles.data(), TRUE, INFINITE);

	//解放
	if (m_ThresdHandles.empty() == false)
	{
		for (size_t i = 0; i < m_ThresdHandles.size(); i++)
		{
			::CloseHandle(m_ThresdHandles[i]);
		}

		m_ThresdHandles.clear();
	}

	::DeleteCriticalSection(&m_ThreadShareData.requestSync);

	if (m_ThreadShareData.compleateEventHandles.empty() == false)
	{
		for (size_t i = 0; i < m_ThreadShareData.compleateEventHandles.size(); i++)
		{
			::CloseHandle(m_ThreadShareData.compleateEventHandles[i]);
			m_ThreadShareData.compleateEventHandles[i] = nullptr;
		}
		m_ThreadShareData.compleateEventHandles.clear();
	}

	::CloseHandle(m_ThreadShareData.wakeupSemaphore);

#endif // PARALLEL_FAKE

	m_Init = false;
}

void ParallelManager::Execute(PParallelFunction function, uint32_t count, void* pData)
{
	if (count == 0)
	{
		return;
	}

	uint32_t threadCount = GetThreadCount();
	uint32_t wakeupCount = (threadCount < count) ? threadCount : count;

	m_FunctionJobPool.resize(wakeupCount);

	ParallelManager::ParallelFunctionJob* pJobs = m_FunctionJobPool.data();
	uint32_t batchCount = count / threadCount;
	uint32_t batchRemainder = count % threadCount;
	uint32_t batchFirst = 0;

	for (uint32_t i = 0; i < wakeupCount; i++)
	{
		ParallelManager::ParallelFunctionJob* pSrc = &pJobs[i];
		pSrc->first = batchFirst;
		pSrc->count = batchCount;
		pSrc->function = function;
		pSrc->pData = pData;

		if (batchRemainder > 0)
		{
			pSrc->count += 1;
			batchRemainder--;
		}

		batchFirst += pSrc->count;

#ifdef PARALLEL_FAKE
		pSrc->function(i, pSrc->first, pSrc->count, pSrc->pData);
#else //PARALLEL_FAKE
		m_ThreadShareData.requests.push_back(ParallelManager::THREAD_REQUEST(pSrc, i, i));
#endif //PARALLEL_FAKE
	}

#ifndef PARALLEL_FAKE
	::ReleaseSemaphore(m_ThreadShareData.wakeupSemaphore, wakeupCount, nullptr);
	::WaitForMultipleObjects(wakeupCount, m_ThreadShareData.compleateEventHandles.data(), TRUE, INFINITE);
#endif //PARALLEL_FAKE
}

void ParallelManager::ExecuteBatch(PParallelFunction function, uint32_t totalCount, uint32_t batchCount, void* pData)
{
	uint32_t threadCount = GetThreadCount();
	uint32_t wakeupCount = (totalCount % batchCount)? (totalCount / batchCount + 1) : (totalCount / batchCount);

	uint32_t compleateEventHandleCount = TO_UI32(m_ThreadShareData.compleateEventHandles.size());
	if (compleateEventHandleCount < wakeupCount)
	{
		uint32_t addCount = wakeupCount - compleateEventHandleCount;
		for (uint32_t i = 0; i < addCount; i++)
		{
			m_ThreadShareData.compleateEventHandles.push_back(::CreateEvent(nullptr, FALSE, FALSE, nullptr));
		}
	}

	m_FunctionJobPool.resize(wakeupCount);

	ParallelManager::ParallelFunctionJob* pJobs = m_FunctionJobPool.data();
	uint32_t jobCount = totalCount;

	uint32_t batchFirst = 0;

	for (uint32_t i = 0; i < wakeupCount; i++)
	{
		ParallelManager::ParallelFunctionJob* pSrc = &pJobs[i];
		pSrc->first = batchFirst;
		pSrc->count = (batchCount < jobCount)? batchCount : jobCount;
		pSrc->function = function;
		pSrc->pData = pData;

		if (jobCount >= batchCount)
		{
			jobCount -= batchCount;
		}

		batchFirst += pSrc->count;

#ifdef PARALLEL_FAKE
		pSrc->function(i % GetThreadCount(), pSrc->first, pSrc->count, pSrc->pData);
#else //PARALLEL_FAKE
		m_ThreadShareData.requests.push_back(ParallelManager::THREAD_REQUEST(pSrc, i % threadCount, i));
#endif //PARALLEL_FAKE
	}

#ifndef PARALLEL_FAKE
	::ReleaseSemaphore(m_ThreadShareData.wakeupSemaphore, wakeupCount, nullptr);
	::WaitForMultipleObjects(wakeupCount, m_ThreadShareData.compleateEventHandles.data(), TRUE, INFINITE);
#endif //PARALLEL_FAKE
}

uint32_t ParallelManager::GetThreadCount() const
{
	return static_cast<uint32_t>(m_ThresdHandles.size());
}

unsigned __stdcall ParallelManager::WorkThread(void* pData)
{
	ParallelManager::THREAD_DATA* pThreadData = static_cast<ParallelManager::THREAD_DATA*>(pData);
	ParallelManager::THREAD_SHARE_DATA* pThreadShareData = pThreadData->pShare;
	bool continueLoop = true;

#ifdef _DEBUG
	std::wstringstream stringStream;

	stringStream << L"ParallelManager : Start thread[" << pThreadData->id << L"]\n";
	::OutputDebugStringW(stringStream.str().c_str());
#endif //_DEBUG

	do
	{
		::WaitForSingleObject(pThreadShareData->wakeupSemaphore, INFINITE);

		ParallelManager::THREAD_REQUEST request(nullptr, 0, 0);

		::EnterCriticalSection(&pThreadShareData->requestSync);

		request = pThreadShareData->requests.back();
		pThreadShareData->requests.pop_back();

		::LeaveCriticalSection(&pThreadShareData->requestSync);

		if (request.pJob != nullptr)
		{
			request.pJob->OnExecute(pThreadData->id, request.page);
			SetEvent(pThreadShareData->compleateEventHandles[request.index]);
		}
		else
		{
			continueLoop = false;
		}

	} while (continueLoop == true);

#ifdef _DEBUG
	stringStream.str(L"");
	stringStream << L"ParallelManager : End thread[" << pThreadData->id << L"]\n";
	::OutputDebugStringW(stringStream.str().c_str());
#endif //_DEBUG

	return 0;
}
