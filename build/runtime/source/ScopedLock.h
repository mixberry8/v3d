#pragma once

#include "CriticalSection.h"

class ScopedLock final
{
private:
	const CriticalSection& m_CriticalSection;

public:
	ScopedLock(const CriticalSection& criticalSection) :
		m_CriticalSection(criticalSection)
	{
		m_CriticalSection.Enter();
	}

	~ScopedLock()
	{
		m_CriticalSection.Leave();
	}

	ScopedLock(const ScopedLock&) = delete;
	ScopedLock& operator = (const ScopedLock&) = delete;
	ScopedLock(const ScopedLock&&) = delete;
	ScopedLock& operator = (const ScopedLock&&) = delete;
};
