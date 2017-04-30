#include "Texture.h"

Texture::Texture(IV3DImageView* pImageView) :
	m_pImage(nullptr),
	m_pImageView(nullptr)
{
	SAFE_ADD_REF(pImageView);
	m_pImageView = pImageView;

	m_pImageView->GetImage(&m_pImage);
}

Texture::Texture(IV3DImage* pImage, IV3DImageView* pImageView) :
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
