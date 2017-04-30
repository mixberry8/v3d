#pragma once

typedef void(__stdcall *PParallelFunction)(uint32_t thread, uint32_t first, uint32_t count,	void* pData);

class ParallelManager
{
public:
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
		std::vector<HANDLE> compleateEventHandles;

		CRITICAL_SECTION threadEventCS;
		std::vector<ParallelManager::THREAD_EVENT_ARGS> threadEventArgs;

		PParallelFunction function;
		void* pData;
	};

	THREAD_DATA m_ThreadData;

	static unsigned __stdcall WorkThread(void* pData);
};
