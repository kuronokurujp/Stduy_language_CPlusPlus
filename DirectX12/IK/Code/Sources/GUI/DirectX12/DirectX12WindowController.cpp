#include "GUI/DirectX12/DirectX12WindowController.h"

namespace GUI
{
    // 制御モデルは外部から受け取る

    DirectX12WindowController::DirectX12WindowController(std::shared_ptr<DirectX12WindowModel> in_model, std::shared_ptr<DirectX12WindowView> in_view)
    {
        this->_model = in_model;
        this->_view = in_view;

        this->_b_show = false;

        this->_window_common_data.width = this->_model->Width();
        this->_window_common_data.height = this->_model->Height();
    }

    // 制御開始!
    const bool DirectX12WindowController::Start()
    {
        this->_view->CreateObject(this);

        return true;
    }

    // 制御終了
    void DirectX12WindowController::End()
    {
    }

    // 更新の開始と終了
    void DirectX12WindowController::PostUpdate()
    {
        this->_view->PostUpdate();
    }

    void DirectX12WindowController::BeginRender()
    {
        this->_view->BeginRender();
    }

    void DirectX12WindowController::EndRender()
    {
        this->_view->EndRender();

        // 更新前にウィンドウを表示してしまうとレンダリング書き込み前の表示が1フレームだけ見えてしまう。
        // それを防ぐために更新が終わった後に一度のみウィンドウ表示をする
        if (this->_b_show == false)
        {
            this->_b_show = true;
            this->_view->Show();
        }
    }

    const bool DirectX12WindowController::IsUpdate()
    {
        return this->_model->IsUpdate();
    }

    // アプリ終了イベント
    void DirectX12WindowController::OnAppQuitEvent()
    {
        this->_model->EnableUpdate(false);
    }

    const WindowCommonData& DirectX12WindowController::GetWindowData()
    {
        return this->_window_common_data;
    }
}
