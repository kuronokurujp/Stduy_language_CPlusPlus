#ifndef __3D_MESH_H__
#define __3D_MESH_H__

#include <string>
#include <vector>

class Renderer;
class VertexArray;
class Texture;

/// <summary>
/// 3Dのメッシュクラス
/// </summary>
class Mesh
{
public:
    explicit Mesh();
    ~Mesh();

    bool Load(const std::string& in_rFileName, Renderer* in_pGame);
    void Unload();

    VertexArray* GetVertexArray() { return this->pVertexArray; }
    const std::string& GetShaderName() const { return this->shaderName; }
    float GetRadius() const { return this->radius; }
    Texture* GetTexture(const int in_index);

    inline const float GetSpecPower() { return this->specPower; }

private:
    std::vector<Texture*> textures;
    VertexArray* pVertexArray;
    std::string shaderName;
    float radius;
    float specPower;

    void _Clear()
    {
        this->textures.clear();
        this->pVertexArray = nullptr;
        this->shaderName   = "";
        this->radius       = 0.0f;
        this->specPower    = 0.0f;
    }
};

#endif  // __3D_MESH_H__