#include "Common.h"

#include <locale.h>
#include <assert.h>
#include <tchar.h>
#include <filesystem>
#include <string>
#include <chrono>

#include "GUI/GUIMiniHeader.h"
#include "DirectX12/DirectX12MiniHeader.h"
#include "App/AppMiniHeader.h"

/// <summary>
/// Mains the specified argc.
/// </summary>
/// <param name="argc">The argc.</param>
/// <param name="argv">The argv.</param>
/// <returns></returns>
int main(int argc, const char* argv[])
{
    // TODO: コマンドプロンプトで日本語表示を可能にしたい
    {
        static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;

        std::string log_dir_path = "Save/Logs";
        std::filesystem::create_directories(log_dir_path.c_str());

        std::string log_file_path = log_dir_path + "/" + "log.txt";
        static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(log_file_path.c_str());
        plog::init(plog::debug, &consoleAppender).addAppender(&fileAppender);
        LOGD << _T("start application");
    }

    std::shared_ptr<DirectX12::Context> context = std::make_shared<DirectX12::Context>();

    // DirectX12の環境作成
    auto result = DirectX12::CreateEnvironment(context);
    assert(SUCCEEDED(result));

    // TODO: 本来なら使用アダプターなどを選択するダイアログを用意するのがいい
    LOGD << _T("directx12: adapters list: start");
    {
        DXGI_ADAPTER_DESC adesc = {};
        for (size_t i = 0; i < context->adapters.size(); ++i)
        {
            context->adapters[i]->GetDesc(&adesc);
            std::wstring str_desc = adesc.Description;
            LOGD << _T("no[") << i << _T("] = ") << str_desc;
        }
    }
    LOGD << _T("directx12: adapters list: end");

    // 指定アダプターを選択
    // NVIDIAのみしか利用できないようにする
    // 書籍「DirectX12の魔導書」のサンプル動作環境がNVIDIAだから
    const auto adaptar_name = _T("NVIDIA");
    LOGD << _T("use adapter name => ") << adaptar_name;
    auto adapter_idx = DirectX12::GetAdapterdx(context, adaptar_name);
    if (adapter_idx < 0)
    {
        // 指定したアダプターがない
        LOGD << _T("directx12: adapters missmatch: ") << adaptar_name;
        assert(false);
    }

    // DirectX12を使うための初期化
    {
        auto result = DirectX12::CreateSystem(context, adapter_idx);
        assert(SUCCEEDED(result));
    }

    // DirectX12用のGUI作成
    std::shared_ptr<GUI::DirectX12WindowModel> win_model = std::make_shared<GUI::DirectX12WindowModel>(context, 640, 480);
    std::shared_ptr<GUI::DirectX12WindowView> win_view = std::make_shared<GUI::DirectX12WindowView>();
    std::shared_ptr<GUI::DirectX12WindowController> win_ctrl = std::make_shared<GUI::DirectX12WindowController>(win_model, win_view);
    if (win_ctrl->Start() == false)
        assert(false);

    // ゲーム制御
    App::GameController game_ctrl(win_ctrl);
    if (game_ctrl.Start() == false)
        assert(false);

    // 現在日時を取得
    std::chrono::system_clock::time_point previous_clock = std::chrono::system_clock::now();
    double lag_s_time = 0.0;
    // 1フレームレートの秒数(60FPS)
    const double fixed_framerate_s_time = 1.0 / 60.0;
    do {
        std::chrono::system_clock::time_point current_clock = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_clock - previous_clock;
        previous_clock = current_clock;

        // 更新フレームを固定化している
        lag_s_time += elapsed_seconds.count();
        while (lag_s_time >= fixed_framerate_s_time)
        {
            // ウィンドウの前更新
            win_ctrl->PostUpdate();

            // ゲーム更新
            game_ctrl.Update();

            lag_s_time -= fixed_framerate_s_time;
        }

        // 描画開始
        win_ctrl->BeginRender();
        {
            // ゲーム専用の描画
            game_ctrl.Render();
        }
        // 描画終了
        win_ctrl->EndRender();
    } while (win_ctrl->IsUpdate());

    // ゲーム終了処理
    game_ctrl.End();

    // ウィンドウ終了処理
    win_ctrl->End();

    // DirectX12のシステムなどもろもろを破棄
    DirectX12::Delete(context);

    return 0;
}
