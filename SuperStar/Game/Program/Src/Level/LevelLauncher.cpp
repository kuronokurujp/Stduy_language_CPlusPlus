#include "Engine.h"
#include "MiniEngine.h"

// デバッグ用なのでリリース版には含めない
#ifdef _HOBBY_ENGINE_DEBUG

#include "Core/Common/FixMap.h"
#include "Core/Math/Math.h"
#include "Level/LevelLauncher.h"

// UIモジュール
#include "HobbyPlugin/UI/UIModule.h"

// TODO: デバッグ画面を表示してレベルを切り替える

namespace Level
{
    const Bool LevelLauncher::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        E_ASSERT(bRet);

        // UIイベントをキャッチするコンポーネントを追加
        {
            auto h    = this->AddComponent<Level::LevelUserInputReceiveComponent>(0);
            auto comp = this->GetComponent<Level::LevelUserInputReceiveComponent>(h);

            auto handler = std::make_unique<Level::LevelUserInputMessage>(
                [this](const Char* in_pMsg)
                {
                    E_LOG_LINE(in_pMsg);
                    // TODO: 次のレベルへ遷移
                });
            comp->SetReceiver(std::move(handler));
        }

        // TODO: UIのBuilderファイルからレイアウト作成
        // 破棄処理を入れていない
        this->_hLayoutAsset = ModuleUI()->LoadAssetWithLayoutBuild(
            Core::File::Path(E_STR_TEXT("UI"), E_STR_TEXT("Builder"), E_STR_TEXT("Debug"),
                             E_STR_TEXT("Launcher.xml")));
        // widgetを作成
        // レベルが切り替わると自動的にwidgetは破棄される
        Core::Common::Handle hUIWidget = ModuleUI()->NewLayoutByLayotuAsset(this->_hLayoutAsset, 0);

        return TRUE;
    }

    const Bool LevelLauncher::End()
    {
        // TODO: ロードしたアセットを破棄
        ModuleUI()->UnloadAssetWithLayoutBuild(this->_hLayoutAsset);

        const Bool bRet = Level::Node::End();
        E_ASSERT(bRet);

        return TRUE;
    }

}  // namespace Level

#endif
