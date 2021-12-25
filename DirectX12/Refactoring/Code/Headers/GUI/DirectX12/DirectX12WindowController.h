#pragma once

#include "GUI/GUIMiniHeader.h"

namespace GUI
{
	class DirectX12WindowController : public WindowControllerInterface
	{
	public:
		// 制御モデルは外部から受け取る
		DirectX12WindowController(std::shared_ptr<DirectX12WindowModel> in_model, std::shared_ptr<DirectX12WindowView> in_view);

		// 制御開始!
		const bool Start() override final;

		// 制御終了
		void End() override final;

		// インプット入力などアプリ更新前に必要な処理
		void PostUpdate() override final;

		// 更新の開始と終了
		void BeginUpdate() override final;
		void EndUpdate() override final;

		// 更新終了
		const bool IsUpdate() override final;

		// タッチイベント
		void OnTouchEvent(int in_type, const int in_x, const int in_y) override final {}
		// アプリ終了イベント
		void OnAppQuitEvent() override final;

		const WindowCommonData& GetWindowData() override final;

		std::shared_ptr<DirectX12WindowModel> GetModel() { return this->_model; }

	private:
		std::shared_ptr<DirectX12WindowModel> _model;
		std::shared_ptr<DirectX12WindowView> _view;

		bool _b_show = false;
		WindowCommonData _window_common_data = {};
	};
}
