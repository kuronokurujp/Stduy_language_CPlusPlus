#ifndef __GUI_WINDOW_CONTROLLER_H__
#define __GUI_WINDOW_CONTROLLER_H__

#include <memory>

class GUIWindowModel;
class GUIWindowWinView;

// GUIWindowの制御クラス
class GUIWindowController
{
public:
	// 制御モデルは外部から受け取る
	GUIWindowController(
		std::shared_ptr<GUIWindowModel> in_model,
		std::shared_ptr<GUIWindowWinView> in_view);

	std::shared_ptr<GUIWindowModel> GetModel() const { return this->_model_ptr; }

	// 制御開始!
	bool Start();

	// 制御終了
	void End();

	// 描画
	virtual void Render();

private:
	std::shared_ptr<GUIWindowModel> _model_ptr;
	std::shared_ptr<GUIWindowWinView> _view_ptr;
};

#endif // __GUI_WINDOW_CONTROLLER_H__
