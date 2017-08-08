#include "Texture.h"

Texture::Texture(const wchar_t* pFilePath, IV3DImageView* pImageView) :
	m_FilePath(pFilePath),
	m_pImage(nullptr),
	m_pImageView(nullptr)
{
	SAFE_ADD_REF(pImageView);
	m_pImageView = pImageView;

	m_pImageView->GetImage(&m_pImage);
}

Texture::Texture(const wchar_t* pFilePath, IV3DImage* pImage, IV3DImageView* pImageView) :
	m_FilePath(pFilePath),
	m_pImage(nullptr),
	m_pImageView(nullptr)
{
	SAFE_ADD_REF(pImage);
	m_pImage = pImage;

	SAFE_ADD_REF(pImageView);
	m_pImageView = pImageView;
}

Texture::~Texture()
{
	Dispose();
}

const wchar_t* Texture::GetFilePath() const
{
	return m_FilePath.c_str();
}

IV3DImage* Texture::GetImagePtr()
{
	return m_pImage;
}

IV3DImageView* Texture::GetImageViewPtr()
{
	return m_pImageView;
}

void Texture::Dispose()
{
	SAFE_RELEASE(m_pImageView);
	SAFE_RELEASE(m_pImage);
}
