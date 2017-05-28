#pragma once

#include <atomic>

typedef void(__stdcall *PParallelFunction)(uint32_t thread, uint32_t page, uint32_t first, uint32_t count,	void* pData);

class ParallelJob
{
public:
	virtual ~ParallelJob() {}
	virtual void OnExecute(uint32_t thread, uint32_t batch) = 0;
};

class ParallelManager
{
public:
	ParallelManager();
	~ParallelManager();

	bool Initialize();
	void Finalize();

	void Execute(PParallelFunction function, uint32_t count, void* pData);
	void ExecuteBatch(PParallelFunction function, uint32_t totalCount, uint32_t batchCount, void* pData);

	uint32_t GetThreadCount() const;

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

	struct THREAD_SHARE_DATA
	{
		HANDLE wakeupSemaphore;
		std::vector<HANDLE> compleateEventHandles;

		CRITICAL_SECTION requestSync;
		std::vector<THREAD_REQUEST> requests;
	};

	struct THREAD_DATA
	{
		THREAD_SHARE_DATA* pShare;
		uint32_t id;
	};

	bool m_Init;
	THREAD_SHARE_DATA m_ThreadShareData;
	std::vector<THREAD_DATA> m_ThreadDatum;
	std::vector<HANDLE> m_ThresdHandles;

	std::vector<ParallelManager::ParallelFunctionJob> m_FunctionJobPool;

	static unsigned __stdcall WorkThread(void* pData);
};
