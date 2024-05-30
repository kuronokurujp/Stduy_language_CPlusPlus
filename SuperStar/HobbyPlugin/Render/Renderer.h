#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <SDL/SDL.h>

#include <string>
#include <vector>
#include <unordered_map>

#include "common.h"
#include "Math/matrix4.h"

class VertexArray;
class Shader;
class SpriteComponent;
class MeshComponent;
class Texture;
class Mesh;

namespace Light
{
	class Directional;
};

/// <summary>
/// 描画下地
/// </summary>
class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initalize(const int in_width, const int in_height);
	bool CreateContext(SDL_Window* pWindow);
	bool Load();

	void Shutdown();

	void UnloadDate();

	void BeginDraw(const Color32& in_rRendererColor);
	void Draw();
	void EndDraw(SDL_Window* pWindow);

	/// <summary>
	/// Adds the sprite.
	/// </summary>
	/// <returns></returns>
	void AddSprite(SpriteComponent *in_pComponent);
	void AddMesh(MeshComponent* in_pComponent);

	/// <summary>
	/// Removes the sprite.
	/// </summary>
	/// <returns></returns>
	void RemoveSprite(SpriteComponent *in_pComponent);
	void RemoveMesh(MeshComponent* in_pComponent);

	Texture* GetTexture(const std::string& in_rFileName);

	Mesh* GetMesh(const std::string& in_rFileName);
	void DestroyAllMeshFreeMem();

	Light::Directional* GetDirectionalLight() { return this->pDirectionalLight; }
	Shader* GetSpriteShader() { return this->pSpriteShader; }
	Shader* GetMeshShader() const { return this->pMeshShader; }

	int GetWidth() const { return this->width; }
	int GetHeight() const { return this->height; }

	// View行列設定
	void SetViewMatrix(const Math::Matrix4& in_rViewMtx) { this->viewMtx = in_rViewMtx; }
	// Projection行列設定
	void SetProjection(const Math::Matrix4& in_rProjectionMtx) { this->projectionMtx = in_rProjectionMtx; }

	// 環境光の設定
	void SetAmbientLight(const Math::Vector3& in_rLight) { this->ambientLight = in_rLight; }

	// ライト設定
	void SetLightUniforms(Shader* in_pShader);

private:
	void _SetRenderColor(const Color32& in_rColor);

	bool _LoadShader();
	void _CreateSpriteVerts();

	void _Clear()
	{
		this->width = this->height = 0;
		this->pDirectionalLight = nullptr;
		this->pSpriteVertexArray = nullptr;
		this->pSpriteShader = nullptr;
		this->pMeshShader = nullptr;

		this->sprites.clear();
		this->meshComponents.clear();
		this->textures.clear();
	}

	// 2Dスプライトのリスト
	std::vector<SpriteComponent*> sprites;

	// Meshのリスト
	std::vector<MeshComponent*> meshComponents;

	// テクスチャリスト
	std::unordered_map<std::string, Texture*> textures;
	std::unordered_map<std::string, Mesh*> mesh;

	VertexArray* pSpriteVertexArray;

	// ライト関連
	Light::Directional* pDirectionalLight;
	Math::Vector3 ambientLight;

	Shader* pSpriteShader;
	Shader* pMeshShader;

	int width, height;
	Math::Matrix4 viewMtx;
	Math::Matrix4 projectionMtx;

	SDL_GLContext glContext;
};

#endif // __RENDERER_H__