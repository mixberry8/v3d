#pragma once

class Texture
{
public:
	Texture(IV3DImageView* pImageView);
	Texture(IV3DImage* pImage, IV3DImageView* pImageView);
	~Texture();

	IV3DImage* GetImagePtr();
	IV3DImageView* GetImageViewPtr();

private:
	IV3DImage* m_pImage;
	IV3DImageView* m_pImageView;

	void Dispose();

	friend class TextureManager;
};

typedef std::shared_ptr<Texture> TexturePtr;
