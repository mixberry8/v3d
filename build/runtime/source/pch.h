#pragma once

#include "v3d.h"
#include "stringResources.h"
#include <vulkan\vulkan.h>
#include <vector>
#include <map>
#include <atomic>
#include <algorithm>

#ifdef V3D_DEBUG
#include <cassert>
#include <sstream>
#endif //V3D_DEBUG

// ----------------------------------------------------------------------------------------------------
// 定数
// ----------------------------------------------------------------------------------------------------

#define V3D_GRAPHICS_SHADER_STAGE_MAX 5 // グラフィックスシェーダーステージの数
#define V3D_COMPUTE_SHADER_STAGE_MAX 1 // コンピュートシェーダーステージの数
#define V3D_INSTANCE_LAYER_COUNT 2 // インスタンスのレイヤー数

#define V3D_PUSH_DESCRIPTOR_MAX 64 // プッシュできるデスクリプタの最大数

static constexpr char* V3D_LAYER_LUNARG_standard_validation = "VK_LAYER_LUNARG_standard_validation";
static constexpr char* V3D_LAYER_NV_nsight = "VK_LAYER_NV_nsight";
static constexpr char* V3D_LAYER_RENDERDOC_Capture = "VK_LAYER_RENDERDOC_Capture";

struct V3DFindLayer
{
public:
	V3DFindLayer(const char* pLayerName) : m_pLayerName(pLayerName) {}

	bool operator()(const VkLayerProperties& props) const
	{
		return (strcmp(props.layerName, m_pLayerName) == 0);
	}

private:
	const char* m_pLayerName;
};

struct V3DFindExtension
{
public:
	V3DFindExtension(const char* pExtensionName) : m_pExtensionName(pExtensionName) {}

	bool operator()(const VkExtensionProperties& props) const
	{
		return (strcmp(props.extensionName, m_pExtensionName) == 0);
	}

private:
	const char* m_pExtensionName;
};

// ----------------------------------------------------------------------------------------------------
// マクロ
// ----------------------------------------------------------------------------------------------------

/************/
/* デバッグ */
/************/

#ifdef V3D_DEBUG

#define V3D_ASSERT(expression) assert(expression)

#define V3D_ADD_DEBUG_OBJECT(instance, object, name) instance->AddDebugObject(object, name)
#define V3D_REMOVE_DEBUG_OBJECT(instance, object) instance->RemoveDebugObject(object)

#define V3D_DEBUG_CODE(code) code

#else //V3D_DEBUG

#define V3D_ASSERT(expression)

#define V3D_ADD_DEBUG_OBJECT(instance, object, name)
#define V3D_REMOVE_DEBUG_OBJECT(instance, object)

#define V3D_DEBUG_CODE(code)

#endif //V3D_DEBUG

struct V3D_PTR_TO_STR_STRUCT_W
{
public:
	V3D_PTR_TO_STR_STRUCT_W(void* ptr)
	{
#ifdef V3D64
		uint64_t addr = reinterpret_cast<uint64_t>(ptr);
		::wsprintf(m_Addr, L"0x%.16I64x", addr);
#else //V3D64
		uint32_t addr32 = reinterpret_cast<uint32_t>(ptr);
		::wsprintf(m_Addr, L"0x%.8x", addr32);
#endif //V3D64
	}

	operator const wchar_t* () const
	{
		return m_Addr;
	}

private:
	wchar_t m_Addr[32];
};

struct V3D_PTR_TO_STR_STRUCT_A
{
public:
	V3D_PTR_TO_STR_STRUCT_A(void* ptr)
	{
#ifdef V3D64
		uint64_t addr = reinterpret_cast<uint64_t>(ptr);
		::sprintf_s(m_Addr, "0x%.16I64x", addr);
#else //V3D64
		uint32_t addr32 = reinterpret_cast<uint32_t>(ptr);
		::sprintf_s(m_Addr, "0x%.8x", addr32);
#endif //V3D64
	}

	operator const char* () const
	{
		return m_Addr;
	}

private:
	char m_Addr[32];
};

#define V3D_SAFE_NAME_W(owner, name) ((name != nullptr)? name : V3D_PTR_TO_STR_STRUCT_W(owner))
#define V3D_SAFE_NAME_A(owner, name) ((name != nullptr)? name : V3D_PTR_TO_STR_STRUCT_A(owner))
#define V3D_SAFE_NAME(owner, name) V3D_SAFE_NAME_W(owner, name)

#define V3D_SET_DEBUG_MARKER_OBJECT_NAME(device, objectType, object, name) device->Vulkan_SetDebugMarkerObjectName(objectType, object, name)

/****************/
/* 参照カウンタ */
/****************/

typedef std::atomic<int64_t> ReferenceCounter;

#define V3D_REF_INC(counter) std::atomic_fetch_add_explicit(&counter, 1, std::memory_order_relaxed)
#define V3D_REF_DEC(counter) (std::atomic_fetch_sub_explicit(&counter, 1, std::memory_order_release) == 1)
#define V3D_REF_FENCE() std::_Atomic_thread_fence(std::memory_order_acquire)

template<class T>
static constexpr T* ToAddRef(T* obj)
{
	if (obj != nullptr)
	{
		obj->AddRef();
	}

	return obj;
}

#define V3D_ADD_REF(x) if(x != nullptr) { x->AddRef(); }
#define V3D_TO_ADD_REF(x) ToAddRef(x)
#define V3D_RELEASE(x) if(x != nullptr) { x->Release(); x = nullptr; }

/**********/
/* その他 */
/**********/

#define V3D_MIN(a, b) ((a) > (b))? (b) : (a)
#define V3D_MAX(a, b) ((a) < (b))? (b) : (a)

#define V3D_FLOAT_EPSILON 1.192092896e-07F
#define V3D_FLOAT_IS_ZERO( a ) ( ( -V3D_FLOAT_EPSILON < ( a ) ) && ( V3D_FLOAT_EPSILON > ( a ) ) )
#define V3D_FLOAT_DIV( a, b ) ( ( V3D_FLOAT_IS_ZERO( b ) == false )? ( ( a ) / ( b ) ) : 0.0f )

#define V3D_CHECK_RANGE_EQUAL(value, minValue, maxValue) ((minValue) <= (value) && (value) <= (maxValue))

// ----------------------------------------------------------------------------------------------------
// メモリ
// ----------------------------------------------------------------------------------------------------

#ifdef V3D_DEBUG

enum V3D_DEBUG_OBJECT_TYPE
{
	V3D_DEBUG_OBJECT_TYPE_ADAPTER = 0,
	V3D_DEBUG_OBJECT_TYPE_BACK_BUFFER = 1,
	V3D_DEBUG_OBJECT_TYPE_BARRIER_SET = 2,
	V3D_DEBUG_OBJECT_TYPE_BUFFER = 3,
	V3D_DEBUG_OBJECT_TYPE_BUFFER_VIEW = 4,
	V3D_DEBUG_OBJECT_TYPE_COMMAND_BUFFER = 5,
	V3D_DEBUG_OBJECT_TYPE_COMMAND_POOL = 6,
	V3D_DEBUG_OBJECT_TYPE_COMPUTE_PIPELINE = 7,
	V3D_DEBUG_OBJECT_TYPE_DEVICE = 8,
	V3D_DEBUG_OBJECT_TYPE_EVENT = 9,
	V3D_DEBUG_OBJECT_TYPE_FENCE = 10,
	V3D_DEBUG_OBJECT_TYPE_FRAME_BUFFER = 11,
	V3D_DEBUG_OBJECT_TYPE_GRAPHICS_PIPELINE = 12,
	V3D_DEBUG_OBJECT_TYPE_IMAGE = 13,
	V3D_DEBUG_OBJECT_TYPE_IMAGE_VIEW = 14,
	V3D_DEBUG_OBJECT_TYPE_INSTANCE = 15,
	V3D_DEBUG_OBJECT_TYPE_PIPELINE_LAYOUT = 16,
	V3D_DEBUG_OBJECT_TYPE_PUSH_DESCRIPTOR_SET = 17,
	V3D_DEBUG_OBJECT_TYPE_PUSH_DESCRIPTOR_SET_LAYOUT = 18,
	V3D_DEBUG_OBJECT_TYPE_QUERY_POOL = 19,
	V3D_DEBUG_OBJECT_TYPE_QUEUE = 20,
	V3D_DEBUG_OBJECT_TYPE_RENDER_PASS = 21,
	V3D_DEBUG_OBJECT_TYPE_RESOURCE_MEMORY = 22,
	V3D_DEBUG_OBJECT_TYPE_SAMPLER = 23,
	V3D_DEBUG_OBJECT_TYPE_SEMAPHORE = 24,
	V3D_DEBUG_OBJECT_TYPE_SHADER_MODULE = 25,
	V3D_DEBUG_OBJECT_TYPE_STANDARD_DESCRIPTOR_SET = 26,
	V3D_DEBUG_OBJECT_TYPE_STANDARD_DESCRIPTOR_SET_LAYOUT = 27,
	V3D_DEBUG_OBJECT_TYPE_SWAPCHAIN = 28,

	V3D_DEBUG_OBJECT_TYPE_COUNT = 29,
};

class DebugMemoryObjectManager
{
public:
	static DebugMemoryObjectManager* GetInstance();
	static void CreateInstance();
	static void ReleaseInstance();

	void Add(IV3DObject* pObject, V3D_DEBUG_OBJECT_TYPE type, const wchar_t* pName);
	void Remove(IV3DObject* pObject);

private:
	struct Info
	{
		V3D_DEBUG_OBJECT_TYPE type;
		std::wstring name;
	};

	static DebugMemoryObjectManager* s_pInstance;

	std::map<IV3DObject*, Info> m_InfoMap;

	DebugMemoryObjectManager();
	~DebugMemoryObjectManager();
};

DebugMemoryObjectManager* DebugMemoryObjectManager::s_pInstance = nullptr;

#define V3D_ADD_DEBUG_MEMORY_OBJECT(object, type, name) DebugMemoryObjectManager::GetInstance()->Add(object, type, name)
#define V3D_REMOVE_DEBUG_MEMORY_OBJECT(object) DebugMemoryObjectManager::GetInstance()->Remove(object)

#else //V3D_DEBUG

#define V3D_ADD_DEBUG_MEMORY_OBJECT(object, type, name)
#define V3D_REMOVE_DEBUG_MEMORY_OBJECT(object)

#endif //V3D_DEBUG

void InitializeMemory(PV3DAllocateMemoryFunction pAllocMemFunc, PV3DReallocateMemoryFunction pReallocMemFunc, PV3DFreeMemoryFunction pFreeMemFunc, void* pUserData);
void* AllocateMemory(size_t size, size_t alignment);
void* ReallocateMemory(void* pMemory, size_t size, size_t alignment);
void FreeMemory(void* pMemory);

#define V3D_DECLARE_ALLOCATOR \
	inline void* operator new(size_t size) { return AllocateMemory(size, sizeof(void*)); } \
	inline void operator delete(void* ptr) {} \
	inline void* operator new[](size_t size) { return AllocateMemory(size, sizeof(void*)); } \
	inline void operator delete[](void* ptr) {} \
	inline void* operator new(size_t size, size_t align) { return AllocateMemory(align, size); } \
	inline void operator delete(void* ptr, size_t align) {} \
	inline void* operator new[](size_t size, size_t align) { return AllocateMemory(align, size); } \
	inline void operator delete[](void* ptr, size_t align) {}

#define V3D_MEM_INIT(pAllocMemFunc, pReallocMemFunc, pFreeMemFunc, pUserData) InitializeMemory(pAllocMemFunc, pReallocMemFunc, pFreeMemFunc, pUserData)

#define V3D_MALLOC(size) AllocateMemory(size, sizeof(void*))
#define V3D_REALLOC(ptr, size) ReallocateMemory(ptr, size, sizeof(void*))
#define V3D_FREE(ptr) FreeMemory(ptr);

#define V3D_ALIGNED_MALLOC(size, alignment) AllocateMemory(size, alignment)
#define V3D_ALIGNED_REALLOC(ptr, size, alignment) ReallocateMemory(ptr, size, alignment)
#define V3D_ALIGNED_FREE(ptr) FreeMemory(ptr);

#define V3D_NEW_T(T, ...) new T(__VA_ARGS__)
#define V3D_DELETE_T(ptr, T) if(ptr != nullptr) { ptr->~T(); FreeMemory(ptr); ptr = nullptr; }
#define V3D_DELETE_THIS_T(ptr, T) ptr->~T(); FreeMemory(ptr);

#define V3D_NEW new
#define V3D_DELETE(ptr) if(ptr != nullptr) { delete ptr; ptr = nullptr; }

// ----------------------------------------------------------------------------------------------------
// STL
// ----------------------------------------------------------------------------------------------------

template<typename T>
class STLAllocator
{
public:
	typedef T value_type;
	typedef T *pointer;
	typedef const T *const_pointer;
	typedef T &reference;
	typedef const T &const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	template <class U>
	struct rebind { typedef STLAllocator<U> other; };

public:
	STLAllocator(void) {}
	STLAllocator(const STLAllocator&) {}

	template <class U>
	STLAllocator(const STLAllocator<U>&) {}

	~STLAllocator(void) {}

	pointer allocate(size_type num, void *hint = 0) { (void)hint; return (pointer)(V3D_MALLOC(sizeof(T) * num)); }
	void deallocate(pointer p, size_type num) { (void)num; V3D_FREE(p); }

	void construct(pointer p, const T& value) { new(p) T(value); }
	void destroy(pointer p) { p->~T(); }

	pointer address(reference value) const { return &value; }
	const_pointer address(const_reference value) const { return &value; }

	size_type max_size() const { return (std::numeric_limits<size_t>::max)() / sizeof(T); }
};

template <class T, class U>
bool operator == (const STLAllocator<T>&, const STLAllocator<U>&)
{
	return true;
}

template <class T, class U>
bool operator != (const STLAllocator<T>&, const STLAllocator<U>&)
{
	return false;
}

// コレクション
template<typename T>
using STLVector = std::vector<T, STLAllocator<T>>;

#ifdef V3D_DEBUG
template<typename Key, typename T>
using STLMap = std::map<Key, T, std::less<Key>, STLAllocator<T>>;
#endif //V3D_DEBUG

// 文字列
typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char>> STLStringA;
#ifdef V3D_DEBUG
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t>> STLStringW;
#endif //V3D_DEBUG

// 文字列ストリーム
#ifdef V3D_DEBUG
typedef std::basic_stringstream<char, std::char_traits<char>, STLAllocator<char>> STLStringStreamA;
typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t>> STLStringStreamW;
#endif //V3D_DEBUG

// ----------------------------------------------------------------------------------------------------
// ログ
// ----------------------------------------------------------------------------------------------------

#ifdef V3D_DEBUG

void InitializeLog(V3DFlags flags, PV3DLogFunction pFunction, void* pUserData);
void PrintLogA(V3D_LOG_FLAG type, const char* pFormat, ...);
void PrintLogW(V3D_LOG_FLAG type, const wchar_t* pFormat, ...);

#define V3D_LOG_MESSAGE_A_MAX 8192
#define V3D_LOG_MESSAGE_W_MAX 4096

#define V3D_LOG_INIT(flags, pFunction, pUserData) InitializeLog(flags, pFunction, pUserData)

#define V3D_LOG_PRINT_A(type, format, ...) PrintLogA(type, format, __VA_ARGS__)
#define V3D_LOG_PRINT_INFO_A(format, ...) PrintLogA(V3D_LOG_INFORMATION, format, __VA_ARGS__)
#define V3D_LOG_PRINT_WARNING_A(format, ...) PrintLogA(V3D_LOG_WARNING, format, __VA_ARGS__)
#define V3D_LOG_PRINT_ERROR_A(format, ...) PrintLogA(V3D_LOG_ERROR, format, __VA_ARGS__)
#define V3D_LOG_PRINT_DEBUG_A(format, ...) PrintLogA(V3D_LOG_DEBUG, format, __VA_ARGS__)

#define V3D_LOG_PRINT_W(type, format, ...) PrintLogW(type, format, __VA_ARGS__)
#define V3D_LOG_PRINT_INFO_W(format, ...) PrintLogW(V3D_LOG_INFORMATION, format, __VA_ARGS__)
#define V3D_LOG_PRINT_WARNING_W(format, ...) PrintLogW(V3D_LOG_WARNING, format, __VA_ARGS__)
#define V3D_LOG_PRINT_ERROR_W(format, ...) PrintLogW(V3D_LOG_ERROR, format, __VA_ARGS__)
#define V3D_LOG_PRINT_DEBUG_W(format, ...) PrintLogW(V3D_LOG_DEBUG, format, __VA_ARGS__)

#define V3D_LOG_PRINT_INFO(format, ...) PrintLogW(V3D_LOG_INFORMATION, format, __VA_ARGS__)
#define V3D_LOG_PRINT_WARNING(format, ...) PrintLogW(V3D_LOG_WARNING, format, __VA_ARGS__)
#define V3D_LOG_PRINT_ERROR(format, ...) PrintLogW(V3D_LOG_ERROR, format, __VA_ARGS__)
#define V3D_LOG_PRINT_DEBUG(format, ...) PrintLogW(V3D_LOG_DEBUG, format, __VA_ARGS__)

#define V3D_LOG_TYPE_A(type) #type
#define V3D_LOG_S_STR_A(str) " \"" << str << "\""
#define V3D_LOG_S_PTR_A(ptr) " | " << #ptr << ((ptr != nullptr)? "(ok)" : "(nullptr)" )
#define V3D_LOG_S_NUM_A(num) " | " << #num << "(" << num << ")"
#define V3D_LOG_S_NUM_NOT_EQUAL_A(num0, num1) " | " << L#num0 << " != " << num1
#define V3D_LOG_S_NUM_LESS_A(num0, num1) " | " << #num0 << "(" << num0 << ")" << " < " << num1
#define V3D_LOG_S_NUM_LESS_EQUAL_A(num0, num1) " | " << #num0 << "(" << num0 << ")" << " <= " << num1
#define V3D_LOG_S_NUM_GREATER_A(num0, num1) " | " << #num0 << "(" << num0 << ")" << " > " << num1
#define V3D_LOG_S_NUM_GREATER_EQUAL_A(num0, num1) " | " << #num0 << "(" << num0 << ")" << " >= " << num1
#define V3D_LOG_S_RANGE_EQUAL_A(num, minNum, maxNum) " | " << minNum << " <= " << #num << "(" << num << ")" << " <=" << maxNum
#define V3D_LOG_S_DEBUG_NAME_A(str) " : " << str

#define V3D_LOG_TYPE_W(type) L#type
#define V3D_LOG_S_STR_W(str) L" \"" << str << L"\""
#define V3D_LOG_S_PTR_W(ptr) L" | " << #ptr << ((ptr != nullptr)? L"(ok)" : L"(nullptr)" )
#define V3D_LOG_S_NUM_W(num) L" | " << #num << L"(" << num << L")"
#define V3D_LOG_S_NUM_NOT_EQUAL_W(num0, num1) L" | " << L#num0 << L" != " << num1
#define V3D_LOG_S_NUM_LESS_W(num0, num1) L" | " << #num0 << L"(" << num0 << L")" << " < " << num1
#define V3D_LOG_S_NUM_LESS_EQUAL_W(num0, num1) L" | " << #num0 << L"(" << num0 << L")" << L" <= " << num1
#define V3D_LOG_S_NUM_GREATER_W(num0, num1) L" | " << #num0 << L"(" << num0 << L")" << " > " << num1
#define V3D_LOG_S_NUM_GREATER_EQUAL_W(num0, num1) L" | " << #num0 << L"(" << num0 << L")" << L" >= " << num1
#define V3D_LOG_S_RANGE_EQUAL_W(num, minNum, maxNum) L" | " << minNum << L" <= " << #num << L"(" << num << L")" << L" <=" << maxNum
#define V3D_LOG_S_DEBUG_NAME_W(str) L" : " << str

#define V3D_LOG_TYPE(type) V3D_LOG_TYPE_W(type)
#define V3D_LOG_S_STR(str) V3D_LOG_S_STR_W(str)
#define V3D_LOG_S_PTR(ptr) V3D_LOG_S_PTR_W(ptr)
#define V3D_LOG_S_NUM(num) V3D_LOG_S_NUM_W(num)
#define V3D_LOG_S_NUM_NOT_EQUAL(num0, num1) V3D_LOG_S_NUM_NOT_EQUAL_W(num0, num1)
#define V3D_LOG_S_NUM_LESS(num0, num1) V3D_LOG_S_NUM_LESS_W(num0, num1)
#define V3D_LOG_S_NUM_LESS_EQUAL(num0, num1) V3D_LOG_S_NUM_LESS_EQUAL_W(num0, num1)
#define V3D_LOG_S_NUM_GREATER(num0, num1) V3D_LOG_S_NUM_GREATER_W(num0, num1)
#define V3D_LOG_S_NUM_GREATER_EQUAL(num0, num1) V3D_LOG_S_NUM_GREATER_EQUAL_W(num0, num1)
#define V3D_LOG_S_RANGE_EQUAL(num, minNum, maxNum) V3D_LOG_S_RANGE_EQUAL_W(num, minNum, maxNum)
#define V3D_LOG_S_DEBUG_NAME(str) V3D_LOG_S_STR_W(str)

#define V3D_LOG_S_PRINT_ERROR_A(stream) \
	{ \
		STLStringStreamA message; \
		message << stream; \
		PrintLogA(V3D_LOG_ERROR, message.str().c_str()); \
	}

#define V3D_LOG_S_PRINT_ERROR_W(stream) \
	{ \
		STLStringStreamW message; \
		message << stream; \
		PrintLogW(V3D_LOG_ERROR, message.str().c_str()); \
	}

#define V3D_LOG_S_PRINT_ERROR(stream) V3D_LOG_S_PRINT_ERROR_W(stream)

#else //V3D_DEBUG

#define V3D_LOG_INIT(flags, pFunction, pUserData)
#define V3D_LOG_FIN()

#define V3D_LOG_PRINT_A(type, format, ...)
#define V3D_LOG_PRINT_INFO_A(format, ...)
#define V3D_LOG_PRINT_WARNING_A(format, ...)
#define V3D_LOG_PRINT_ERROR_A(format, ...)

#define V3D_LOG_PRINT_W(type, format, ...)
#define V3D_LOG_PRINT_INFO_W(format, ...)
#define V3D_LOG_PRINT_WARNING_W(format, ...)
#define V3D_LOG_PRINT_ERROR_W(format, ...)

#define V3D_LOG_PRINT_INFO(format, ...)
#define V3D_LOG_PRINT_WARNING(format, ...)
#define V3D_LOG_PRINT_ERROR(format, ...)

#define V3D_LOG_TYPE_A(type)
#define V3D_LOG_S_PTR_A(ptr)
#define V3D_LOG_S_NUM_A(num)
#define V3D_LOG_S_NUM_LESS_A(num0, num1)
#define V3D_LOG_S_NUM_LESS_EQUAL_A(num0, num1)
#define V3D_LOG_S_NUM_GREATER_A(num0, num1)
#define V3D_LOG_S_NUM_GREATER_EQUAL_A(num0, num1)
#define V3D_LOG_S_RANGE_EQUAL_A(num, minNum, maxNum)

#define V3D_LOG_TYPE_W(type)
#define V3D_LOG_S_PTR_W(ptr)
#define V3D_LOG_S_NUM_W(num)
#define V3D_LOG_S_NUM_LESS_W(num0, num1)
#define V3D_LOG_S_NUM_LESS_EQUAL_W(num0, num1)
#define V3D_LOG_S_NUM_GREATER_W(num0, num1)
#define V3D_LOG_S_NUM_GREATER_EQUAL_W(num0, num1)
#define V3D_LOG_S_RANGE_EQUAL_W(num, minNum, maxNum)

#define V3D_LOG_TYPE(type)
#define V3D_LOG_S_PTR(ptr)
#define V3D_LOG_S_NUM(num)
#define V3D_LOG_S_NUM_LESS(num0, num1)
#define V3D_LOG_S_NUM_LESS_EQUAL(num0, num1)
#define V3D_LOG_S_NUM_GREATER(num0, num1)
#define V3D_LOG_S_NUM_GREATER_EQUAL(num0, num1)
#define V3D_LOG_S_RANGE_EQUAL(num, minNum, maxNum)

#define V3D_LOG_S_PRINT_ERROR_A(stream)
#define V3D_LOG_S_PRINT_ERROR_W(stream)
#define V3D_LOG_S_PRINT_ERROR(stream)

#endif //V3D_DEBUG

// ----------------------------------------------------------------------------------------------------
// グローバル関数
// ----------------------------------------------------------------------------------------------------

// 全般
void ToMultibyteString(const wchar_t* pSrc, STLStringA& dst);

// Vulkan -> v3d の変換
static constexpr bool ToV3DBool(VkBool32 value) { return (value == VK_TRUE) ? true : false; }
V3D_RESULT ToV3DResult(VkResult value);
V3D_FORMAT ToV3DFormat(VkFormat value);
V3DFlags ToV3DQueueFlags(VkQueueFlags value);
V3D_IMAGE_LAYOUT ToV3DImageLayout(VkImageLayout value);
V3DFlags ToV3DMemoryPropertyFlags(VkMemoryPropertyFlags value);

// v3d -> Vulkan の変換
static constexpr VkBool32 ToVkBool32(bool value) { return (value == true) ? VK_TRUE : VK_FALSE; }
VkFormat ToVkFormat(V3D_FORMAT value);
VkCommandBufferUsageFlags ToVkCommandBufferUsageFlags(V3DFlags value);
VkShaderStageFlags ToVkShaderStageFlags(V3DFlags value);
VkAttachmentLoadOp ToVkAttachmentLoadOp(V3D_ATTACHMENT_LOAD_OP value);
VkAttachmentStoreOp ToVkAttachmentStoreOp(V3D_ATTACHMENT_STORE_OP value);
VkImageLayout ToVkImageLayout(V3D_IMAGE_LAYOUT value);
VkPipelineStageFlagBits ToVkPipelineStageFlagBits(V3D_PIPELINE_STAGE_FLAG value);
VkPipelineStageFlags ToVkPipelineStageFlags(V3DFlags value);
VkAccessFlags ToVkAccessFlags(V3DFlags value);
VkDependencyFlags ToVkDependencyFlags(V3DFlags value);
VkPrimitiveTopology ToVkPrimitiveTopology(V3D_PRIMITIVE_TOPOLOGY value);
VkPolygonMode ToVkPolygonMode(V3D_POLYGON_MODE value);
VkCullModeFlags ToVkCullMode(V3D_CULL_MODE value);
VkCompareOp ToVkCompareOp(V3D_COMPARE_OP value);
VkBlendFactor ToVkBlendFactor(V3D_BLEND_FACTOR value);
VkBlendOp ToVkBlendOp(V3D_BLEND_OP value);
VkColorComponentFlags ToVkColorComponentFlags(V3DFlags value);
VkLogicOp ToVkLogicOp(V3D_LOGIC_OP value);
VkBufferUsageFlags ToVkBufferUsageFlags(V3DFlags value);
VkMemoryPropertyFlags ToVkMemoryPropertyFlags(V3DFlags value);
VkImageType ToVkImageType(V3D_IMAGE_TYPE value);
VkImageUsageFlags ToVkImageUsageFlags(V3DFlags value);
VkImageCreateFlags ToVkImageCreateFlags(V3DFlags value);
VkImageViewType ToVkImageViewType(V3D_IMAGE_VIEW_TYPE value);
VkIndexType ToVkIndexType(V3D_INDEX_TYPE value);
VkDescriptorType ToVkDescriptorType(V3D_DESCRIPTOR_TYPE value);
VkFilter ToVkFilter(V3D_FILTER value);
VkSamplerMipmapMode ToVkSamplerMipmapMode(V3D_MIPMAP_MODE value);
VkSamplerAddressMode ToVkSamplerAddressMode(V3D_ADDRESS_MODE value);
VkBorderColor ToVkBorderColor(V3D_BORDER_COLOR value);
VkPipelineBindPoint ToVkPipelineBindPoint(V3D_PIPELINE_TYPE value);
VkImageTiling ToVkImageTiling(V3D_IMAGE_TILING value);
VkSampleCountFlagBits ToVkSampleCountFlagBits(V3D_SAMPLE_COUNT_FLAG value);
VkSampleCountFlags ToVkSampleCountFlags(V3DFlags value);
VkQueryType ToVkQueryType(V3D_QUERY_TYPE value);
VkQueryResultFlags ToVkQueryResultFlags(V3DFlags value);
VkFormatFeatureFlags ToVkBufferFormatFeatureFlags(V3DFlags value);
VkFormatFeatureFlags ToVkImageFormatFeatureFlags(V3DFlags value);
VkStencilOp ToVkStencilOp(V3D_STENCIL_OP value);
VkStencilFaceFlags ToVkStencilFaceFlags(V3DFlags value);

// ----------------------------------------------------------------------------------------------------
// 定義に依存するインクルード
// ----------------------------------------------------------------------------------------------------

#ifdef V3D_DEBUG
#include "V3DInstance.h"
#endif //V3D_DEBUG
