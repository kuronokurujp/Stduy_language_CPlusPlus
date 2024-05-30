#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>

/// <summary>
/// テクスチャ制御クラス
/// OpenGL専用
/// </summary>
class Texture
{
public:
	Texture() { this->_Clear(); }
	~Texture() { this->Unload(); }

	bool Load(const std::string& in_rFileName);
	void Unload();

	void SetActive();

	int GetWidth() { return this->width; }
	int GetHeight() { return this->height; }

	std::string& GetName() { return this->fileName; }

	unsigned int GetTextureID() { return this->textureID; }

private:
	unsigned int textureID;
	int width, height;
	std::string fileName;

	auto _Clear() -> void
	{
		this->width = this->height = 0;
		this->textureID = 0;
		this->fileName = "";
	}
};

#endif // __TEXTURE_H__