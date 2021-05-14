// ゲーム全体をコントロール
#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

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

private:
	void _Clear();

	ActorManager* _p_actor_manger;

	RenderingInterface* _pRendering;
	KeyboardInterface* _p_keyboard;
};

#endif // __GAME_WORLD_H__
