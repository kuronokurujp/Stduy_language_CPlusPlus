// コンソール専用描画
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <Windows.h>

#include "System/Interface/console_render_interface.h"

/// <summary>
/// コンソール描画システム.
/// 半角文字を画面に描画.
/// ※全角文字は使えない！.
/// </summary>
class ConsoleRenderer : public RenderingInterface
{
public:
	/// <summary>
	/// セットアップデータ.
	/// </summary>
	struct sSetup
	{
		sSetup()
		{
			this->textWidth = this->textHeight = 0;
		}

		sSetup(const short in_textWidth, const short in_textHeight)
		{
			this->textWidth = in_textWidth;
			this->textHeight = in_textHeight;
		}

		short textWidth, textHeight;
	};

	ConsoleRenderer(const sSetup& in_rSetup);
	virtual ~ConsoleRenderer();

	/// <summary>
	/// 描画インターフェイスの登録.
	/// </summary>
	void AddViewInterface(RenderViewInterface* in_pAddRender) override final;

	/// <summary>
	/// 描画インターフェイスの解除.
	/// </summary>
	void RemoveViewInterface(RenderViewInterface* in_pRemoveRender) override final;

	// レンダーのテキスト縦横サイズ
	const unsigned GetTextRenderWidth() override final { return this->_setupData.textWidth; }
	const unsigned GetTextRenderHeight() override final { return this->_setupData.textHeight; }

	// 画面クリア文字コード
	const char GetClsCharacteCode() override final { return ' '; };

	/// <summary>
	/// 描画.
	/// </summary>
	void Draw();

	/// <summary>
	/// 矩形文字の転送.
	/// memo: 文字列に0が途中で入っていると表示が途中で切れるので注意.
	/// </summary>
	void FlashRectHalfCharacter(
		const char* in_ppRectHalfCharcter,
		const int in_startPointX,
		const int in_startPointY,
		const unsigned int in_width,
		const unsigned int in_height) override final;

	/// <summary>
	/// 描画する半角文字を1行単位で反映.
	/// memo: 文字列に0が途中で入っていると表示が途中で切れるので注意.
	/// </summary>
	void FlashLineHalfCharacter(
		const char* in_pRectHalfCharcter,
		const int in_startPointX,
		const int in_startPointY) override final;

private:
	enum eHalfCharacterMapSize
	{
		eHalfCharacterMapSize_Width = 100,
		eHalfCharacterMapSize_Height = 34,
	};

	/// <summary>
	/// インスタンス生成時のメンバー変数の初期化.
	/// </summary>
	void _Clear();

	std::vector<RenderViewInterface*> _draws;

	// コンソール画面を制御に必要
	HANDLE _consoleOutputHandle;
	SMALL_RECT _consoleWindowRect;
	CONSOLE_CURSOR_INFO _defaultConsoleCursorInfo;

	sSetup _setupData;

	// 改行コード文を1文字横に増やす
	char _renderingHalfCharcterMap[eHalfCharacterMapSize_Height][(eHalfCharacterMapSize_Width + 1)];
};

#endif // __RENDERER_H__