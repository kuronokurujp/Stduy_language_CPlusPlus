#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <string>

namespace Math
{
	class Matrix4;
	class Vector3;
}

// シェーダー制御クラス
class Shader {
public:
	Shader();
	~Shader();

	// シェーダーをロード
	bool Load(const std::string& in_rVertName, const std::string& in_rFragName);
	// シェーダーをアンロード
	void Unload();
	// シェーダーを有効化
	// todo: 無効化も必要なのでは？
	void SetActive();
	// シェーダーに行列を渡す
	void SetMatrixUniform(const char* in_pName, const Math::Matrix4* in_pMat);
	// シェーダーにテクスチャを渡す
	void SetTextureUniform(const char* in_pName, unsigned int in_textureID);
	// シェーダーにベクトルを渡す
	void SetVectorUniform(const char* in_pName, Math::Vector3& in_rVec3);
	// シェーダーにfloatを渡す
	void SetFloatUniform(const char* in_pName, float in_value);

private:
	// シェーダーをコンパイル
	bool _CompileShader(
		const std::string& in_rFileName,
		GLenum in_shaderType,
		GLuint& in_rOutShader);

	// シェーダーがコンパイルしているか
	bool _IsCompiled(GLuint in_shader);
	// シェーダーが有効かチェック
	bool _IsValidProgram();

	void _ValidateUniform(const char* in_pUniformName, const char* in_pAttrName);

	void _Clear()
	{
		vertexShader = GL_NONE;
		fragShader = GL_NONE;
		shaderProgram = GL_NONE;
	}

private:
	GLuint vertexShader;
	GLuint fragShader;
	GLuint shaderProgram;
};

#endif // __SHADER_H__
