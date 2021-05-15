// ゲーム全体をコントロール
#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include <vector>

// タッチイベントをコンポーネントに通知するために必要
#include "Component/input_component.h"

// 前方宣言
class RenderingInterface;
class KeyboardInterface;
class ActorManager;

class GameController
{
public:
	GameController(RenderingInterface* in_pRendering, KeyboardInterface* in_pKeyboard);
	~GameController();

	void Update(const float in_deltaTimeSecond);

	void Render();
	// レンダー遅延
	void EndRender();

	// タッチイベント
	void OnTouchEvent(
		const InputComponent::eTouchEvent in_type,
		std::vector<InputComponent::_touch_event_data_>& in_datas);

private:
	void _Clear();

	ActorManager* _p_actor_manger;

	RenderingInterface* _pRendering;
	KeyboardInterface* _p_keyboard;
};

#endif // __GAME_WORLD_H__
