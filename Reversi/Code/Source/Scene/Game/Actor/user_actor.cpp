#include "Scene/Game/Actor/user_actor.h"

#include "Scene/Game/Component/stonehand_component.h"
#include "System/Interface/console_render_interface.h"

UserActor::UserActor(BoardData::eStone in_useStone, RenderingInterface* in_pConsoleRendering)
	: Actor()
{
	this->_stone = in_useStone;
	this->_pStoneHandComponent = new StoneHandComponent(this, in_useStone, in_pConsoleRendering->GetClsCharacteCode());
}

/// <summary>
/// 打ち手を始める.
/// </summary>
void UserActor::StartTurn(BoardControllerInteface* in_pBoardController, BoardStatusInteface* in_pBoardStatus)
{
	// アクターのステータスを変えて更新を始める
	this->SetState(Actor::eState_Active);

	this->_pStoneHandComponent->Reset(in_pBoardController, in_pBoardStatus);
}

void UserActor::UpdateActor(float in_deltaTimeSecond)
{
	switch (this->_pStoneHandComponent->GetState())
	{
	case StoneHandComponent::eState_Start:
	{
		// 開始
		break;
	}
	case StoneHandComponent::eState_Update:
	{
		// 打っている最中
		// 別のコンポーネントで処理しているので状態監視だけ

		break;
	}
	case StoneHandComponent::eState_End:
	{
		// 打ち手が打つのが終わった
		this->_EndStonePlacement();
		break;
	}
	}
}

/// <summary>
/// オブジェクトの状態をテキスト化.
/// </summary>
const char* UserActor::ToText()
{
	return this->_pStoneHandComponent->ToText();
}

/// <summary>
/// 打つのが終わった.
/// </summary>
void UserActor::_EndStonePlacement()
{
	this->SetState(eState_Pause);
}

void UserActor::_Clear()
{
	this->_pStoneHandComponent = NULL;
	this->_stone = BoardData::eStone_None;
}