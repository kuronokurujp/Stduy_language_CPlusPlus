#include "3D/shader.h"
#include <fstream>
#include <sstream>
#include <SDL/SDL.h>

// シェーダーに行列値を渡すのに必要
#include "Math/matrix4.h"
#include "texture.h"

// シェーダー制御クラス
Shader::Shader()
{
	this->_Clear();
}

Shader::~Shader()
{
	this->Unload();
}

// シェーダーをロード
auto Shader::Load(const std::string& in_rVertName, const std::string& in_rFragName) -> bool
{
	bool bVertexLoad = this->_CompileShader(in_rVertName, GL_VERTEX_SHADER, this->vertexShader);
	bool bFragmentLoad = this->_CompileShader(in_rFragName, GL_FRAGMENT_SHADER, this->fragShader);
	if (!bVertexLoad || !bFragmentLoad)
	{
		return false;
	}

	// 頂点とフラグメントのシェーダーコンパイルが出来た

	// 頂点とフラグメントのシェーダを一つにまとめる
	this->shaderProgram = glCreateProgram();
	glAttachShader(this->shaderProgram, this->vertexShader);
	glAttachShader(this->shaderProgram, this->fragShader);
	glLinkProgram(this->shaderProgram);

	if (this->_IsValidProgram() == false)
	{
		return false;
	}

	return true;
}

// シェーダーをアンロード
auto Shader::Unload() -> void
{
	if (this->shaderProgram != GL_NONE)
	{
		glDeleteProgram(this->shaderProgram);
		this->shaderProgram = GL_NONE;
	}

	if (this->vertexShader != GL_NONE)
	{
		glDeleteShader(this->vertexShader);
		this->vertexShader = GL_NONE;
	}

	if (this->fragShader != GL_NONE)
	{
		glDeleteShader(this->fragShader);
		this->fragShader = GL_NONE;
	}
}

// シェーダーを有効化
auto Shader::SetActive() -> void
{
	glUseProgram(this->shaderProgram);
}

// シェーダーに行列を渡す
auto Shader::SetMatrixUniform(const char* in_pName, const Math::Matrix4* in_pMat) -> void
{
	// todo: 失敗した場合を今は考えないで後で対処
	GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);
	this->_ValidateUniform("Location", in_pName);

	glUniformMatrix4fv(
		loc,
		1,
		GL_TRUE,
		// 行列クラスのfloat配列の先頭ポインタを渡す
		in_pMat->GetAsFloatPtr());
	this->_ValidateUniform("Matrix4fv", in_pName);
}

// シェーダーにテクスチャを渡す
void Shader::SetTextureUniform(const char* in_pName, unsigned int in_textureID)
{
	int textureLocation = glGetUniformLocation(this->shaderProgram, in_pName);
	this->_ValidateUniform("Location Texture", in_pName);

	glUniform1i(textureLocation, in_textureID);

	this->_ValidateUniform("texture", in_pName);
}

// シェーダーにベクトルを渡す
void Shader::SetVectorUniform(const char* in_pName, Math::Vector3& in_rVec3)
{
	GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);
	this->_ValidateUniform("Location", in_pName);

	glUniform3fv(
		loc,
		1,
		in_rVec3.GetAsFloatPtr());
	this->_ValidateUniform("3fv", in_pName);
}

// シェーダーにfloatを渡す
void Shader::SetFloatUniform(const char* in_pName, float in_value)
{
	GLuint loc = glGetUniformLocation(this->shaderProgram, in_pName);
	this->_ValidateUniform("Location", in_pName);

	glUniform1f(
		loc,
		in_value);
	this->_ValidateUniform("1f", in_pName);
}

// シェーダーをコンパイル
auto Shader::_CompileShader(
	const std::string& in_rFileName,
	GLenum in_shaderType,
	GLuint& in_rOutShader) -> bool
{
	// シェーダーファイルを開く
	// todo: 拡張子チェックが必要
	std::ifstream shaderFile(in_rFileName);
	if (shaderFile.is_open() == false)
	{
		// シェーダーファイルを開けなかった
		SDL_Log("Failed open not shader file => %s", in_rFileName.c_str());
		return false;
	}

	// シェーダーファイルを開いた

	// ファイルの文字列をバッファに拡張
	// todo: シェーダーファイルをバイナリ用でも扱えるようにいずれする
	std::stringstream sstream;
	sstream << shaderFile.rdbuf();
	std::string contents = sstream.str();
	const char* pContensChar = contents.c_str();

	// shaderオブジェクト作成
	in_rOutShader = glCreateShader(in_shaderType);
	// シェーダー文字列を指定
	glShaderSource(in_rOutShader, 1, &(pContensChar), nullptr);
	// シェーダーコンパイル
	glCompileShader(in_rOutShader);

	// コンパイル成功かチェック
	if (this->_IsCompiled(in_rOutShader) == false)
	{
		SDL_Log("Failed to compile shader file => %s", in_rFileName.c_str());

		return false;
	}

	// コンパイル成功
	SDL_Log("Success to compile shader file => %s", in_rFileName.c_str());

	return true;
}

// シェーダーがちゃんとコンパイルしているか
auto Shader::_IsCompiled(GLuint in_shader) -> bool
{
	GLint status;
	// コンパイル状態を取得
	glGetShaderiv(in_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		return true;
	}

	// コンパイル失敗!
	// todo: ログファイルにも出したいな
	char buffer[512];
	memset(buffer, 0, 512);
	glGetShaderInfoLog(in_shader, 511, nullptr, buffer);
	SDL_Log("GLSL Compile Failed:\n%s", buffer);

	return false;
}

// シェーダーが有効化
auto Shader::_IsValidProgram() -> bool
{
	GLint status;
	// コンパイル状態を取得
	glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_TRUE)
	{
		return true;
	}

	// コンパイル失敗!
	// todo: ログファイルにも出したいな
	char buffer[512];
	memset(buffer, 0, 512);
	glGetProgramInfoLog(this->shaderProgram, 511, nullptr, buffer);
	SDL_Log("GLSL Link Failed Status:\n%s", buffer);

	return false;
}

auto Shader::_ValidateUniform(const char* in_pUniformName, const char* in_pAttrName) -> void
{
	switch (glGetError())
	{
	case GL_INVALID_VALUE:
	{
		SDL_Log("shader uniform invalid value: %s uniform name => %s", in_pUniformName, in_pAttrName);
		break;
	}
	case GL_INVALID_OPERATION:
	{
		SDL_Log("shader uniform invalid operation: %s uniform name => %s", in_pUniformName, in_pAttrName);
		break;
	}
	default:
		break;
	}
}