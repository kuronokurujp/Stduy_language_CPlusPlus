#include "consoleRenderer.h"
#include "keyboard.h"
#include "gameWorld.h"
#include "fps.h"

/// <summary>
/// プログラムメイン.
/// </summary>
/// <returns></returns>
int main(int argc, const char * argv[])
{
	// 描画レンダーの初期化
	const short rendererScreenWidth = 100;
	const short rendererScreenHeight = 40;
	ConsoleRenderer::sSetup rendererSetup(rendererScreenWidth, rendererScreenHeight);
	ConsoleRenderer renderer(rendererSetup);

	Keyboard keyboard;

	Fps fps;

	GameWorld gameWorld(&renderer, &keyboard);
	float deltaTimeSecond = 0.0f;
	while (true)
	{
		deltaTimeSecond = fps.WaitUpdate();

		keyboard.Input();

		gameWorld.Update(deltaTimeSecond);

		renderer.Draw();
	}

	return 0;
}