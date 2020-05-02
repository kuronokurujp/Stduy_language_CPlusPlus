#ifndef __TERMINAL_ACTOR_H__
#define __TERMINAL_ACTOR_H__

#include "Actor/actor.h"

// 前方宣言
class ConsoleRenderingInterface;
class TerminalRenderComponent;

/// <summary>
/// ゲームログやコマンドを扱うターミナル
/// </summary>
/// <seealso cref="Actor" />
class TerminalActor final : public Actor
{
public:
	TerminalActor(ConsoleRenderingInterface* in_pRenderingInterface);
	void UpdateActor(float in_deltaTimeSecond) override;

	/// <summary>
	/// 画面クリア.
	/// </summary>
	void Cls();

	/// <summary>
	/// 表示するテキスト追加.
	/// </summary>
	void PushText(const char* in_pText);

	/// <summary>
	/// 指定ライン位置にテキストを書き込み.
	/// </summary>
	void WriteLineText(const unsigned int in_linePostion, const char* in_pText);

	/// <summary>
	/// ターミナルエリア全体にテキストを書き込む.
	/// </summary>
	void WriteFullAreaText(const char* in_pText);

private:
	void _Clear();

	TerminalRenderComponent* _pRenderComponent;
};

#endif // __TERMINAL_ACTOR_H__
