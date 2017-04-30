#pragma once

class ResourceHeap
{
public:
	struct Node;
	typedef Node* Handle;

	ResourceHeap();
	~ResourceHeap();

	bool IsInitialized() const;

	V3D_RESULT Initialize(IV3DDevice* pDevice, V3DFlags propertyFlags, uint64_t size);
	void Dispose();

	V3D_RESULT Bind(IV3DResource* pResource, ResourceHeap::Handle* pHandle);
	void Unbind(ResourceHeap::Handle handle);

	IV3DResourceMemory* GetMemory();

private:
	struct NodeSort
	{
		bool operator() (const ResourceHeap::Node* lh, const ResourceHeap::Node* rh) const;
	};

	IV3DDevice* m_pDevice;
	IV3DResourceMemory* m_pMemory;
	V3DFlags m_PropertyFlags;

	ResourceHeap::Node* m_pNodeTop;
	ResourceHeap::Node* m_pNodeBottom;
	std::vector<ResourceHeap::Node*> m_UnusedNodes;
	std::vector<ResourceHeap::Node*> m_NodePool;

	ResourceHeap::Node* Allocate(uint64_t size, uint64_t alignment);
	void Free(ResourceHeap::Node* pNode);

	ResourceHeap::Node* AllocateNode();
	ResourceHeap::Node* FreeNode(ResourceHeap::Node* pNode, bool down);
};
