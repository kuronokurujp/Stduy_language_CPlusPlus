#include "texture.h"
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <SDL/SDL.h>

/// <summary>
/// Loads the specified in r file name.
/// </summary>
/// <param name="in_rFileName">Name of the in r file.</param>
/// <returns></returns>
auto Texture::Load(const std::string& in_rFileName) -> bool
{
	int channels = 0;

	// 画像ロード
	unsigned char* pImage = SOIL_load_image(
		in_rFileName.c_str(),
		&this->width,
		&this->height,
		&channels,
		SOIL_LOAD_AUTO
	);

	// ロード失敗した場合のメッセージ
	if (pImage == nullptr)
	{
		// SOIL処理の結果を表示
		SDL_Log("SOIL failed to load image file %s: %s", in_rFileName.c_str(), SOIL_last_result());
		return false;
	}

	// 画像がRGB/RGBAか判定してフォーマットを変える
	int format = GL_RGB;
	if (channels == 4)
	{
		// RGBAの画像
		format = GL_RGBA;
	}

	{
		GLint  textureUnits = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
		// テクスチャユニット数
		SDL_Log("GPU TextureUnitNum %d", textureUnits);
	}

	// OpneGLのテクスチャを作成してBindでアクティブにする
	glGenTextures(1, &this->textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	// memo: openglのメソッドを使ったから直後にassert入れると早めに問題が分かる！
	SDL_assert(glGetError() == GL_NO_ERROR);

	// OpenGLで作成したテクスチャにファイルロードした画像を当てはめる
	glTexImage2D(
		// ターゲット
		GL_TEXTURE_2D,
		// LoD
		0,
		// カラーフォーマット
		format,
		this->width,
		this->height,
		0,
		// 入力データのカラーフォーマット
		format,
		// 入力データのビット深度
		GL_UNSIGNED_BYTE,
		// 画像データのポインタ
		pImage
	);
	SDL_assert(glGetError() == GL_NO_ERROR);

	// テクスチャデータを作成したのでファイルロードした画像はいらないので破棄
	SOIL_free_image_data(pImage);

	// テクスチャのフィルタリングをバイリニアにする
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_assert(glGetError() == GL_NO_ERROR);

	SDL_Log("enable textureID: flag:%d / id:%d", glIsTexture(this->textureID), this->textureID);
	// 下記の有効設定は固定機能パイプラインでのみ有効
	// https://community.khronos.org/t/opengl-3-x-glenable-gl-texture-2d-gl-invalid-enum/61405
	/*
	glEnable(GL_TEXTURE);
	SDL_Log("enable GL_TEXUTER: %d", glIsEnabled(GL_TEXTURE));
	*/

	this->fileName = in_rFileName;
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

/// <summary>
/// Unloads this instance.
/// </summary>
/// <returns></returns>
auto Texture::Unload() -> void
{
	// 二重開放防止
	if (this->textureID == 0)
	{
		return;
	}

	glDeleteTextures(1, &this->textureID);
	this->textureID = 0;
}

/// <summary>
/// Sets the active.
/// </summary>
/// <returns></returns>
auto Texture::SetActive() -> void
{
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}