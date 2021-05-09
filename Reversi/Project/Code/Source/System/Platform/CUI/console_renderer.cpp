#ifdef __CUI_GAME__
#include "System/Platform/CUI/console_renderer.h"

#include <string>
#include <iostream>
#include <stdio.h>

ConsoleRenderer::ConsoleRenderer(const ConsoleRenderer::sSetup& in_rSetup)
{
	this->_Clear();

	this->_consoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	// 制御するコンソールウィンドウサイズ設定
	{
		this->_consoleWindowRect = { 0, 0, in_rSetup.textWidth, in_rSetup.textHeight };
	}

	// コンソールのカーソルを非表示
	{
		constexpr CONSOLE_CURSOR_INFO cursor{ 1, FALSE };
		GetConsoleCursorInfo(this->_consoleOutputHandle, &this->_defaultConsoleCursorInfo);
		SetConsoleCursorInfo(this->_consoleOutputHandle, &cursor);
	}

	memcpy(&this->_setupData, &in_rSetup, sizeof(this->_setupData));
}

ConsoleRenderer::~ConsoleRenderer()
{
	// 終了時に変更前のコンソール画面の状態に戻す
	SetConsoleCursorInfo(this->_consoleOutputHandle, &this->_defaultConsoleCursorInfo);
}

/// <summary>
/// 描画レンダーの登録.
/// </summary>
/// <param name="in_pAddRender">登録する描画レンダー.</param>
void ConsoleRenderer::AddViewInterface(RenderViewInterface* in_pAddRender)
{
	this->_draws.push_back(in_pAddRender);
}

/// <summary>
/// 描画レンダーの解除.
/// </summary>
/// <param name="in_pRemoveRender">解除する描画レンダー.</param>
void ConsoleRenderer::RemoveViewInterface(RenderViewInterface* in_pRemoveRender)
{
	for (auto iter = this->_draws.begin(); iter != this->_draws.end(); ++iter)
	{
		if (*iter == in_pRemoveRender)
		{
			this->_draws.erase(iter);
			return;
		}
	}
}

/// <summary>
/// 描画.
/// </summary>
void ConsoleRenderer::Draw()
{
	// コマンドプロンプトウィンドウのウィンドウサイズ固定
	// カーソル表示を非表示
	{
		SetConsoleWindowInfo(this->_consoleOutputHandle, TRUE, &this->_consoleWindowRect);
		SetConsoleCursorPosition(this->_consoleOutputHandle, COORD{ 0, 0 });
	}

	// レンダリングの初期化
	{
		memset(this->_renderingHalfCharcterMap, '\0', sizeof(this->_renderingHalfCharcterMap));

		// 最後の文字以外に空白を埋める
		const char clsChacterCode = this->GetClsCharacteCode();
		memset(this->_renderingHalfCharcterMap, clsChacterCode, sizeof(this->_renderingHalfCharcterMap) - sizeof(this->_renderingHalfCharcterMap[0][0]));

		// 末尾に改行コードを埋め込む
		// 正し配列の終端にはすでに'\0'が入っているので最後の行はスキップ
		for (unsigned int charcterMapYIndex = 0; charcterMapYIndex < eHalfCharacterMapSize_Height - 1; ++charcterMapYIndex)
		{
			this->_renderingHalfCharcterMap[charcterMapYIndex][eHalfCharacterMapSize_Width] = '\n';
		}
	}

	for (auto render : this->_draws)
	{
		render->Draw(this);
	}

	// mapに書き込んだ半角文字を一括で表示
	// 要注意: 文字列のmap内に0があると0の要素までしか表示しないので注意
	puts(this->_renderingHalfCharcterMap[0]);
}

/// <summary>
/// 矩形文字の転送.
/// </summary>
void ConsoleRenderer::FlashRectHalfCharacter(
	const char* in_ppRectHalfCharcter,
	const int in_startPointX,
	const int in_startPointY,
	const unsigned int in_width,
	const unsigned int in_height)
{
	// 一行ずつ書き込む
	int mapY = 0;
	int mapX = 0;

	for (unsigned int y = 0; y < in_height; ++y)
	{
		mapY = y + in_startPointY;
		// 画面範囲外かチェック
		if ((mapY < 0) || (mapY >= this->_setupData.textHeight))
		{
			continue;
		}

		for (unsigned int x = 0; x < in_width; ++x)
		{
			mapX = x + in_startPointX;

			// 画面範囲外かチェック
			if ((mapX < 0) || (mapX >= this->_setupData.textWidth))
			{
				continue;
			}

			// 一文字ずつ書き込む
			this->_renderingHalfCharcterMap[mapY][mapX] = in_ppRectHalfCharcter[y * in_width + x];
		}
	}
}

/// <summary>
/// 描画する半角文字を1行単位で反映.
/// </summary>
void ConsoleRenderer::FlashLineHalfCharacter(
	const char* in_pRectHalfCharcter,
	const int in_startPointX,
	const int in_startPointY)
{
	int mapY = in_startPointY;

	// 画面範囲外かチェック
	if ((mapY < 0) || (mapY >= this->_setupData.textHeight))
	{
		return;
	}

	unsigned int width = static_cast<unsigned int>(strlen(in_pRectHalfCharcter));
	int mapX = 0;
	for (unsigned int x = 0; x < width; ++x)
	{
		mapX = x + in_startPointX;

		// 画面範囲外かチェック
		if ((mapX < 0) || (mapX >= this->_setupData.textWidth))
		{
			continue;
		}

		// 一文字ずつ書き込む
		this->_renderingHalfCharcterMap[mapY][mapX] = in_pRectHalfCharcter[x];
	}
}

/// <summary>
/// インスタンス生成時のメンバー変数の初期化.
/// </summary>
void ConsoleRenderer::_Clear()
{
	this->_draws.clear();
	memset(this->_renderingHalfCharcterMap, 0, sizeof(this->_renderingHalfCharcterMap));

	_consoleOutputHandle = NULL;
	memset(&this->_consoleWindowRect, 0, sizeof(this->_consoleWindowRect));
	memset(&this->_defaultConsoleCursorInfo, 0, sizeof(this->_defaultConsoleCursorInfo));
}
#endif