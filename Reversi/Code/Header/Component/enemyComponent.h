#ifndef __ENEMY_COMPONENT_H__
#define __ENEMY_COMPONENT_H__

#include "Component/component.h"

// 前方宣言
class AIAlphaBetaComponent;
class BoardControllerInteface;
class BoardStatusInteface;
class AIComponentInterface;
class UserActor;

class EnemyComponent final : public Component
{
public:
	EnemyComponent(
		UserActor* in_pOwner,
		UserActor* in_pEnemyOnwer,
		BoardControllerInteface* in_pBoardController,
		BoardStatusInteface* in_pBoardStatus);

	~EnemyComponent();

	void Update(const float in_deltaTimeSecond) override final;

private:
	void _Clear();

	AIAlphaBetaComponent* _pAI;
};

#endif // __ENEMY_COMPONENT_H__