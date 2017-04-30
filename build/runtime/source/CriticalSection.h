#pragma once

class CriticalSection final
{
private:
	mutable CRITICAL_SECTION criticalSection_;

public:
	CriticalSection()
	{
		::InitializeCriticalSection(&criticalSection_);
	}

	~CriticalSection()
	{
		::DeleteCriticalSection(&criticalSection_);
	}

	void Enter() const
	{
		::EnterCriticalSection(&criticalSection_);
	}

	void Leave() const
	{
		::LeaveCriticalSection(&criticalSection_);
	}
};
