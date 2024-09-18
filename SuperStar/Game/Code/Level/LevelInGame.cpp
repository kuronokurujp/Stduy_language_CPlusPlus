#include "LevelInGame.h"

#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameShotManagerComponent.h"
#include "InGame/Component/InGameStageManagerComponent.h"
#include "InGame/Component/InGameSystemComponent.h"
#include "LevelInGame/LevelInGame_BG.h"

// 利用モジュール
#include "RenderModule.h"

namespace Level
{
    namespace Local
    {
        class EventManagerStrategy final : public Event::EventManagerStrategyInterface
        {
        public:
            Bool VIsEventTypeStr(const Event::EventTypeStr& in_rTypeStr) { return FALSE; }
        };

        class EventListener final : public Event::EventListenerInterface
        {
            HE_CLASS_COPY_NG(EventListener);
            HE_CLASS_MOVE_NG(EventListener);

        public:
            EventListener() = default;
            EventListener(LevelInGame* in_pLevelInGame) { this->_pLevel = in_pLevelInGame; }

            // リスナー名をテキストで返す
            const Char* VName() { return HE_STR_TEXT("LevelInGame"); }

            /// <summary>
            /// リスナーがイベント受け取ったかどうか
            /// </summary>
            Bool VHandleEvent(Event::EventDataInterfacePtr const&) { return TRUE; }

        private:
            LevelInGame* _pLevel = NULL;
        };
    }  // namespace Local

    LevelInGame::LevelInGame() : Level::Node()
    {
    }

    Bool LevelInGame::VBegin()
    {
        const Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        // TODO: インゲーム内で利用するイベント管理を追加
        {
            auto pEventModule = Module::ModuleManager::I().Get<Event::EventModule>();

            auto upStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR(Local::EventManagerStrategy);
            pEventModule->AddEventManager(std::move(upStrategy));

            pEventModule->AddListenr(HE_MAKE_CUSTOM_SHARED_PTR(Local::EventListener, this),
                                     HE_STR_TEXT("test"));
        }

        // 背景のレベル追加
        this->AddLevel<LevelInGame_BG>();

        // レンダリングビュー作成
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

        // インゲームのシステムコンポーネントを追加
        {
            this->_systemComponentHandle = this->AddComponent<InGame::InGameSystemComponent>(0);
            auto pSystemComponent =
                this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
            pSystemComponent->SetFlgGameEnd(FALSE);
            pSystemComponent->SetPoint(0);
        }

        // 味方や敵の弾を管理するコンポーネント追加
        {
            this->_shotManagerComponentHandle =
                this->AddComponent<InGame::InGameShotManagerComponent>(0);
        }

        // インゲームのステージコンポーネント追加
        {
            this->_stageManagerComponentHandle =
                this->AddComponent<InGame::InGameStageManagerComponent>(0);
            auto pStageManagerComponent = this->GetComponent<InGame::InGameStageManagerComponent>(
                this->_stageManagerComponentHandle);
            HE_ASSERT(pStageManagerComponent);
            pStageManagerComponent->SetViewHandle(this->_viewHandle);
        }

        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.init();
        */

        return TRUE;
    }

    Bool LevelInGame::VEnd()
    {
        auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
        if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);

        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.final();
        */
        return Node::VEnd();
    }

    void LevelInGame::VUpdate(const Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        auto pSystemComponent =
            this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
        {
            if (pSystemComponent->IsGameEnd())
            {
                // next = new C_TitleSeq();
            }
        }

        // TODO: コリジョン処理をする
        {
            InGame::InGameCollisionComponent::CollisionAll();
        }
    }

}  // namespace Level
