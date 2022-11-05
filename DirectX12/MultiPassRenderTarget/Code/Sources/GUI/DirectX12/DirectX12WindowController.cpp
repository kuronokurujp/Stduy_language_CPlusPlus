#include "GUI/DirectX12/DirectX12WindowController.h"

#include "DirectX12/DirectX12RednerTarget.h"

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
        if (!this->_model->IsActivePostProcessRenderTarget())
            this->_view->BeginRender();
    }

    void DirectX12WindowController::EndRender()
    {
        if (this->_model->IsActivePostProcessRenderTarget())
        {
            this->_view->BeginRender();
            // TODO: ポストプロセス用に作成したレンダーターゲットをスクリーンに描画
            this->_model->GetPostProcessRenderTarget()->Render(this->_model->Context());
        }

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

    /// <summary>
    /// TODO: ポストプロセスのシステムに接続
    /// </summary>
    void DirectX12WindowController::ConnectPostProcessSystem()
    {
        this->_model->CreatePostProcessRenderTarget(
            this->_view->GetRTVDescHeap(),
            this->_view->GetDSVDescHeap(),
            this->_view->GetBackBuffer()
        );

        this->_view->SetRenderType(GUI::DirectX12WindowView::RenderType::AddPostProcesss);
    }

    /// <summary>
    /// TODO: ポストプロセスのシステムを遮断
    /// </summary>
    void DirectX12WindowController::BreakPostProcessSystem()
    {
        this->_model->ReleasePostProcessRenderTarget();
        this->_view->SetRenderType(GUI::DirectX12WindowView::RenderType::Normal);
    }

    void DirectX12WindowController::BeginPostProcessSystem()
    {
        if (!this->_model->IsActivePostProcessRenderTarget())
            return;

        auto context = this->_model->Context();
        auto render_target = this->_model->GetPostProcessRenderTarget();

        render_target->BeginWrite(context->cmd_list, this->_view->GetDSVDescHeap(), this->_model->GetClearClear());
    }

    void DirectX12WindowController::EndPostProcessSystem()
    {
        if (!this->_model->IsActivePostProcessRenderTarget())
            return;

        auto context = this->_model->Context();
        auto render_target = this->_model->GetPostProcessRenderTarget();
        render_target->EndWrite(context->cmd_list);
    }
}
