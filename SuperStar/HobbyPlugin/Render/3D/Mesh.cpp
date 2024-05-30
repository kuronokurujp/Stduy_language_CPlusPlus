#include "3D/mesh.h"
#include "3D/vertexArray.h"

#include "Math/math.h"
#include "Math/vector3.h"

#include "renderer.h"

#include <fstream>
#include <sstream>
#include <SDL/SDL.h>
#include <rapidjson/document.h>

Mesh::Mesh()
{
	this->_Clear();
}

Mesh::~Mesh()
{
	this->Unload();
}

auto Mesh::Load(const std::string& in_rFileName, Renderer* in_pRenderer) -> bool
{
	// mehsファイルを開く
	std::ifstream meshFile(in_rFileName);
	if (meshFile.is_open() == false)
	{
		// ファイルを開けなかった
		SDL_Log("Failed open not mesh file => %s", in_rFileName.c_str());
		return false;
	}

	// jsonとしてロード
	std::stringstream sstream;
	sstream << meshFile.rdbuf();
	std::string contents = sstream.str();
	rapidjson::StringStream jsonStream(contents.c_str());
	rapidjson::Document document;
	document.ParseStream(jsonStream);

	// jsonフォーマットに従っているかチェック
	if (document.IsObject() == false)
	{
		SDL_Log("Mesh File is not valid json => %s", in_rFileName.c_str());
		return false;
	}

	// バージョンのチェック
	int ver = document["version"].GetInt();
	if (ver != 1)
	{
		SDL_Log("Mesh File Version is not 1 != %d => %s", ver, in_rFileName.c_str());
		return false;
	}

	// シェーダファイル名
	this->shaderName = document["shader"].GetString();

	// 鏡面反射値
	this->specPower = static_cast<float>(document["specularPower"].GetDouble());

	// テクスチャ情報取得
	{
		const rapidjson::Value& documentTextures = document["textures"];
		if (!documentTextures.IsArray() || documentTextures.Size() <= 0)
		{
			// 少なくとも一つは必ずテクスチャデータがある！
			SDL_Log("Mesh File Textures has not documentTextures, there should be at least one => %s", in_rFileName.c_str());
			return false;
		}

		for (rapidjson::SizeType i = 0; i < documentTextures.Size(); ++i)
		{
			auto textureFilePath = documentTextures[i].GetString();
			auto texture = in_pRenderer->GetTexture(textureFilePath);
			if (texture == nullptr)
			{
				// テクスチャデータがない場合はデフォルトを用意する
				texture = in_pRenderer->GetTexture("Resources/Texture/Default.png");
			}

			if (texture == nullptr)
			{
				continue;
			}

			// リストに追加
			this->textures.emplace_back(texture);
		}
	}

	this->radius = 0.0f;

	// 頂点情報取得
	std::vector<float> vertices;
	{
		const rapidjson::Value& verticesJson = document["vertices"];
		if (!verticesJson.IsArray() || verticesJson.Size() <= 0)
		{
			// 少なくとも一つは必ず頂点がある！
			SDL_Log("Mesh File has not vertices, there should be at least one => %s", in_rFileName.c_str());
			return false;
		}

		vertices.reserve(VertexArray::ItemMax * verticesJson.Size());
		for (rapidjson::SizeType i = 0; i < verticesJson.Size(); ++i)
		{
			const rapidjson::Value& vert = verticesJson[i];
			if (!vert.IsArray() || vert.Size() != VertexArray::ItemMax)
			{
				// 頂点フォーマットにミス!
				SDL_Log("unexpected vertex format for %s", in_rFileName.c_str());
				return false;
			}

			Math::Vector3 pos(
				static_cast<float>(vert[0].GetDouble()),
				static_cast<float>(vert[1].GetDouble()),
				static_cast<float>(vert[2].GetDouble()));

			// 球サイズは各頂点の中でもっとも大きい長さを採用する
			this->radius = Math::Max(Math::Vector3::GetVector3Mag(pos), this->radius);

			for (rapidjson::SizeType vertIndex = 0; vertIndex < vert.Size(); ++vertIndex)
			{
				double v = vert[vertIndex].GetDouble();
				vertices.emplace_back(static_cast<float>(v));
			}
		}
	}

	// 頂点インデックス情報取得
	std::vector<unsigned int> indices;
	{
		const rapidjson::Value& indicesJson = document["indices"];
		if (!indicesJson.IsArray() || indicesJson.Size() <= 0)
		{
			// 少なくとも一つは必ず頂点インデックスがある！
			SDL_Log("Mesh File has not indices, there should be at least one => %s", in_rFileName.c_str());
			return false;
		}

		indices.reserve(indicesJson.Size() * 3);
		for (rapidjson::SizeType i = 0; i < indicesJson.Size(); ++i)
		{
			const rapidjson::Value& index = indicesJson[i];
			if (!index.IsArray() || index.Size() != 3)
			{
				// 頂点インデックスフォーマットにミス!
				SDL_Log("invalide indices for %s", in_rFileName.c_str());
				return false;
			}

			for (rapidjson::SizeType j = 0; j < index.Size(); ++j)
			{
				indices.emplace_back(index[j].GetUint());
			}
		}
	}

	// jsonの頂点と頂点インデックスから頂点配列作成
	this->pVertexArray = new VertexArray(
		vertices.data(), static_cast<unsigned>(vertices.size() / VertexArray::ItemMax),
		indices.data(), static_cast<unsigned>(indices.size()));

	return true;
}

void Mesh::Unload()
{
	SAFETY_MEM_RELEASE(this->pVertexArray);
}

Texture* Mesh::GetTexture(const int in_index)
{
	if (static_cast<int>(this->textures.size()) <= in_index)
	{
		return nullptr;
	}

	return this->textures[in_index];
}