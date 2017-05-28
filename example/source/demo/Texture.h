#pragma once

class Texture
{
public:
	Texture(const wchar_t* pFilePath, IV3DImageView* pImageView);
	Texture(const wchar_t* pFilePath, IV3DImage* pImage, IV3DImageView* pImageView);
	~Texture();

	const wchar_t* GetFilePath() const;

	IV3DImage* GetImagePtr();
	IV3DImageView* GetImageViewPtr();

private:
	std::wstring m_FilePath;
	IV3DImage* m_pImage;
	IV3DImageView* m_pImageView;

	void Dispose();

	friend class TextureManager;
};

typedef std::shared_ptr<Texture> TexturePtr;
