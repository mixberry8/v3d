#include "DynamicOffsetHeap.h"

DynamicOffsetHeap::DynamicOffsetHeap() :
	m_Count(0),
	m_Stride(0),
	m_InuseOffsetCount(0)
{
}

DynamicOffsetHeap::~DynamicOffsetHeap()
{
}

void DynamicOffsetHeap::Initialize(uint32_t count, uint32_t stride)
{
	ASSERT(count > 0);
	ASSERT(stride > 0);

	m_Count = count;
	m_Stride = stride;
}

bool DynamicOffsetHeap::Retain(DynamicOffsetHeap::Handle* pHandle)
{
	if (m_InuseOffsetCount == m_Count)
	{
		return false;
	}

	pHandle->m_pParent = this;

	if (m_UnuseOffsets.empty() == false)
	{
		pHandle->m_Offset = m_UnuseOffsets.back();
		m_UnuseOffsets.pop_back();
	}
	else
	{
		pHandle->m_Offset = m_InuseOffsetCount * m_Stride;
	}

	m_InuseOffsetCount++;

	return true;
}

void DynamicOffsetHeap::Release(DynamicOffsetHeap::Handle* pHandle)
{
	if (pHandle->m_Offset != DynamicOffsetHeap::INVALID_OFFSET)
	{
		ASSERT(pHandle->m_pParent == this);
		ASSERT(m_InuseOffsetCount > 0);

		m_UnuseOffsets.push_back(pHandle->m_Offset);
		pHandle->m_Offset = DynamicOffsetHeap::INVALID_OFFSET;

		m_InuseOffsetCount--;
	}
}
