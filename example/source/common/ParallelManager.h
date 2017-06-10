#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>

typedef void(__stdcall *PParallelFunction)(uint32_t thread, uint32_t page, uint32_t first, uint32_t count,void* pData);

class ParallelJob
{
public:
	virtual ~ParallelJob() {}
	virtual void OnExecute(uint32_t thread, uint32_t batch) = 0;
};

struct ParallelThreadInfo
{
	long long busy;
	uint32_t jobCount;
};

class ParallelManager
{
public:
	ParallelManager();
	~ParallelManager();

	bool Initialize();
	void Finalize();

	void Reset();

	void Execute(PParallelFunction function, uint32_t count, void* pData);
	void ExecuteBatch(PParallelFunction function, uint32_t totalCount, uint32_t batchCount, void* pData);

	uint32_t GetThreadCount() const;
	const ParallelThreadInfo& GetThreadInfo(uint32_t thread) const;

private:
	class ParallelFunctionJob : public ParallelJob
	{
	public:
		uint32_t first;
		uint32_t count;
		PParallelFunction function;
		void* pData;

		virtual void OnExecute(uint32_t thread, uint32_t page) override
		{
			function(thread, page, first, count, pData);
		}
	};

	struct THREAD_SHARE_DATA
	{
		std::mutex completeMutex;
		std::condition_variable completeCondition;
		uint32_t wakeupCount;
	};

	struct THREAD_REQUEST
	{
		ParallelJob* pJob;
		uint32_t page;
		uint32_t index;

		THREAD_REQUEST()
		{
		}

		THREAD_REQUEST(ParallelJob* pJob, uint32_t page, uint32_t index)
		{
			this->pJob = pJob;
			this->page = page;
			this->index = index;
		}
	};

	struct THREAD_DATA
	{
		THREAD_SHARE_DATA* pShare;

		uint32_t id;

		std::mutex wakeupMutex;
		std::condition_variable wakeupCondition;
		bool wakeup;

		std::vector<THREAD_REQUEST> requests;

		ParallelThreadInfo info;

		THREAD_DATA(THREAD_SHARE_DATA* pShare, uint32_t id)
		{
			this->pShare = pShare;
			this->id = id;

			wakeup = false;

			info.busy = 0;
			info.jobCount = 0;
		}
	};

	bool m_Init;

	THREAD_SHARE_DATA m_ThreadShareData;
	std::vector<std::unique_ptr<THREAD_DATA>> m_ThreadDatum;
	std::vector<std::thread> m_Threads;

	std::vector<ParallelManager::ParallelFunctionJob> m_FunctionJobPool;
	uint32_t m_ThreadRotation;

	static unsigned __stdcall WorkThread(void* pData);
};
