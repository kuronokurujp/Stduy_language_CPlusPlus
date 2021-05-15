// ゲームシーンのモデル
#ifndef __GAME_SCENE_MODEL_H__
#define __GAME_SCENE_MODEL_H__

#include "Component/input_component.h"

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
	enum enum_game_state
	{
		eGameState_Boot = 0,
		eGameState_BeginGame,
		eGameState_UpdateGame,
		eGameStaet_EndGame,
	};

	// ユーザー情報
	struct _struct_user_info_
	{
		BoardData::eStone stone;
		UserActor* pUserActor;
	};

	union _union_user_info_group_
	{
		_struct_user_info_ aUsers[2];
		_struct_user_info_ player, enemy;
	};

	GameSceneModel();
	~GameSceneModel();

	void Initlize(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard);

	enum_game_state GetState() const { return this->_game_state; }
	// TODO: 中身を作る
	void GameStart();
	void GameReset();
	void BeginTurn();
	bool UpdateGame();
	void EndTurn();

	void UpdateActor(const float in_deltaTimeSecond);

	void NoticeTouchEvent(const InputComponent::eTouchEvent in_type, std::vector<InputComponent::_touch_event_data_>& in_datas);
private:
	void _Clear();

	ActorManager* _p_actor_manger;
	KeyboardInterface* _p_keyboard;

	// 指し手のユーザー達の情報
	_union_user_info_group_ _user_info_group;

	// 現在のターンで指し手している最中のユーザー
	_struct_user_info_* _p_now_turn_playuser;

	BoardActor* _p_board;

	// ターミナル
	TerminalActor* _p_terminal;

	enum_game_state _game_state;

	unsigned int _turn;
};

#endif // __GAME_SCENE_MODEL_H__
