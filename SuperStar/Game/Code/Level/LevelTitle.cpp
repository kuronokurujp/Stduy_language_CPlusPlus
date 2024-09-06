#include "LevelTitle.h"

#include "LevelInGame.h"
#include "UIModule.h"

namespace Level
{
    namespace Local
    {
        static const Char* szInputActionNameByGameStart = HE_STR_TEXT("TitleLevel_GameStart");
        static const Char* szInputActionNameByGameEnd   = HE_STR_TEXT("TitleLevel_GameEnd");

        static const EnhancedInput::ActionMap mInputAction =
            {{szInputActionNameByGameStart,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_SPACE})},
             {szInputActionNameByGameEnd,
              EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_A})}};
    };  // namespace Local

    const Bool LevelTitle::VBegin()
    {
        const Bool bRet = Level::Node::VBegin();
        HE_ASSERT(bRet);

        // レンダリングビュー作成
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

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
        {
            auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();

            this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Game"),
                                 HE_STR_TEXT("Title.xml")));

            // widgetを作成
            // レベルが切り替わると自動的にwidgetは破棄される
            pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0, this->_viewHandle,
                                              this->GetHandle());
        }

        return bRet;
    }

    const Bool LevelTitle::VEnd()
    {
        // ビューのハンドルを外す
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);
        }

        // 専用の入力アクションを外す
        {
            auto pInputModule =
                Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
            if (pInputModule != NULL) pInputModule->RemoveCommonMappingAction(Local::mInputAction);
        }

        // ロードしたアセットを破棄
        {
            auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();
            if (pUIModule != NULL) pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        }

        const Bool bRet = Level::Node::VEnd();
        HE_ASSERT(bRet);

        return bRet;
    }

    void LevelTitle::_VProcessInput(const EnhancedInput::InputMap* in_pInputMap)
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
