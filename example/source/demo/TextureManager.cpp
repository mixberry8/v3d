#include "TextureManager.h"
#include "GraphicsManager.h"

TextureManager::TextureManager() :
	m_pGraphicsManager(nullptr)
{
}

TextureManager::~TextureManager()
{
}

V3D_RESULT TextureManager::Initialize(GraphicsManager* pGraphicsManager)
{
	m_pGraphicsManager = pGraphicsManager;

	return V3D_OK;
}

void TextureManager::Finalize()
{
	if (m_TextureMap.empty() == false)
	{
		TextureManager::TextureMap::iterator it_begin = m_TextureMap.begin();
		TextureManager::TextureMap::iterator it_end = m_TextureMap.end();
		TextureManager::TextureMap::iterator it;

		for (it = it_begin; it != it_end; ++it)
		{
			it->second->Dispose();
		}
	}

	m_pGraphicsManager = nullptr;
}

TexturePtr TextureManager::Load(const wchar_t* pFilePath)
{
	TextureManager::TextureMap::iterator it = m_TextureMap.find(pFilePath);
	if (it != m_TextureMap.end())
	{
		return it->second;
	}

	std::wstring filePath;
	CreateFilePath(pFilePath, filePath);

	IV3DImageView* pImageView;

	V3D_RESULT result = CreateImageFromFile(
		m_pGraphicsManager->GetDevicePtr(),
		m_pGraphicsManager->GetQueuePtr(),
		m_pGraphicsManager->GetCommandBufferPtr(),
		m_pGraphicsManager->GetFencePtr(),
		filePath.c_str(),
		true,
		V3D_PIPELINE_STAGE_FRAGMENT_SHADER,
		&pImageView);

	if (result != V3D_OK)
	{
		return nullptr;
	}

	TexturePtr texture = std::make_shared<Texture>(pFilePath, pImageView);
	m_TextureMap[pFilePath] = texture;

	SAFE_RELEASE(pImageView);

	return std::move(texture);
}
