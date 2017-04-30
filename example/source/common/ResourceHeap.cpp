#include "ResourceHeap.h"

struct ResourceHeap::Node
{
	bool exist;
	bool used;

	uint64_t offset;
	uint64_t size;

	Node* pPrev;
	Node* pNext;
};

bool ResourceHeap::NodeSort::operator() (const ResourceHeap::Node* lh, const ResourceHeap::Node* rh) const
{
	if (lh->exist < rh->exist)
	{
		return false;
	}

	return (lh->size > rh->size);
}

ResourceHeap::ResourceHeap() :
	m_pDevice(nullptr),
	m_pMemory(nullptr)
{
}

ResourceHeap::~ResourceHeap()
{
	Dispose();
}

bool ResourceHeap::IsInitialized() const
{
	return (m_pMemory != nullptr);
}

V3D_RESULT ResourceHeap::Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint64_t size)
{
	SAFE_ADD_REF(pDevice);
	m_pDevice = pDevice;

	m_PropertyFlags = propertyFlags;

	V3D_RESULT result = m_pDevice->AllocateResourceMemory(propertyFlags, size, &m_pMemory);
	if (result != V3D_OK)
	{
		return result;
	}

	// ----------------------------------------------------------------------------------------------------
	// 最初のノードを作成
	// ----------------------------------------------------------------------------------------------------

	ResourceHeap::Node* pNode = new(std::nothrow) ResourceHeap::Node;
	if (pNode == nullptr)
	{
		return V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	pNode->exist = true;
	pNode->used = false;
	pNode->offset = 0;
	pNode->size = size;
	pNode->pPrev = nullptr;
	pNode->pNext = nullptr;

	m_pNodeTop = pNode;
	m_pNodeBottom = pNode;

	m_UnusedNodes.push_back(pNode);

	return V3D_OK;
}

void ResourceHeap::Dispose()
{
	if (m_NodePool.empty() == false)
	{
		std::vector<ResourceHeap::Node*>::iterator it_begin = m_NodePool.begin();
		std::vector<ResourceHeap::Node*>::iterator it_end = m_NodePool.end();
		std::vector<ResourceHeap::Node*>::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			ResourceHeap::Node* pNode = *it;
			SAFE_DELETE(pNode);
		}
	}

	if (m_pNodeTop != nullptr)
	{
		ResourceHeap::Node* pNode = m_pNodeTop;
		while (pNode != nullptr)
		{
			ResourceHeap::Node* pNextNode = pNode->pNext;
			SAFE_DELETE(pNode);
			pNode = pNextNode;
		}

		m_pNodeTop = nullptr;
	}

	SAFE_RELEASE(m_pMemory);
	SAFE_RELEASE(m_pDevice);
}

V3D_RESULT ResourceHeap::Bind(IV3DResource* pResource, ResourceHeap::Handle* pHandle)
{
	const V3DResourceDesc& resourceDesc = pResource->GetResourceDesc();

	ResourceHeap::Node* pNode = Allocate(resourceDesc.memorySize, resourceDesc.memoryAlignment);
	if (pNode == nullptr)
	{
		return (m_PropertyFlags & V3D_MEMORY_PROPERTY_DEVICE_LOCAL)? V3D_ERROR_OUT_OF_DEVICE_MEMORY : V3D_ERROR_OUT_OF_HOST_MEMORY;
	}

	V3D_RESULT result = m_pDevice->BindResourceMemory(m_pMemory, pResource, pNode->offset);
	if(result != V3D_OK)
	{
		Free(pNode);
		return result;
	}

	return V3D_OK;
}

void ResourceHeap::Unbind(ResourceHeap::Handle handle)
{
	Free(handle);
}

IV3DResourceMemory* ResourceHeap::GetMemory()
{
	return m_pMemory;
}

ResourceHeap::Node* ResourceHeap::Allocate(uint64_t size, uint64_t alignment)
{
	if (m_UnusedNodes.empty() == true)
	{
		return nullptr;
	}

	// ----------------------------------------------------------------------------------------------------
	// 空いているノードを探す
	// ----------------------------------------------------------------------------------------------------

	ResourceHeap::Node* pNode = nullptr;
	uint64_t memoryOffset = 0;
	size_t unusedNodeIndex = 0;

	ResourceHeap::Node** ppNode = m_UnusedNodes.data();
	ResourceHeap::Node** ppNodeEnd = ppNode + m_UnusedNodes.size();

	while ((ppNode != ppNodeEnd) && (pNode == nullptr))
	{
		ResourceHeap::Node* pTempNode = *ppNode;
		uint64_t tempSize = 0;

		if ((pTempNode->offset % alignment) == 0)
		{
			memoryOffset = pTempNode->offset;
			tempSize = pTempNode->size;
		}
		else
		{
			memoryOffset = ((pTempNode->offset / alignment) + 1) * alignment;
			tempSize = pTempNode->size - (memoryOffset - pTempNode->offset);
		}

		if (tempSize >= size)
		{
			pNode = pTempNode;
		}

		unusedNodeIndex++;
		ppNode++;
	}

	if (pNode == nullptr)
	{
		return nullptr;
	}

	// 1 余計にカウントされるはずなので -1 しておく
	ASSERT(unusedNodeIndex >= 1);
	unusedNodeIndex -= 1;

	// ----------------------------------------------------------------------------------------------------
	// ノードの使用状況を更新
	// ----------------------------------------------------------------------------------------------------

	ASSERT(pNode->offset <= memoryOffset);

	uint64_t upSize = memoryOffset - pNode->offset;
	if (upSize > 0)
	{
		// 上方向に分割
		ResourceHeap::Node* pNewNode = AllocateNode();
		ASSERT(pNode != nullptr);

		pNewNode->exist = true;
		pNewNode->used = false;
		pNewNode->offset = pNode->offset;
		pNewNode->size = upSize;

		if (pNode == m_pNodeTop)
		{
			ASSERT(pNode->pPrev == nullptr);

			pNode->pPrev = pNewNode;

			pNewNode->pPrev = nullptr;
			pNewNode->pNext = pNode;

			m_pNodeTop = pNewNode;
		}
		else
		{
			ASSERT(pNode->pPrev != nullptr);

			pNewNode->pPrev = pNode->pPrev;
			pNewNode->pNext = pNode;

			pNode->pPrev->pNext = pNewNode;
			pNode->pPrev = pNewNode;
		}

		m_UnusedNodes.push_back(pNewNode);

		// 上に挿入されたらインデックスが +1 する
		unusedNodeIndex += 1;
	}

	ASSERT(size <= (pNode->size - upSize));

	uint64_t downSize = 0;
	if ((pNode->size - upSize) > size)
	{
		// 下方向に分割
		ResourceHeap::Node* pNewNode = AllocateNode();
		ASSERT(pNode != nullptr);

		pNewNode->exist = true;
		pNewNode->used = false;
		pNewNode->offset = pNode->offset + upSize + size;
		pNewNode->size = pNode->size - (pNewNode->offset - pNode->offset);

		downSize = pNewNode->size;

		if (pNode == m_pNodeBottom)
		{
			ASSERT(pNode->pNext == nullptr);

			pNode->pNext = pNewNode;

			pNewNode->pPrev = pNode;
			pNewNode->pNext = nullptr;

			m_pNodeBottom = pNewNode;
		}
		else
		{
			ASSERT(pNode->pNext != nullptr);

			pNewNode->pPrev = pNode;
			pNewNode->pNext = pNode->pNext;

			pNode->pNext->pPrev = pNewNode;
			pNode->pNext = pNewNode;
		}

		m_UnusedNodes.push_back(pNewNode);

		// 下に挿入された場合はインデックスは変化が無い
		// unusedNodeIndex
	}

	// ----------------------------------------------------------------------------------------------------
	// 未使用リストから削除
	// ----------------------------------------------------------------------------------------------------

	ASSERT(m_UnusedNodes[unusedNodeIndex] == pNode);

	m_UnusedNodes[unusedNodeIndex] = m_UnusedNodes.back();
	m_UnusedNodes[m_UnusedNodes.size() - 1] = pNode;
	m_UnusedNodes.pop_back();

	pNode->used = true;
	pNode->offset = pNode->offset + upSize;
	pNode->size = pNode->size - upSize - ((pNode->size - upSize) - size);

	// ----------------------------------------------------------------------------------------------------

	return pNode;
}

void ResourceHeap::Free(ResourceHeap::Node* pNode)
{
	// ----------------------------------------------------------------------------------------------------
	// 空いているノードを結合
	// ----------------------------------------------------------------------------------------------------

	ResourceHeap::Node* pCompNode;
	uint64_t newOffset = pNode->offset;
	uint64_t newSize = pNode->size;
	size_t invalidNodeCount = 0;

	// 上方向
	pCompNode = pNode->pPrev;
	while (pCompNode != nullptr)
	{
		if (pCompNode->used == false)
		{
			newOffset = pCompNode->offset;
			newSize += pCompNode->size;

			invalidNodeCount++;

			pCompNode = FreeNode(pCompNode, false);
		}
		else
		{
			pCompNode = nullptr;
		}
	}

	// 下方向
	pCompNode = pNode->pNext;
	while (pCompNode != nullptr)
	{
		if (pCompNode->used == false)
		{
			newSize += pCompNode->size;

			invalidNodeCount++;

			pCompNode = FreeNode(pCompNode, true);
		}
		else
		{
			pCompNode = nullptr;
		}
	}

	uint64_t oldSize = pNode->size;

	pNode->used = false;
	pNode->offset = newOffset;
	pNode->size = newSize;

	// ----------------------------------------------------------------------------------------------------
	// 未使用リストに追加してソート
	// ----------------------------------------------------------------------------------------------------

	m_UnusedNodes.push_back(pNode);

	std::sort(m_UnusedNodes.begin(), m_UnusedNodes.end(), ResourceHeap::NodeSort());

	// ----------------------------------------------------------------------------------------------------
	// 結合され無効になったノードを削除
	// ----------------------------------------------------------------------------------------------------

	if (invalidNodeCount != 0)
	{
		m_UnusedNodes.resize(m_UnusedNodes.size() - invalidNodeCount);
	}

	// ----------------------------------------------------------------------------------------------------
}

ResourceHeap::Node* ResourceHeap::AllocateNode()
{
	if (m_NodePool.empty() == false)
	{
		ResourceHeap::Node* pNode = m_NodePool.back();
		m_NodePool.pop_back();
		return pNode;
	}

	return new(std::nothrow) ResourceHeap::Node;
}

ResourceHeap::Node* ResourceHeap::FreeNode(ResourceHeap::Node* pNode, bool down)
{
#ifdef _DEBUG
	if ((m_pNodeTop == pNode) && (m_pNodeBottom == pNode))
	{
		ASSERT(0);
	}
#endif //_DEBUG

	if ((m_pNodeTop == pNode) && (m_pNodeBottom != pNode))
	{
		m_pNodeTop = pNode->pNext;
		m_pNodeTop->pPrev = nullptr;
	}
	else if ((m_pNodeTop != pNode) && (m_pNodeBottom == pNode))
	{
		m_pNodeBottom = pNode->pPrev;
		m_pNodeBottom->pNext = nullptr;
	}
	else
	{
		pNode->pPrev->pNext = pNode->pNext;
		pNode->pNext->pPrev = pNode->pPrev;
	}

	ResourceHeap::Node* pNextNode = (down == true) ? pNode->pNext : pNode->pPrev;

	pNode->exist = false;
	pNode->used = false;
	pNode->offset = 0;
	pNode->size = 0;
	pNode->pPrev = nullptr;
	pNode->pNext = nullptr;

	m_NodePool.push_back(pNode);

	return pNextNode;
}
