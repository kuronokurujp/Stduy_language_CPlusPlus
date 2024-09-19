#include "LevelInGame.h"

#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameShotManagerComponent.h"
#include "InGame/Component/InGameStageManagerComponent.h"
#include "InGame/Component/InGameSystemComponent.h"
#include "InGame/Event/InGameEvent.h"
#include "LevelInGame/LevelInGame_BG.h"

// 利用モジュール
#include "RenderModule.h"

namespace Level
{
    namespace Local
    {
        class ShotEventManagerStrategy final : public Event::EventManagerStrategyInterface
        {
        public:
            ShotEventManagerStrategy() = default;
            ShotEventManagerStrategy(const Event::EventTypeStr& in_rEventType)
                : _eventTypeHash(in_rEventType.Hash())
            {
            }

            Bool VIsEventTypeHash(const Uint64 in_ulHash)
            {
                return (this->_eventTypeHash == in_ulHash);
            }

        private:
            Uint64 _eventTypeHash = 0;
        };

        class ShotEventListener final : public Event::EventListenerInterface
        {
            HE_CLASS_COPY_NG(ShotEventListener);
            HE_CLASS_MOVE_NG(ShotEventListener);

        public:
            ShotEventListener() = default;
            ShotEventListener(LevelInGame* in_pLevelInGame) { this->_pLevel = in_pLevelInGame; }
            virtual ~ShotEventListener() { this->_pLevel = NULL; }

            // リスナー名
            const Char* VName() { return HE_STR_TEXT("LevelInGameListener"); }

            /// <summary>
            /// リスナーがイベント受け取ったかどうか
            /// </summary>
            Bool VHandleEvent(Event::EventDataInterfacePtr const& in_rEventData)
            {
                // TODO: ゲームのイベント処理を記述
                if (in_rEventData->VDataTypeHash() ==
                    InGame::EventShotNormalBullet::EventType().Hash())
                {
                    // TODO: 通常弾の発射処理
                    HE_LOG_LINE(HE_STR_TEXT("Shot!!"));
                }

                return TRUE;
            }

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

        // TODO: インゲーム内で利用する弾を打つイベント管理を追加
        {
            auto pEventModule = Module::ModuleManager::I().Get<Event::EventModule>();

            auto upStrategy        = HE_MAKE_CUSTOM_UNIQUE_PTR(Local::ShotEventManagerStrategy,
                                                               INGAME_SHOT_EVENT_TYPE_NAME);
            this->_shotEventHandle = pEventModule->AddEventManager(std::move(upStrategy));
            HE_ASSERT(this->_shotEventHandle.Null() == FALSE);

            this->_shotEventListener = HE_MAKE_CUSTOM_SHARED_PTR(Local::ShotEventListener, this);
            if (pEventModule->AddListenr(this->_shotEventListener, INGAME_SHOT_EVENT_TYPE_NAME) ==
                FALSE)
            {
                HE_ASSERT(0 && "イベントリスナー設定に失敗");
            }
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

        if (Module::ModuleManager::Exist())
        {
            auto pEventModule = Module::ModuleManager::I().Get<Event::EventModule>();

            // TODO: 設定したイベントを解放
            pEventModule->RemoveListener(this->_shotEventListener, INGAME_SHOT_EVENT_TYPE_NAME);
            pEventModule->RemoveEventManager(this->_shotEventHandle);
        }

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
