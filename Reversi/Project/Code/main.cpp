#include "System/Platform/CUI/console_renderer.h"
#include "System/Platform/CUI/keyboard.h"
#include "game_controller.h"
#include "System/fps.h"

#include "GUIWindow/Model/gui_window_model.h"
#include "GUIWindow/View/Win/gui_window_win_view.h"
#include "GUIWindow/gui_window_controller.h"

/// <summary>
/// プログラムメイン.
/// </summary>
/// <returns></returns>
int main(int argc, const char * argv[])
{
	/*
		// 描画レンダーの初期化
		const short rendererScreenWidth = 100;
		const short rendererScreenHeight = 40;
		ConsoleRenderer::sSetup rendererSetup(rendererScreenWidth, rendererScreenHeight);
		ConsoleRenderer renderer(rendererSetup);

		Keyboard keyboard;

		Fps fps;

		GameController gameController(&renderer, &keyboard);
		float deltaTimeSecond = 0.0f;

		while (gameController.IsPlaying())
		{
			deltaTimeSecond = fps.WaitUpdate();

			keyboard.Input();

			gameController.Update(deltaTimeSecond);

			renderer.Draw();
		}
		*/

	std::shared_ptr<GUIWindowModel> model = std::make_shared<GUIWindowModel>(600, 400);
	std::shared_ptr<GUIWindowWinView> view = std::make_shared<GUIWindowWinView>();

	GUIWindowController ctrl(model, view);
	if (ctrl.Start() == false)
		return -1;

	return 0;
}