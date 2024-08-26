﻿#include "Engine/MiniEngine.h"

// デバッグ用なのでリリース版には含めない
#ifdef HE_ENGINE_DEBUG

#include "Engine/Common/CustomMap.h"
#include "Engine/Math/Math.h"
#include "LevelLauncher.h"
#include "LevelTitle.h"

// モジュール
#include "RenderModule.h"
#include "UIModule.h"

// デバッグ画面を表示してレベルを切り替える
namespace Level
{
    const Bool LevelLauncher::Begin()
    {
        const Bool bRet = Level::Node::Begin();
        HE_ASSERT(bRet);

        // ランチャー用のレンダリングビュー作成
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

        // UIイベントをキャッチするコンポーネントを追加
        {
            auto h    = this->AddComponent<Level::LevelUserInputReceiveComponent>(0);
            auto comp = this->GetComponent<Level::LevelUserInputReceiveComponent>(h);

            auto handler = Core::Memory::MakeCustomUniquePtr<Level::LevelUserInputMessage>(
                [this](const Char* in_pMsg)
                {
                    HE_LOG_LINE(in_pMsg);
                    // 次のレベルへ遷移
                    if (HE_STR_CMP(in_pMsg, HE_STR_TEXT("C_TitleSeq")) == 0)
                    {
                        auto pLevelModule = Module::ModuleManager::I().Get<Level::LevelModule>();
                        // タイトルへ遷移する
                        pLevelModule->GetManager()->StartLevel<Level::LevelTitle>();
                    }
                });
            comp->SetReceiver(std::move(handler));
        }

        // UIのBuilderファイルからレイアウト作成
        {
            auto pUIModule           = Module::ModuleManager::I().Get<UI::UIModule>();
            this->_layoutAssetHandle = pUIModule->LoadAssetWithLayoutBuild(
                Core::File::Path(HE_STR_TEXT("UI"), HE_STR_TEXT("Builder"), HE_STR_TEXT("Debug"),
                                 HE_STR_TEXT("Launcher.xml")));
            // widgetを作成
            // レベルが切り替わると自動的にwidgetは破棄される
            this->_uIWidgetHandle =
                pUIModule->NewLayoutByLayotuAsset(this->_layoutAssetHandle, 0, this->_viewHandle);
        }

        return bRet;
    }

    const Bool LevelLauncher::End()
    {
        // ビューのハンドルを外す
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);
        }

        // ロードしたアセットを破棄
        {
            auto pUIModule = Module::ModuleManager::I().Get<UI::UIModule>();
            if (pUIModule != NULL) pUIModule->UnloadAssetWithLayoutBuild(this->_layoutAssetHandle);
        }

        const Bool bRet = Level::Node::End();
        HE_ASSERT(bRet);

        return TRUE;
    }

}  // namespace Level

#endif
