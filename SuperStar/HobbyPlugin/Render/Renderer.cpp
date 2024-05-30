#include "renderer.h"

#include "texture.h"

#include "common.h"

#include "3D/mesh.h"
#include "3D/vertexArray.h"
#include "3D/shader.h"
#include "3D/directionalLight.h"

#include "Math/matrix4.h"
#include "Math/math.h"

#include "Component/spriteComponent.h"
#include "Component/meshComponent.h"

Renderer::Renderer()
{
	this->_Clear();
}

Renderer::~Renderer()
{
	this->Shutdown();
}

bool Renderer::Initalize(const int in_width, const int in_height)
{
	this->width = in_width;
	this->height = in_height;

	// openglの属性設定をする
	// windowを作成する前にしないといけない
	{
		// コア機能を有効にする
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// コンテキストのバージョン設定
		// OpenGL3.3を使用
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

		// RGBAそれぞれに割り当てるビットサイズ指定
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		// デプスバッファのビット数を設定
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		// 描画のダブルバッファリングを有効にする
		// これで描画くずれが防げる
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// 描画計算をGPU任せにする(0ならGPU任せになる)
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	}

	// 環境光作成
	{
		// 適当に明るく
		this->ambientLight.Set(0.2f);
	}

	// 平行光源作成
	{
		this->pDirectionalLight = new Light::Directional();
	}

	return true;
}

bool Renderer::CreateContext(SDL_Window* pWindow)
{
	// Windowに紐づいているOpenGLのコンテキストを生成
	// これを利用してOpenGLの機能を使う
	this->glContext = SDL_GL_CreateContext(pWindow);
	if (this->glContext == nullptr)
	{
		SDL_Log("Falide to create glcontext: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool Renderer::Load()
{
	if (!this->_LoadShader())
	{
		return false;
	}

	this->_CreateSpriteVerts();

	return true;
}

void Renderer::Shutdown()
{
	this->UnloadDate();

	if (this->glContext != nullptr)
	{
		SDL_GL_DeleteContext(this->glContext);
		this->glContext = nullptr;
	}
	else
	{
		SDL_Log("Unable to shutdown SDK to Unable to create glcontext");
	}
}

void Renderer::UnloadDate()
{
	// テクスチャを全て破棄
	{
		for (auto tex : this->textures)
		{
			tex.second->Unload();
			SAFETY_MEM_RELEASE(tex.second);
		}

		this->textures.clear();
	}

	// シェーダーを破棄
	if (this->pSpriteShader != nullptr)
	{
		SAFETY_MEM_RELEASE(this->pSpriteShader);
	}

	if (this->pMeshShader != nullptr)
	{
		SAFETY_MEM_RELEASE(this->pMeshShader);
	}
}

void Renderer::BeginDraw(const Color32& in_rRendererColor)
{
	// 描画画面の色を決める
	this->_SetRenderColor(in_rRendererColor);

	// カラーバッファをクリアする
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw()
{
	// 3D描画開始
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	if (this->pMeshShader != nullptr)
	{
		// 3Dメッシュ描画
		this->pMeshShader->SetActive();

		Math::Matrix4 viewProj = this->viewMtx;
		viewProj.Mul(this->projectionMtx);
		this->pMeshShader->SetMatrixUniform("uViewProj", &viewProj);

		// 平行光源設定
		this->SetLightUniforms(this->pMeshShader);

		for (auto meshComponent : this->meshComponents)
		{
			meshComponent->Draw(this->pMeshShader);
		}
	}

	// 2D描画開始

	// 描画のブレンド設定
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// srcFactor * srcAlpha + (1 - srcAlpha) * dstFactor
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	{
		// 2Dスプライトの描画
		this->pSpriteShader->SetActive();
		this->pSpriteVertexArray->SetActive();
		{
			for (auto sprite : this->sprites)
			{
				sprite->Draw();
			}
		}
	}
}

void Renderer::EndDraw(SDL_Window* pWindow)
{
	// ウィンドウの描画バッファを切り替える
	SDL_GL_SwapWindow(pWindow);
}

/// <summary>
/// Adds the sprite.
/// </summary>
/// <returns></returns>
void Renderer::AddSprite(SpriteComponent *in_pComponent)
{
	SDL_assert((in_pComponent != nullptr));

	// 描画オーダーを小さい値から先に描画するためにオーダー値を参照してリストのオーダー値より低い箇所に挿入する
	auto iter = this->sprites.begin();
	const int drawOrder = in_pComponent->GetDrawOrder();
	for (; iter != this->sprites.end(); ++iter)
	{
		if (drawOrder < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	this->sprites.insert(iter, in_pComponent);
}

void Renderer::AddMesh(MeshComponent* in_pComponent)
{
	SDL_assert((in_pComponent != nullptr));

	this->meshComponents.push_back(in_pComponent);
}

/// <summary>
/// Removes the sprite.
/// </summary>
/// <returns></returns>
void Renderer::RemoveSprite(SpriteComponent *in_pComponent)
{
	SDL_assert((in_pComponent != nullptr));
	RemoveObjects<SpriteComponent>(in_pComponent, this->sprites);
}

void Renderer::RemoveMesh(MeshComponent* in_pComponent)
{
	SDL_assert((in_pComponent != nullptr));
	RemoveObjects<MeshComponent>(in_pComponent, this->meshComponents);
}

Texture* Renderer::GetTexture(const std::string& in_rFileName)
{
	Texture* pTexture = nullptr;

	// すでにテクスチャーデータが存在しているかチェック
	{
		auto iter = this->textures.find(in_rFileName);
		if (iter != this->textures.end())
		{
			if (iter->second != nullptr)
			{
				return iter->second;
			}
		}
	}

	pTexture = new Texture();
	if (pTexture->Load(in_rFileName) == false)
	{
		SAFETY_MEM_RELEASE(pTexture);
		SDL_Log("Failed to convert surface to texture for %s", in_rFileName.c_str());
		return nullptr;
	}

	this->textures[in_rFileName] = pTexture;

	return pTexture;
}

Mesh* Renderer::GetMesh(const std::string& in_rFileName)
{
	auto iter = this->mesh.find(in_rFileName);
	if (iter != this->mesh.end())
	{
		if (iter->second != nullptr)
		{
			return iter->second;
		}

		this->mesh.erase(iter);
	}

	auto pMeshData = new Mesh();
	if (pMeshData->Load(in_rFileName, this) == false)
	{
		SAFETY_MEM_RELEASE(pMeshData);
		SDL_Log("Failed to convert surface to meshComponent for %s", in_rFileName.c_str());

		return nullptr;
	}

	this->mesh[in_rFileName] = pMeshData;

	return pMeshData;
}

void Renderer::DestroyAllMeshFreeMem()
{
	for (auto m : this->mesh)
	{
		SAFETY_MEM_RELEASE(m.second);
	}

	this->mesh.clear();
}

/// <summary>
/// Sets the light uniforms.
/// </summary>
/// <param name="in_pShader">The in p shader.</param>
void Renderer::SetLightUniforms(Shader* in_pShader)
{
	SDL_assert(in_pShader != nullptr);
	// シェーダーにパラメータを渡す
	Math::Matrix4 invMat;
	this->viewMtx.OutputInvert(&invMat);

	Math::Vector3 camPos;
	this->viewMtx.OutputTranslation(&camPos);
	in_pShader->SetVectorUniform("uCameraPos", camPos);

	// 環境光
	in_pShader->SetVectorUniform("uAmbientLight", this->ambientLight);

	// 平行光源
	auto pLightData = this->pDirectionalLight->GetDataPtr();
	in_pShader->SetVectorUniform("uDirLight._direction", pLightData->_direction);
	in_pShader->SetVectorUniform("uDirLight._diffuseColor", pLightData->_diffuseColor);
	in_pShader->SetVectorUniform("uDirLight._specColor", pLightData->_specColor);
}

/// <summary>
/// Sets the color of the render.
/// </summary>
/// <param name="in_color">Color of the in.</param>
void Renderer::_SetRenderColor(const Color32& in_rColor)
{
	// カラー値が0 - 1である。
	// Color32のカラー値は0 - 255なので0 - 1の範囲に変換した
	static float invColorRate = 1.0f / 255.0f;
	glClearColor(
		in_rColor.r * invColorRate,
		in_rColor.g * invColorRate,
		in_rColor.b * invColorRate,
		in_rColor.a * invColorRate);
}

bool Renderer::_LoadShader()
{
	// スプライトシェーダー作成
	{
		this->pSpriteShader = new Shader();
		// memo: shaderコードがs-jisでないとコード解析に失敗する！
		if (!this->pSpriteShader->Load("Resources/Shaders/Sprite.vert", "Resources/Shaders/Sprite.frag"))
		{
			return false;
		}
	}

	// シェーダーに正規正方系の座標変換の行列を渡す
	{
		this->pSpriteShader->SetActive();

		auto viewProj = Math::Matrix4::CreateSimpleViewProj(
			static_cast<float>(this->width),
			static_cast<float>(this->height));

		this->pSpriteShader->SetMatrixUniform("uViewProj", &viewProj);
	}

	// メッシュ用のシェーダー作成
	{
		this->pMeshShader = new Shader();
		// memo: shaderコードがs-jisでないとコード解析に失敗する！
		// ↓光の影響を受けないunlitシェーダー
		//if (!this->pMeshShader->Load("Resources/Shaders/BasicMesh.vert", "Resources/Shaders/BasicMesh.frag"))
		// todo: 各特徴を持つシェーダークラスが必要
		// todo: 拡散反射光のシェーダーが必要
		if (!this->pMeshShader->Load("Resources/Shaders/Phong.vert", "Resources/Shaders/Phong.frag"))
		{
			return false;
		}

		this->pMeshShader->SetActive();

		// 透視行列作成
		this->viewMtx = Math::Matrix4::CreateLookAt(Math::Vector3::Zero, Math::Vector3::Unit_X, Math::Vector3::Unit_Z);
		this->projectionMtx = Math::Matrix4::CreatePerspectiveFOV(
			Math::DegreesToRadians(70.0f),
			static_cast<float>(this->width),
			static_cast<float>(this->height),
			25.0f,
			10000.0f
		);

		auto viewProjMtx = this->viewMtx;
		viewProjMtx.Mul(this->projectionMtx);
		this->pMeshShader->SetMatrixUniform("uViewProj", &viewProjMtx);
	}

	return true;
}

auto Renderer::_CreateSpriteVerts() -> void
{
	if (this->pSpriteVertexArray != nullptr)
	{
		return;
	}

	// 頂点配列作成
	{
		float points[] =
		{
			// 座標軸がデカルト座標系に準拠
			// x    y     z    nx   ny   nz   u    v
			-0.5f,  0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, // top left
			 0.5f,  0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, // top right
			 0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, // bottom right
			-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f  // bottom left
		};

		unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		this->pSpriteVertexArray = new VertexArray(points, 4, indices, 6);
	}
}