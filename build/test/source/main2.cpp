#include "stdafx.h"
#include "Application.h"
#include "Window.h"
#include "GraphicsManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "Font.h"
#include "NamingService.h"
#include "ParallelManager.h"

// ----------------------------------------------------------------------------------------------------

static auto constexpr SCREEN_WIDTH = 1024;
static auto constexpr SCREEN_HEIGHT = 768;
static auto constexpr BUFFER_COUNT = 2;

static auto constexpr OBJECT_COUNT = 50 * 50 * 50;

// ----------------------------------------------------------------------------------------------------

struct Test
{
	Matrix4x4 worldMat;
	Matrix4x4 worldViewProjMat;
};

// ----------------------------------------------------------------------------------------------------

class TestWindow : public Window
{
public:
	TestWindow() :
		m_pMemory(nullptr)
	{
	}

	virtual ~TestWindow()
	{
	}

protected:
	virtual bool OnInitialize() override
	{
		const V3DDeviceCaps& deviceCaps = Application::GetDevice()->GetCaps();
		uint64_t memoryAlignment = MAXIMUM(deviceCaps.minMemoryMapAlignment, deviceCaps.minUniformBufferOffsetAlignment);
		uint64_t memorySize = sizeof(Test);

		if (memorySize % memoryAlignment)
		{
			if (memorySize < memoryAlignment)
			{
				memorySize = memoryAlignment * OBJECT_COUNT;
			}
			else
			{
				memorySize = (((memorySize / memoryAlignment) + 1) * memoryAlignment) * OBJECT_COUNT;
			}
		}

		if (Application::GetDevice()->AllocateResourceMemory(V3D_MEMORY_PROPERTY_DEVICE_LOCAL, memorySize, &m_pMemory) != V3D_OK)
		{
			return false;
		}

		m_Objects.reserve(OBJECT_COUNT);

		V3DBufferSubresourceDesc desc;
		desc.usageFlags = V3D_BUFFER_USAGE_UNIFORM;
		desc.size = sizeof(Test);

		uint64_t memoryOffset = 0;

		for (uint32_t i = 0; i < 32769; i++)
		{
			Object obj;

			if (Application::GetDevice()->CreateBuffer(1, &desc, &obj.pBuffer) != V3D_OK)
			{
				return false;
			}

			if (Application::GetDevice()->BindResourceMemory(m_pMemory, obj.pBuffer, memoryOffset) != V3D_OK)
			{
				return false;
			}

			const V3DResourceDesc& rd = obj.pBuffer->GetResourceDesc();
			memoryOffset += rd.memorySize;

			m_Objects.push_back(obj);
		}

		return true;
	}

	virtual void OnFinalize() override
	{
		OnLostSwapChain();

		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			SAFE_RELEASE(m_Objects[i].pBuffer);
		}
		m_Objects.clear();

		SAFE_RELEASE(m_pMemory);
	}

	virtual bool OnIdle() override
	{
		return true;
	}

	virtual void OnLostSwapChain() override
	{
	}

	virtual bool OnRestoreSwapChain() override
	{
		return true;
	}

private:
	struct Object
	{
		IV3DBuffer* pBuffer;
	};

	std::vector<Object> m_Objects;

	IV3DResourceMemory* m_pMemory;
};

class TestApp : public Application
{
public:
	TestApp() {}
	virtual ~TestApp() {}

	virtual bool OnInitialize() override
	{
		::srand((unsigned)time(nullptr));

		uint32_t queueFamily = Application::FindQueueFamily(V3D_QUEUE_GRAPHICS, 2);
		if (queueFamily == ~0U)
		{
			return false;
		}

		IV3DQueue* pWorkQueue;
		IV3DQueue* pGraphicsQueue;

		Application::GetDevice()->GetQueue(queueFamily, 0, &pWorkQueue);
		Application::GetDevice()->GetQueue(queueFamily, 1, &pGraphicsQueue);

		if (m_Window.Initialize(L"test", SCREEN_WIDTH, SCREEN_HEIGHT, pWorkQueue, pGraphicsQueue) == false)
		{
			SAFE_RELEASE(pGraphicsQueue);
			SAFE_RELEASE(pWorkQueue);
			return false;
		}

		SAFE_RELEASE(pGraphicsQueue);
		SAFE_RELEASE(pWorkQueue);
		return true;
	}

	virtual void OnIdle() override {}

	virtual void OnFinalize() override {}

private:
	TestWindow m_Window;
};

// ----------------------------------------------------------------------------------------------------

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	TestApp app;
	return app.Execute();
}
