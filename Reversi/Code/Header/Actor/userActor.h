#ifndef __USER_ACTOR_H__
#define __USER_ACTOR_H__

#include "Actor/actor.h"
#include "Interface/boardInterface.h"

// 前方宣言
class StoneHandComponent;
class ConsoleRenderingInterface;

class UserActor final : public Actor
{
public:
	UserActor(BoardData::eStone in_useStone, ConsoleRenderingInterface* in_pConsoleRendering);

	/// <summary>
	/// 打ち手を始める.
	/// </summary>
	void StartTurn(BoardControllerInteface* in_pBoardController, BoardStatusInteface* in_pBoardStatus);

	// 実装する
	void UpdateActor(float in_deltaTimeSecond) override final;

	/// <summary>
	/// 指している最中.
	/// </summary>
	bool IsPlaying() { return this->GetState() == Actor::eState_Active; }

	/// <summary>
	/// オブジェクトの状態をテキスト化.
	/// </summary>
	const char* ToText();

	const BoardData::eStone GetUseStone() const { return this->_stone; }

private:
	/// <summary>
	/// 打つのが終わった.
	/// </summary>
	void _EndStonePlacement();

	void _Clear();

	StoneHandComponent* _pStoneHandComponent;
	BoardData::eStone _stone;
};

#endif // __USER_ACTOR_H__
