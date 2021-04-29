// ゲームシーンの制御
#ifndef __GAME_SCENE_CONTROLLER_H__
#define __GAME_SCENE_CONTROLLER_H__

#include "Actor/actor.h"

// 前方宣言
class GameSceneModel;
class RenderingInterface;
class KeyboardInterface;

class GameSceneController : public Actor
{
public:
	GameSceneController(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard);

	void UpdateActor(float in_deltaTimeSecond) override;

private:
	void _Clear();

	GameSceneModel* _pModel;

	KeyboardInterface* _p_keyboard;
};

#endif // __GAME_SCENE_CONTROLLER_H__
