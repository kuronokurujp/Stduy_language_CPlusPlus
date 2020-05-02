#include "Actor/terminalActor.h"

#include "Component/terminalRenderComponent.h"

#include <sstream>

TerminalActor::TerminalActor(ConsoleRenderingInterface* in_pRenderingInterface)
{
	// レンダーコンポーネント追加
	this->_pRenderComponent = new TerminalRenderComponent(this, in_pRenderingInterface);
}

void TerminalActor::UpdateActor(float in_deltaTimeSecond)
{
}

/// <summary>
/// 画面クリア.
/// </summary>
void TerminalActor::Cls()
{
	this->_pRenderComponent->Cls();
}

/// <summary>
/// 表示するテキスト追加.
/// </summary>
void TerminalActor::PushText(const char* in_pText)
{
	// 一つ下にずらす
	this->_pRenderComponent->ScrollLineText();
	this->_pRenderComponent->WriteLineText(0, in_pText);
}

/// <summary>
/// 指定ライン位置にテキストを書き込み.
/// </summary>
void TerminalActor::WriteLineText(const unsigned int in_linePostion, const char* in_pText)
{
	this->_pRenderComponent->WriteLineText(in_linePostion, in_pText);
}

/// <summary>
/// ターミナルエリア全体にテキストを書き込む.
/// </summary>
void TerminalActor::WriteFullAreaText(const char* in_pText)
{
	std::stringstream ss{ in_pText };
	std::string s;

	unsigned int linePosition = 0;

	// 文字列途中に改行コードがあれば、一行ずらして表示
	while (std::getline(ss, s, '\n'))
	{
		if (this->_pRenderComponent->WriteLineText(linePosition, s.c_str()) == false)
		{
			break;
		}

		++linePosition;
	}
}

void TerminalActor::_Clear()
{
	this->_pRenderComponent = NULL;
}