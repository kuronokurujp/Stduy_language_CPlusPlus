#include "LevelTitle.h"

// モジュール
#include "UIModule.h"

namespace Level
{
    LevelTitle::LevelTitle() : Node()
    {
    }

    LevelTitle::~LevelTitle()
    {
    }

    const Bool LevelTitle::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        HE_ASSERT(bRet);

        // UIイベントをキャッチするコンポーネントを追加
        {
            auto h    = this->AddComponent<Level::LevelUserInputReceiveComponent>(0);
            auto comp = this->GetComponent<Level::LevelUserInputReceiveComponent>(h);

            auto handler = Core::Memory::MakeCustomUniquePtr<Level::LevelUserInputMessage>(
                [this](const Char* in_pMsg) { HE_LOG_LINE(in_pMsg); });
            comp->SetReceiver(std::move(handler));
        }

        // UIのBuilderファイルからレイアウト作成
        auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();

        this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
            Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Game"),
                             HE_STR_TEXT("Title.xml")));

        // widgetを作成
        // レベルが切り替わると自動的にwidgetは破棄される
        this->_uIWidgetHandle = pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0);

        return bRet;
    }

    const Bool LevelTitle::End()
    {
        auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();
        // ロードしたアセットを破棄
        pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return bRet;
    }

}  // namespace Level
