#include "Window/Code/Header/GUIWindow/gui_window_controller.h"

#include "Window/Code/Header/GUIWindow/Model/gui_window_model.h"
#include "Window/Code/Header/GUIWindow/View/Win/gui_window_win_view.h"

// 制御モデルは外部から受け取る
GUIWindowController::GUIWindowController(
	std::shared_ptr<GUIWindowModel> in_model,
	std::shared_ptr<GUIWindowWinView> in_view)
{
	this->_model_ptr = in_model;
	this->_view_ptr = in_view;
}

// 制御開始!
bool GUIWindowController::Start()
{
	this->_view_ptr->CreateObject(this);
	return true;
}

// 制御終了
void GUIWindowController::End()
{
}

void GUIWindowController::Render()
{
	// モデル側で描画データを更新
	this->_model_ptr->UpdateRender();
}

void GUIWindowController::EndRender()
{
	// モデルデータでView内容を書き換える
	this->_view_ptr->WriteRender(this->_model_ptr);
}