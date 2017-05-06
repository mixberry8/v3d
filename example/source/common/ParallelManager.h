#pragma once

#include <atomic>

typedef void(__stdcall *PParallelFunction)(uint32_t thread, uint32_t first, uint32_t count,	void* pData);

class ParallelManager
{
public:
	ParallelManager();
	~ParallelManager();

	bool Initialize();
	void Finalize();

	void Execute(PParallelFunction func, uint32_t count, void* pData);

	uint32_t GetThreadCount() const;

private:
	struct THREAD_EVENT_ARGS
	{
		uint32_t thread;
		uint32_t first;
		uint32_t count;
	};

	struct THREAD_DATA
	{
		std::vector<HANDLE> threadHandles;
		HANDLE wakeupSemaphore;
		std::atomic_uint32_t remainingCount;
		HANDLE compleateEventHandle;

		CRITICAL_SECTION eventSync;
		std::vector<ParallelManager::THREAD_EVENT_ARGS> threadEventArgs;

		PParallelFunction function;
		void* pData;
	};

	bool m_Init;
	THREAD_DATA m_ThreadData;

	static unsigned __stdcall WorkThread(void* pData);
};
