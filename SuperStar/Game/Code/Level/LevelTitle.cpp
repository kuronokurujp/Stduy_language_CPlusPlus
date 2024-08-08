#include "LevelTitle.h"

// UIモジュール
#include "UIModule.h"

namespace Level
{
    //	変数
    static const char* s_pTitleSeqTitleNameText = "STG";
    static const char* s_pTitleSeqKeyGuideText  = " Space : GAME START a : GAME END";

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

        // TODO: ユーザー入力割り当て設定
        /*
        {
            struct InputData
            {
                InputData(Core::Common::ArrayBase<Platform::EKeyboard>& in_arKeys,
                          Core::Common::ArrayBase<Platform::EInputMouseType>& in_arTouchs)
                {
                    this->aKeys.DeepCopy(in_arKeys);
                    this->aTouchs.DeepCopy(in_arTouchs);
                }

                Core::Common::CustomArray<Platform::EKeyboard, 4> aKeys;
                Core::Common::CustomArray<Platform::EInputMouseType, 4> aTouchs;
                // Core::Common::CustomArray<Uint32, 4> aButtons;
                // Core::Common::CustomArray<Uint32, 4> aSticks;
            };

            Core::Common::CustomFixMap<Core::Common::FixString128, InputData, 64> mInputData;
            Core::Common::CustomArray<Platform::EKeyboard, 4>
        aKeys({Platform::EKeyboard::EKeyboard_A});
            Core::Common::CustomArray<Platform::EInputMouseType, 4>
        aTouchs({Platform::EInputMouseType::EInputMouseType_All});

            mInputData.Add(HE_STR_TEXT("test"), InputData(aKeys, aTouchs));

            //            {"A": {"Key": ["A", "B"]}, {"Button": [0, 0]}, {"Stick": [0, 0]}},
            //           {"B": {"Key": ["A", "B"]}, {"Button": [0, 0]}, {"Stick": [0, 0]}},
        }
        */

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

        // this->_layoutAssetHandle = ModuleUI()->LoadAssetWithLayoutBuild(
        this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
            Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Game"),
                             HE_STR_TEXT("Title.xml")));

        // widgetを作成
        // レベルが切り替わると自動的にwidgetは破棄される
        // this->_uIWidgetHandle = ModuleUI()->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0);
        this->_uIWidgetHandle = pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0);

        return bRet;
    }

    const Bool LevelTitle::End()
    {
        auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();
        // ロードしたアセットを破棄
        // ModuleUI()->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return bRet;
    }

}  // namespace Level
