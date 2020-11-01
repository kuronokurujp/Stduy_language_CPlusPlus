#include "Scene/Game/Component/enemy_component.h"

#include "Scene/Game/Actor/user_actor.h"
#include "Scene/Game/Component/stonehand_component.h"
#include "Scene/Game/Component/AI/ai_alphabeta_component.h"

EnemyComponent::EnemyComponent(
	UserActor* in_pOwner,
	UserActor* in_pEnemyOnwer,
	BoardControllerInteface* in_pBoardController,
	BoardStatusInteface* in_pBoardStatus)
	: Component(in_pOwner)
{
	this->_pAI = new AIAlphaBetaComponent(in_pOwner, in_pEnemyOnwer, in_pBoardController, in_pBoardStatus);
}

EnemyComponent::~EnemyComponent()
{
	this->_pOwner->RemoveComponentAndMemFree(this->_pAI);
}

void EnemyComponent::Update(const float in_deltaTimeSecond)
{
	StoneHandComponent* pStoneHandComponent = this->_pOwner->GetComponent<StoneHandComponent*>();
	switch (pStoneHandComponent->GetState())
	{
	case StoneHandComponent::eState_Start:
	{
		pStoneHandComponent->Start();

		this->_pAI->Start();

		break;
	}
	case StoneHandComponent::eState_Update:
	{
		pStoneHandComponent->SetObjectText("Enemy Thinking ...");

		// ターン終了
		switch (this->_pAI->GetState())
		{
		case AIAlphaBetaComponent::eState_SuccessPlacement:
		{
			pStoneHandComponent->SetPlacementStone(this->_pAI->GetPlacementPoint());
		}
		case AIAlphaBetaComponent::eState_SkipPlacement:
		{
			pStoneHandComponent->End();
			break;
		}
		}

		break;
	}
	}
}

void EnemyComponent::_Clear()
{
	this->_pAI = NULL;
}