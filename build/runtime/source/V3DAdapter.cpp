#include "V3DAdapter.h"

V3DAdapter* V3DAdapter::Create()
{
	return V3D_NEW_T(V3DAdapter);
}

V3D_RESULT V3DAdapter::Initialize(const VkPhysicalDevice physicalDevice)
{
	V3D_ASSERT(physicalDevice != VK_NULL_HANDLE);

	m_Source.physicalDevice = physicalDevice;

	// ----------------------------------------------------------------------------------------------------
	// アダプタの記述
	// ----------------------------------------------------------------------------------------------------

	VkPhysicalDeviceProperties vkPhysicalDeviceProps;
	vkGetPhysicalDeviceProperties(m_Source.physicalDevice, &vkPhysicalDeviceProps);

	m_Desc.apiVersion = vkPhysicalDeviceProps.apiVersion;
	m_Desc.driverVersion = vkPhysicalDeviceProps.driverVersion;
	m_Desc.vendorID = vkPhysicalDeviceProps.vendorID;
	m_Desc.deviceID = vkPhysicalDeviceProps.deviceID;
	strcpy_s(m_Desc.deviceName, _countof(m_Desc.deviceName), vkPhysicalDeviceProps.deviceName);

	// ----------------------------------------------------------------------------------------------------
	// メモリヒープを列挙
	// ----------------------------------------------------------------------------------------------------

	vkGetPhysicalDeviceMemoryProperties(m_Source.physicalDevice, &m_Source.physicalDeviceMemoryProps);

	m_MemoryHeaps.resize(m_Source.physicalDeviceMemoryProps.memoryHeapCount);

	for (uint32_t i = 0; i < m_Source.physicalDeviceMemoryProps.memoryHeapCount; i++)
	{
		const VkMemoryHeap& vkMemHeap = m_Source.physicalDeviceMemoryProps.memoryHeaps[i];

		m_MemoryHeaps[i].type = (vkMemHeap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)? V3D_MEMORY_HEAP_TYPE_DEVICE : V3D_MEMORY_HEAP_TYPE_HOST;
		m_MemoryHeaps[i].size = vkMemHeap.size;
	}

	// ----------------------------------------------------------------------------------------------------
	// メモリタイプを列挙
	// ----------------------------------------------------------------------------------------------------

	m_MemoryTypes.resize(m_Source.physicalDeviceMemoryProps.memoryTypeCount);

	for (uint32_t i = 0; i < m_Source.physicalDeviceMemoryProps.memoryTypeCount; i++)
	{
		const VkMemoryType& vkMemType = m_Source.physicalDeviceMemoryProps.memoryTypes[i];

		m_MemoryTypes[i].propertyFlags = ToV3DMemoryPropertyFlags(vkMemType.propertyFlags);
		m_MemoryTypes[i].heap = vkMemType.heapIndex;
	}

	// ----------------------------------------------------------------------------------------------------

	return V3D_OK;
}

uint32_t V3DAdapter::Vulkan_GetMemoryTypeIndex(VkMemoryPropertyFlags memoryPropertyFlags)
{
	const VkPhysicalDeviceMemoryProperties& vkMemoryProps = m_Source.physicalDeviceMemoryProps;
	VkMemoryPropertyFlags vkMemoryPropFlags = ToVkMemoryPropertyFlags(memoryPropertyFlags);

	for (uint32_t i = 0; i < vkMemoryProps.memoryTypeCount; ++i)
	{
		if ((vkMemoryProps.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
		{
			return i;
		}
	}

	return 0xFFFFFFFF;
}

V3DAdapter::Source& V3DAdapter::GetSource()
{
	return m_Source;
}

/*********************************/
/* public override - IV3DAdapter */
/*********************************/

const V3DAdapterDesc& V3DAdapter::GetDesc() const
{
	return m_Desc;
}

uint32_t V3DAdapter::GetMemoryHeapCount() const
{
	return static_cast<uint32_t>(m_MemoryHeaps.size());
}

const V3DMemoryHeapDesc& V3DAdapter::GetMemoryHeapDesc(uint32_t memoryHeap) const
{
	return m_MemoryHeaps[memoryHeap];
}

uint32_t V3DAdapter::GetMemoryTypeCount() const
{
	return static_cast<uint32_t>(m_MemoryTypes.size());
}

const V3DMemoryTypeDesc& V3DAdapter::GetMemoryTypeDesc(uint32_t memoryType) const
{
	return m_MemoryTypes[memoryType];
}

/********************************/
/* public override - IV3DObject */
/********************************/

int64_t V3DAdapter::GetRefCount() const
{
	return m_RefCounter;
}

void V3DAdapter::AddRef()
{
	++m_RefCounter;
}

void V3DAdapter::Release()
{
	if (--m_RefCounter == 0)
	{
		V3D_DELETE_THIS_T(this, V3DAdapter);
	}
}

/*************************/
/* private - IV3DAdapter */
/*************************/

V3DAdapter::V3DAdapter() :
	m_RefCounter(1),
	m_Desc({})
{
}

V3DAdapter::~V3DAdapter()
{
}
