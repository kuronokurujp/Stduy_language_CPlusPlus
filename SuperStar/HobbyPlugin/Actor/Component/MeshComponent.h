#ifndef __MESH_COMPONENT_H__
#define __MESH_COMPONENT_H__

#include "Component/component.h"

class Mesh;
class Shader;

class MeshComponent : public Component
{
public:
	MeshComponent(Actor* in_pOwner);
	~MeshComponent();

	void Update(const float in_deltaTime) override;
	void Draw(Shader* in_pShader);

	virtual void SetMesh(Mesh* in_pMesh)
	{
		this->pMesh = in_pMesh;
	}

	void SetTextureIndex(const int in_index) { this->textureIndex = in_index; }

private:
	void _Clear()
	{
		this->pMesh = nullptr;
		this->textureIndex = 0;
	}

	Mesh* pMesh;
	int textureIndex;
};
#endif // __MESH_COMPONENT_H__
