#include "Engine/MiniEngine.h"

// デバッグ用なのでリリース版には含めない
#ifdef HE_ENGINE_DEBUG

#include "Engine/Common/CustomMap.h"
#include "Engine/Math/Math.h"
#include "LevelLauncher.h"
#include "LevelTitle.h"

// UIモジュール
#include "UIModule.h"

// TODO: デバッグ画面を表示してレベルを切り替える

namespace Level
{
    const Bool LevelLauncher::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        HE_ASSERT(bRet);

        // UIイベントをキャッチするコンポーネントを追加
        {
            auto h    = this->AddComponent<Level::LevelUserInputReceiveComponent>(0);
            auto comp = this->GetComponent<Level::LevelUserInputReceiveComponent>(h);

            // auto handler = Core::Memory::MakeCustomUniqueShared<Level::LevelUserInputMessage>(//
            // std::make_unique<Level::LevelUserInputMessage>(
            auto handler = std::make_unique<Level::LevelUserInputMessage>(
                [this](const Char* in_pMsg)
                {
                    HE_LOG_LINE(in_pMsg);
                    // TODO: 次のレベルへ遷移
                    if (HE_STR_CMP(in_pMsg, HE_STR_TEXT("C_TitleSeq")) == 0)
                    {
                        // TODO: タイトルへ遷移する
                        // レベル更新中なのにレベルを破棄してしまうので例外エラーになる
                        // レベル切り替えを考えないといけない
                        // ModuleLevel()->GetManager()->StartLevel<Level::LevelTitle>();
                    }
                });
            comp->SetReceiver(std::move(handler));
        }

        // UIのBuilderファイルからレイアウト作成
        this->_layoutAssetHandle = ModuleUI()->LoadAssetWithLayoutBuild(
            Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Debug"),
                             HE_STR_TEXT("Launcher.xml")));
        // widgetを作成
        // レベルが切り替わると自動的にwidgetは破棄される
        this->_uIWidgetHandle = ModuleUI()->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0);

        return TRUE;
    }

    const Bool LevelLauncher::End()
    {
        // ロードしたアセットを破棄
        ModuleUI()->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return TRUE;
    }

}  // namespace Level

#endif
