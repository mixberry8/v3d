#include "ParallelManager.h"
#include <process.h>
#include <chrono>

ParallelManager::ParallelManager() :
	m_Init(false),
	m_ThreadRotation(0)
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

	m_Threads.resize(sysInfo.dwNumberOfProcessors);
	m_FunctionJobPool.resize(sysInfo.dwNumberOfProcessors);

	for (uint32_t i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		m_ThreadDatum.push_back(std::make_unique<ParallelManager::THREAD_DATA>(&m_ThreadShareData, i));

		m_Threads[i] = std::thread(ParallelManager::WorkThread, m_ThreadDatum[i].get());

		HANDLE threadHandle = reinterpret_cast<HANDLE>(m_Threads[i].native_handle());
		uint32_t threadMask = 1 << i;
		SetThreadAffinityMask(threadHandle, threadMask);
	}

	m_Init = true;

	return true;
}

void ParallelManager::Finalize()
{
	if (m_Init == false)
	{
		return;
	}

	uint32_t threadCount = GetThreadCount();

	// ----------------------------------------------------------------------------------------------------
	// スレッドに終了を通知
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < threadCount; i++)
	{
		// リクストを追加
		m_ThreadDatum[i]->requests.push_back(ParallelManager::THREAD_REQUEST(nullptr, i, i));

		// スレッドを起こす
		std::lock_guard<std::mutex> lock(m_ThreadDatum[i]->wakeupMutex);
		m_ThreadDatum[i]->wakeup = true;
		m_ThreadDatum[i]->wakeupCondition.notify_one();
	}

	// ----------------------------------------------------------------------------------------------------
	// スレッドの終了を待機
	// ----------------------------------------------------------------------------------------------------

	for (uint32_t i = 0; i < threadCount; i++)
	{
		m_Threads[i].join();
	}

	// ----------------------------------------------------------------------------------------------------

	m_Init = false;
}

void ParallelManager::Reset()
{
	std::vector<std::unique_ptr<THREAD_DATA>>::iterator it_begin = m_ThreadDatum.begin();
	std::vector<std::unique_ptr<THREAD_DATA>>::iterator it_end = m_ThreadDatum.end();
	std::vector<std::unique_ptr<THREAD_DATA>>::iterator it;

	for (it = it_begin; it != it_end; ++it)
	{
		(*it)->info.busy = 0;
		(*it)->info.jobCount = 0;
	}
}

void ParallelManager::Execute(PParallelFunction function, uint32_t count, void* pData)
{
	if (count == 0)
	{
		return;
	}

	// ----------------------------------------------------------------------------------------------------
	// ジョブを追加
	// ----------------------------------------------------------------------------------------------------

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

		uint32_t thread = (m_ThreadRotation + i) % threadCount;
		m_ThreadDatum[thread]->requests.push_back(std::move(ParallelManager::THREAD_REQUEST(pSrc, thread, i)));
	}

	// ----------------------------------------------------------------------------------------------------
	// スレッドを起こす
	// ----------------------------------------------------------------------------------------------------

	m_ThreadShareData.wakeupCount = wakeupCount;

	for (uint32_t i = 0; i < wakeupCount; i++)
	{
		uint32_t thread = (m_ThreadRotation + i) % threadCount;

		std::lock_guard<std::mutex> lock(m_ThreadDatum[thread]->wakeupMutex);
		m_ThreadDatum[thread]->wakeup = true;
		m_ThreadDatum[thread]->wakeupCondition.notify_one();
	}

	// ----------------------------------------------------------------------------------------------------
	// スレッドの終了を待機
	// ----------------------------------------------------------------------------------------------------

	std::unique_lock<std::mutex> lock(m_ThreadShareData.completeMutex);
	m_ThreadShareData.completeCondition.wait(lock, [this]() { return (m_ThreadShareData.wakeupCount == 0); });

	// ----------------------------------------------------------------------------------------------------
	// スレッドのローテーション
	// ----------------------------------------------------------------------------------------------------

	m_ThreadRotation = (m_ThreadRotation + 1) % threadCount;
}

void ParallelManager::ExecuteBatch(PParallelFunction function, uint32_t totalCount, uint32_t batchCount, void* pData)
{
	// ----------------------------------------------------------------------------------------------------
	// ジョブを追加
	// ----------------------------------------------------------------------------------------------------

	uint32_t threadCount = GetThreadCount();
	uint32_t jobBatchCount = totalCount / batchCount;
	uint32_t jobRemainder = totalCount % batchCount;
	uint32_t wakeupCount = (jobBatchCount > threadCount) ? threadCount : jobBatchCount;

	m_FunctionJobPool.resize(jobBatchCount);

	ParallelManager::ParallelFunctionJob* pJobs = m_FunctionJobPool.data();

	uint32_t batchFirst = 0;

	for (uint32_t i = 0; i < jobBatchCount; i++)
	{
		ParallelManager::ParallelFunctionJob* pSrc = &pJobs[i];
		pSrc->first = batchFirst;
		pSrc->count = batchCount + ((jobRemainder > 0)? 1 : 0);
		pSrc->function = function;
		pSrc->pData = pData;

		if (jobRemainder > 0)
		{
			jobRemainder--;
		}

		batchFirst += pSrc->count;

		uint32_t thread = (m_ThreadRotation + i) % threadCount;
		m_ThreadDatum[thread]->requests.push_back(ParallelManager::THREAD_REQUEST(pSrc, thread, i));
	}

	// ----------------------------------------------------------------------------------------------------
	// スレッドを起こす
	// ----------------------------------------------------------------------------------------------------

	m_ThreadShareData.wakeupCount = wakeupCount;

	for (uint32_t i = 0; i < wakeupCount; i++)
	{
		uint32_t thread = (m_ThreadRotation + i) % threadCount;

		std::lock_guard<std::mutex> lock(m_ThreadDatum[thread]->wakeupMutex);
		m_ThreadDatum[thread]->wakeup = true;
		m_ThreadDatum[thread]->wakeupCondition.notify_one();
	}

	// ----------------------------------------------------------------------------------------------------
	// スレッドの終了を待機
	// ----------------------------------------------------------------------------------------------------

	std::unique_lock<std::mutex> lock(m_ThreadShareData.completeMutex);
	m_ThreadShareData.completeCondition.wait(lock, [this]() { return (m_ThreadShareData.wakeupCount == 0); });

	// ----------------------------------------------------------------------------------------------------
	// スレッドのローテーション
	// ----------------------------------------------------------------------------------------------------

	m_ThreadRotation = (m_ThreadRotation + 1) % threadCount;
}

uint32_t ParallelManager::GetThreadCount() const
{
	return static_cast<uint32_t>(m_Threads.size());
}

const ParallelThreadInfo& ParallelManager::GetThreadInfo(uint32_t thread) const
{
	return m_ThreadDatum[thread]->info;
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
		// ----------------------------------------------------------------------------------------------------
		// リクエストが来るのを待機
		// ----------------------------------------------------------------------------------------------------

		{
			std::unique_lock<std::mutex> lock(pThreadData->wakeupMutex);

			pThreadData->wakeupCondition.wait(lock, [pThreadData]() { return pThreadData->wakeup; });
			pThreadData->wakeup = false;
		}

		// ----------------------------------------------------------------------------------------------------
		// リクエストを処理する
		// ----------------------------------------------------------------------------------------------------

		// 計測開始
		auto startCount = std::chrono::high_resolution_clock::now();

		ParallelManager::THREAD_REQUEST request(nullptr, 0, 0);

		while (pThreadData->requests.empty() == false)
		{
			request = pThreadData->requests.back();
			pThreadData->requests.pop_back();

			if (request.pJob != nullptr)
			{
				request.pJob->OnExecute(pThreadData->id, request.page);
				pThreadData->info.jobCount++;
			}
			else
			{
				continueLoop = false;
			}
		}

		// 計測終了
		auto endCount = std::chrono::high_resolution_clock::now();
		pThreadData->info.busy += std::chrono::duration_cast<std::chrono::duration<long long, std::chrono::microseconds::period>>(endCount - startCount).count();

		// ----------------------------------------------------------------------------------------------------
		// リクエストの処理が完了したことを通知
		// ----------------------------------------------------------------------------------------------------

		{
			std::lock_guard<std::mutex> lock(pThreadShareData->completeMutex);

			pThreadShareData->wakeupCount--;
			pThreadShareData->completeCondition.notify_one();
		}

	} while (continueLoop == true);

#ifdef _DEBUG
	stringStream.str(L"");
	stringStream << L"ParallelManager : End thread[" << pThreadData->id << L"]\n";
	::OutputDebugStringW(stringStream.str().c_str());
#endif //_DEBUG

	return 0;
}
