// ゲームシーンのモデル
#ifndef __GAME_SCENE_MODEL_H__
#define __GAME_SCENE_MODEL_H__

#include "Scene/Game/Interface/board_interface.h"

// 前方宣言
class UserActor;
class BoardActor;
class TerminalActor;
class ActorManager;
class RenderingInterface;
class KeyboardInterface;

class GameSceneModel
{
public:
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

	GameSceneModel();
	~GameSceneModel();

	void Initlize(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard);

	eGameState GetState() const { return this->_gameState; }
	// TODO: 中身を作る
	void GameStart();
	void GameReset();
	void BeginTurn();
	bool UpdateGame();
	void EndTurn();

	void UpdateActor(const float in_deltaTimeSecond);

private:
	void _Clear();

	ActorManager* _pActorManger;
	KeyboardInterface* _pKeyboard;

	// 指し手のユーザー達の情報
	uUserInfoGroup _userInfoGroup;

	// 現在のターンで指し手している最中のユーザー
	sUserInfo* _pNowTurnPlayUser;

	BoardActor* _p_board;

	// ターミナル
	TerminalActor* _pTerminal;

	eGameState _gameState;

	unsigned int _turn;
};

#endif // __GAME_SCENE_MODEL_H__
