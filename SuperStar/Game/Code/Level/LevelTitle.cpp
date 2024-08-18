#include "LevelTitle.h"

#include "LevelInGame.h"

// モジュール
#include "UIModule.h"

namespace Level
{
    namespace Local
    {
        const Char* szInputActionNameByGameStart = HE_STR_TEXT("TitleLevel_GameStart");
        const Char* szInputActionNameByGameEnd   = HE_STR_TEXT("TitleLevel_GameEnd");

        EnhancedInput::ActionMap mInputAction =
            {{szInputActionNameByGameStart,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_A})},
             {szInputActionNameByGameEnd,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_SPACE})}};
    };  // namespace Local

    const Bool LevelTitle::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        HE_ASSERT(bRet);

        // ユーザー共通入力割り当て設定
        {
            auto pInputModule =
                Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->AddCommonMappingAction(Local::mInputAction);
        }

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
        // 専用の入力アクションを外す
        {
            auto pInputModule =
                Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
            pInputModule->RemoveCommonMappingAction(Local::mInputAction);
        }

        auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();
        // ロードしたアセットを破棄
        pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return bRet;
    }

    void LevelTitle::_ProcessInput(const Float32 in_fDt,
                                   const EnhancedInput::InputMap* in_pInputMap)
    {
        // メインゲーム開始入力があるか
        {
            auto it = in_pInputMap->FindKey(Local::szInputActionNameByGameStart);
            if (it != in_pInputMap->End())
            {
                // メインゲームに遷移
                auto pLevelModule = Module::ModuleManager::I().Get<Level::LevelModule>();
                pLevelModule->GetManager()->StartLevel<Level::LevelInGame>();
                return;
            }
        }

        // ゲーム終了入力があるか
        {
            auto it = in_pInputMap->FindKey(Local::szInputActionNameByGameEnd);
            if (it != in_pInputMap->End())
            {
                // TODO: ゲーム終了
                return;
            }
        }
    }
}  // namespace Level
