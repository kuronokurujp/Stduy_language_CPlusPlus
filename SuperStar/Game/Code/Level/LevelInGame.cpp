#include "LevelInGame.h"

#include "Engine/Common/Hash.h"
#include "Engine/Math/Vector2.h"
#include "InGame/Bullet/InGameBulletNormal.h"
#include "InGame/Component/InGameBulletManagerComponent.h"
#include "InGame/Component/InGameCollisionComponent.h"
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
            ShotEventListener(
                std::function<InGame::InGameBulletManagerComponent*()> in_bulletManagerCompFunc)
            {
                this->_bulletManagerCompFunc = in_bulletManagerCompFunc;
            }

            virtual ~ShotEventListener() { this->_bulletManagerCompFunc = NULL; }

            // リスナー名
            const Char* VName() { return HE_STR_TEXT("LevelInGameListener"); }

            /// <summary>
            /// リスナーがイベント受け取ったかどうか
            /// </summary>
            Bool VHandleEvent(Event::EventDataInterfacePtr const& in_rEventData)
            {
                // ゲームのイベント処理を記述

                // 通常弾を打つ
                if (in_rEventData->VDataTypeHash() ==
                    InGame::EventShotNormalBullet::EventType().Hash())
                {
                    // 通常弾の発射処理
                    // HE_LOG_LINE(HE_STR_TEXT("Normal Shot!!"));

                    InGame::EventShotNormalBullet* pEvent =
                        reinterpret_cast<InGame::EventShotNormalBullet*>(in_rEventData.get());
                    HE_ASSERT(pEvent != NULL);

                    auto pBulletManagerComponent = this->_bulletManagerCompFunc();
                    HE_ASSERT(pBulletManagerComponent);

                    auto upFactor =
                        HE_MAKE_CUSTOM_UNIQUE_PTR(InGame::InGameBulletNormalFactory, pEvent->_pos,
                                                  pEvent->_dir, pEvent->_uCollisionHashCode);

                    pBulletManagerComponent->MakeObject(std::move(upFactor));
                }

                return TRUE;
            }

        private:
            std::function<InGame::InGameBulletManagerComponent*()> _bulletManagerCompFunc;
        };

    }  // namespace Local

    Bool LevelInGame::VBegin()
    {
        const Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        // TODO: アセットのロード

        // 背景のレベル追加
        this->AddLevel<LevelInGame_BG>();

        // インゲームのレンダリングメインビュー作成
        {
            auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
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

        // 味方や敵の弾を管理するオブジェクトとコンポーネント追加
        Core::Common::Handle bulletManagerActorHandle;
        Core::Common::Handle bulletManagerComponentHandle;
        {
            bulletManagerActorHandle = this->AddActor<Actor::Object>();
            auto pActor              = this->GetActor<Actor::Object>(bulletManagerActorHandle);

            auto [handle, pComp] =
                pActor->AddComponentByHandleAndComp<InGame::InGameBulletManagerComponent>(0);
            HE_ASSERT(handle.Null() == FALSE);

            bulletManagerComponentHandle = handle;

            // 弾を描画ハンドルを渡す
            pComp->SetViewHandle(this->_viewHandle);

            pComp->SetHashCode(HE_STR_TEXT("Bullet"));

            // 弾が当たった時のヒットアクション
            pComp->SetHitAction(
                [](const InGame::CollisionData& in_rSelf, const InGame::CollisionData& in_rTargtt)
                { HE_LOG_LINE(HE_STR_TEXT("Hit Bullet")); });

            // 利用する弾のアルゴリズムを登録
            auto upStrategy = HE_MAKE_CUSTOM_UNIQUE_PTR(InGame::InGameBulletNormalStrategy);
            pComp->AddStrategy(std::move(upStrategy));
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

        // インゲーム内で利用するイベントシステムを構築
        {
            auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

            // 弾を打つイベント管理を追加
            {
                auto upStrategy        = HE_MAKE_CUSTOM_UNIQUE_PTR(Local::ShotEventManagerStrategy,
                                                                   INGAME_SHOT_EVENT_TYPE_NAME);
                this->_shotEventHandle = pEventModule->AddEventManager(std::move(upStrategy));
                HE_ASSERT(this->_shotEventHandle.Null() == FALSE);

                this->_shotEventListener = HE_MAKE_CUSTOM_SHARED_PTR(
                    Local::ShotEventListener,
                    // 弾管理コンポーネントを返す関数を登録
                    [this, bulletManagerActorHandle, bulletManagerComponentHandle]()
                    {
                        auto pActor = this->GetActor<Actor::Object>(bulletManagerActorHandle);
                        HE_ASSERT(pActor);

                        return pActor->GetComponent<InGame::InGameBulletManagerComponent>(
                            bulletManagerComponentHandle);
                    });

                if (pEventModule->AddListener(this->_shotEventListener,
                                             INGAME_SHOT_EVENT_TYPE_NAME) == FALSE)
                {
                    HE_ASSERT(0 && "イベントリスナー設定に失敗");
                }
            }
        }

        return TRUE;
    }

    Bool LevelInGame::VEnd()
    {
        auto pRenderModule = HE_ENGINE.ModuleManager().Get<Render::RenderModule>();
        pRenderModule->RemoveView(this->_viewHandle);

        auto pEventModule = HE_ENGINE.ModuleManager().Get<Event::EventModule>();

        // 設定したイベントリスナーを解放
        pEventModule->RemoveAllListener(INGAME_SHOT_EVENT_TYPE_NAME);
        // 作成したイベント管理を解放
        pEventModule->RemoveEventManager(this->_shotEventHandle);

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
                // TODO: ゲームオーバー
            }
        }

        // コリジョン処理をする
        InGame::CollisionAll();
    }

}  // namespace Level
