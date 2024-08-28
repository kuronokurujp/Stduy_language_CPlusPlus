#include "InGamePlayerActor.h"

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

namespace InGame
{
    //	変数
    static const int s_PlayerInvCnt         = 30;
    static const float s_PlayerMoveSpeed    = 1.f;
    static const char* s_apPlayerShotName[] = {
        "Normal",
        "Laser",
        "Homing",
        "4Way",
    };

    /*
            @brief	コンストラクタ
    */
    C_PlayerActor::C_PlayerActor()
        : C_ColisionActor(Vec3(100.f, 100.f, 0.0f), 16.f),
          m_shotType(eSHOT_BASIC),
          m_life(4),
          m_InvincibleCnt(0)
    {
        //	自機が使用する弾を弾管理に追加する。
        {
            I_InterfaceBulletEmit* pShotList[eSHOT_MAX] = {NULL};

            {
                pShotList[eSHOT_BASIC]  = new C_NormalBulletEmit(20);
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
    }

    C_PlayerActor::~C_PlayerActor(void)
    {
    }

    /*
            @brief	更新
            @return	更新成功 : true / 更新失敗 : false
    */
    bool C_PlayerActor::update(void)
    {
        GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

        //	パッド制御
        {
            //	上
            if (r.isButtonOn(Input::KEY_UP))
            {
                m_pos.y -= s_PlayerMoveSpeed;
            }

            //	下
            if (r.isButtonOn(Input::KEY_DOWN))
            {
                m_pos.y += s_PlayerMoveSpeed;
            }

            //	右
            if (r.isButtonOn(Input::KEY_RIGHT))
            {
                m_pos.x += s_PlayerMoveSpeed;
            }

            //	左
            if (r.isButtonOn(Input::KEY_LEFT))
            {
                m_pos.x -= s_PlayerMoveSpeed;
            }
        }

        if (m_InvincibleCnt > 0)
        {
            --m_InvincibleCnt;
        }

        return true;
    }

    /*
            @brief	描画
    */
    void C_PlayerActor::draw(void)
    {
        if ((m_InvincibleCnt % 2) == 0)
        {
            DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                      D3DCOLOR_ARGB(255, 255, 255, 255));
        }
    }

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
            @brief	弾を撃つ
    */
    void C_PlayerActor::Shot()
    {
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

                    const int homingBulletMax = pHomingBullteEmit->GetBulletMax();

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
}  // namespace InGame
#endif
