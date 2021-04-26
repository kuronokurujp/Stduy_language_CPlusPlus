﻿// ゲーム全体をコントロール

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

	/// <summary>
	/// ゲーム終了しているか検知.
	/// </summary>
	/// <returns>
	///   <c>true</c> if this instance is playing; otherwise, <c>false</c>.
	/// </returns>
	bool IsPlaying() { return true; }

private:
	void _Clear();

	ActorManager* _pActorManger;

	RenderingInterface* _pRendering;
	KeyboardInterface* _pKeyboard;
};

#endif // __GAME_WORLD_H__