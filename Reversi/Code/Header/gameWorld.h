#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include "Interface/boardInterface.h"

// 前方宣言
class UserActor;
class BoardActor;
class TerminalActor;
class ConsoleRenderingInterface;
class KeyboardInterface;
class ActorManager;

class GameWorld
{
public:
	GameWorld(ConsoleRenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard);
	~GameWorld();

	void Update(const float in_deltaTimeSecond);

private:
	// ゲーム状態
	enum eGameState
	{
		eGameState_Boot = 0,
		eGameState_BeginGame,
		eGameState_UpdateGame,
		eGameStaet_EndGame,
	};

	// ユーザー情報
	struct sUserInfo
	{
		BoardData::eStone stone;
		UserActor* pUserActor;
	};

	union uUserInfoGroup
	{
		sUserInfo aUsers[2];
		sUserInfo player, enemy;
	};

	void _Clear();

	ActorManager* _pActorManger;

	// 指し手のユーザー達の情報
	uUserInfoGroup _userInfoGroup;

	// 現在のターンで指し手している最中のユーザー
	sUserInfo* _pNowTurnPlayUser;

	BoardActor* _pBoard;

	// ターミナル
	TerminalActor* _pTerminal;

	ConsoleRenderingInterface* _pRendering;
	KeyboardInterface* _pKeyboard;

	eGameState _gameState;

	unsigned int _turn;
};

#endif // __GAME_WORLD_H__
