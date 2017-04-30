#pragma once

#include "Texture.h"

class GraphicsManager;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	V3D_RESULT Initialize(GraphicsManager* pGraphicsManager);
	void Finalize();

	TexturePtr Load(const wchar_t* pFilePath);

private:
	GraphicsManager* m_pGraphicsManager;

	typedef std::map<std::wstring, TexturePtr> TextureMap;
	std::map<std::wstring, TexturePtr> m_TextureMap;
};
