﻿#include "InGamePlayerActor.h"

#include "Actor/Component/InputComponent.h"
#include "Actor/Component/TransformComponent.h"
#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameShotComponent.h"
#include "InGame/Component/Shot/InGameShotStrategy.h"

// 依存するモジュール一覧
#include "RenderModule.h"

#if 0

#include "actor/ActorCommon.h"
#include "common/Common.h"
#include "game/GameSystem.h"
#include "shot/HomingBulletEmit.h"
#include "shot/LaserBulletEmit.h"
#include "shot/NormalBulletEmit.h"
#include "shot/WayBulletEmit.h"
#include "system/System.h"
#include "tips/Primitive.h"
#endif

namespace InGame
{
    InGamePlayerActor::InGamePlayerActor() : Actor::Object()
    {
        this->_Clear();
    }

    Bool InGamePlayerActor::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        // 座標関連のコンポーネント追加
        {
            this->_transformHandle = this->AddComponent<Actor::TransformComponent>(1);
            HE_ASSERT((this->_transformHandle.Null() == FALSE) &&
                      "トランスフォームコンポーネントの追加失敗");

            // 事前に設定していた座標をトランスフォームコンポーネントに設定
            this->SetPos(this->_pos);
        }

        // 当たり判定コンポーネント追加
        {
            auto [handle, pComponent] =
                this->AddComponentByHandleAndComp<InGameCircleCollision2DComponent>(0);
            HE_ASSERT(handle.Null() == FALSE);

            pComponent->SetRadius(HE_MIN(this->_size._fX, this->_size._fY));
            pComponent->SetCollisionHashCode(HE_STR_TEXT("Player"));
            //  TODO: 当たった時の処理を追加
            pComponent->SetHitAction([](const CollisionData& in_rSelf, const CollisionData& in_rHit)
                                     { HE_LOG_LINE(HE_STR_TEXT("Hit")); });

            this->_collisoinHandle = handle;
        }

        // 弾を打つ機能一覧作成
        // TODO: 各機能を実装する
        {
            this->_mShotStrategy.Add(HE_STR_TEXT("Normal"),
                                     HE_MAKE_CUSTOM_SHARED_PTR(InGameShotStrategyNormalBullet));
            this->_mShotStrategy.Add(HE_STR_TEXT("Laser"),
                                     HE_MAKE_CUSTOM_SHARED_PTR(InGameShotStrategyNormalBullet));
            this->_mShotStrategy.Add(HE_STR_TEXT("Homing"),
                                     HE_MAKE_CUSTOM_SHARED_PTR(InGameShotStrategyNormalBullet));
            this->_mShotStrategy.Add(HE_STR_TEXT("4Way"),
                                     HE_MAKE_CUSTOM_SHARED_PTR(InGameShotStrategyNormalBullet));
        }

        // 弾を打つコンポーネントを追加
        {
            this->_shotHandle = this->AddComponent<InGameShotComponent>(2);
            HE_ASSERT(this->_shotHandle.Null() == FALSE);

            auto pShotComponent = this->GetComponent<InGameShotComponent>(this->_shotHandle);
            pShotComponent->SetStrategy(this->_mShotStrategy[HE_STR_TEXT("Normal")]);
        }

#if 0
        //	自機が使用する弾を弾管理に追加する。
        {
            I_InterfaceBulletEmit* pShotList[eSHOT_MAX] = {NULL};

            {
                pShotList[eSHOT_LASER]  = new C_LaserBulletEmit(1);
                pShotList[eSHOT_HOMING] = new C_HomingBulletEmit(8);
                pShotList[eSHOT_WAY]    = new C_WayBulletEmit(4, 12);
            }

            C_ColisionActorManager& inst = C_ColisionActorManager::inst();

            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                SystemProprtyInterfaceInGame::GetActorHandleData();

            ShotManagerActor* pShotManagerActor =
                (ShotManagerActor*)inst.GetData(rHandle.shotManager);
            if (pShotManagerActor != NULL)
            {
                for (int i = 0; i < eSHOT_MAX; ++i)
                {
                    //	弾を登録する。
                    m_aShotHandle[i] =
                        pShotManagerActor->Add(pShotList[i], ShotManagerActor::PLAER_PROPERTY);
                }
            }
        }
#endif

        return TRUE;
    }

    Bool InGamePlayerActor::VEnd()
    {
        // 弾の機能解放
        for (auto itr = this->_mShotStrategy.Begin(); itr != this->_mShotStrategy.End(); ++itr)
        {
            itr->data.reset();
        }

        return Actor::Object::VEnd();
    }

    void InGamePlayerActor::VUpdate(const Float32 in_fDt)
    {
        Actor::Object::VUpdate(in_fDt);

        // 移動情報による座標更新
        {
            Core::Math::Vector2 newPos;
            newPos.SetAdd(this->_pos, this->_move);
            this->_move.Zero();

            this->SetPos(newPos);
        }

        // 描画座標取得
        Core::Math::Rect2 rect;
        {
            auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
            HE_ASSERT(pTrans);

            Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_size._fX, this->_size._fY,
                                      Core::Math::Rect2::EAnchor_Center);
            pTrans->TransformLocalToWorldRect2D(&rect, srcRect);
        }

        // 描画コマンド追加
        // TODO: 画像表示に切り替えるかも
        Render::Command2DRectDraw(this->_viewHandle, rect, Render::RGB::White);
    }

    void InGamePlayerActor::SetPos(const Core::Math::Vector2& in_rPos)
    {
        this->_pos = in_rPos;

        if (this->_transformHandle.Null()) return;

        // 事前に設定していた座標をトランスフォームコンポーネントに設定
        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        pTrans->SetPos(Core::Math::Vector3(this->_pos));
    }

    void InGamePlayerActor::SetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    void InGamePlayerActor::SetViewHandle(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        this->_viewHandle = in_rHandle;
    }

    void InGamePlayerActor::Move(const Core::Math::Vector2& in_rMove)
    {
        this->_move.Zero();
        this->_move.Madd(in_rMove, this->_parameter.speed);
    }

    void InGamePlayerActor::Shot()
    {
        HE_ASSERT(this->_transformHandle.Null() == FALSE);
        HE_ASSERT(this->_shotHandle.Null() == FALSE);

        auto pShotComponent = this->GetComponent<InGameShotComponent>(this->_shotHandle);
        HE_ASSERT(pShotComponent);

        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        auto pCollision =
            this->GetComponent<InGameCircleCollision2DComponent>(this->_collisoinHandle);
        HE_ASSERT(pCollision);

        auto pos = pTrans->GetWorldPos();

        // 弾を打つ
        // 自身が打った弾は接触しないようにコリジョンハッシュコードを設定
        pShotComponent->Shot(Core::Math::Vector2(pos._fX, pos._fY), Core::Math::Vector2(1.0f, 0.0f),
                             pCollision->HashCode());
#if 0
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();

        SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& handle =
            SystemProprtyInterfaceInGame::GetActorHandleData();

        ShotManagerActor* pShotManagerActor = (ShotManagerActor*)inst.GetData(handle.shotManager);
        if (pShotManagerActor != NULL)
        {
            ShotManagerActor::DATA_ST* pShotData =
                pShotManagerActor->GetData(m_aShotHandle[m_shotType]);
            if (pShotData != NULL)
            {
                if (m_shotType == eSHOT_HOMING)
                {
                    // 誘導段の場合はどの座標に飛ぶか指定
                    C_HomingBulletEmit* pHomingBullteEmit = (C_HomingBulletEmit*)pShotData->pShot;

                    int homingBulletMax = pHomingBullteEmit->GetBulletMax();

                    int bulletIdx = 0;
                    //	敵を探す
                    for (int i = 0; i < handle.enemyMax; ++i)
                    {
                        C_PaketSendActor* pEnemyActor =
                            (C_PaketSendActor*)inst.GetData(handle.aEnemy[i]);
                        if ((pEnemyActor != NULL) &&
                            (pEnemyActor->VGetRttiId() == C_ColisionActor::RTTI_ID))
                        {
                            C_ColisionActor* pEnemyCollisionActor = (C_ColisionActor*)pEnemyActor;
                            D3DXVECTOR3 pos                       = pEnemyCollisionActor->GetPos();

                            pHomingBullteEmit->setTargetPos(bulletIdx, pos);
                            ++bulletIdx;

                            if (bulletIdx >= homingBulletMax)
                            {
                                break;
                            }
                        }
                    }
                }

                pShotData->pShot->shot(m_pos);
            }
        }
#endif
    }

#if 0
    /*
            @brief	現在の操作しているショット名を取得
            @return	ショット名
    */
    const char* C_PlayerActor::GetActiveShotName() const
    {
        return s_apPlayerShotName[m_shotType];
    }

    /*
            @brief	撃てる弾の名前を取得
            @return	ショット名を取得
    */
    const char* C_PlayerActor::GetShotName(const int in_Type) const
    {
        ASSERT(in_Type < eSHOT_MAX);
        return s_apPlayerShotName[in_Type];
    }



    /*
            @brief	弾の種類を返す
            @param	in_MoveNum	弾種類変更する方向を決める
    */
    void C_PlayerActor::MoveShotType(const int in_MoveNum)
    {
        if (in_MoveNum > 0)
        {
            m_shotType = (++m_shotType) % eSHOT_MAX;
        }
        else if (in_MoveNum < 0)
        {
            m_shotType = (--m_shotType) < 0 ? eSHOT_MAX - 1 : m_shotType;
        }
    }

    /*
            @brief	データ通知<BR>
            アクターシステム側からのデータ通知<BR>
    */
    void C_PlayerActor::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
        switch (in_DataType)
        {
            case eACTOR_SEND_MESS_HIT_ENEMY:
            {
                //	敵と接触した
                //	死亡。
                if (m_InvincibleCnt <= 0)
                {
                    printf("衝突\n");

                    --m_life;
                    m_InvincibleCnt = s_PlayerInvCnt;
                    if (m_life <= 0)
                    {
                        VDie();
                    }
                }

                break;
            }
        }
    }
#endif
}  // namespace InGame
