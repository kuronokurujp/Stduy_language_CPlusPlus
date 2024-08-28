#include "InGameSplitEnemyActor.h"

#if 0
#include "Zako.h"
#include "actor/ActorCommon.h"
#include "actor/shot/Manager.h"
#include "event/data/EnemySplit.h"
#include "game/GameSystem.h"
#include "scripting/LuaStateManager.h"
#include "system/System.h"
#include "tips/Collsion.h"
#include "tips/Primitive.h"

namespace InGame
{
    //	変数
    static const char* s_aEnemySplitScriptFileName[C_EnemyActorBase::TYPE_MAX] = {
        "./resource/script/enemy/split_pawn.lua",
        "./resource/script/enemy/split_knight.lua",
    };

    /*
            @brief	コンストラクタ
    */
    EnemyActorSplit::EnemyActorSplit() : C_EnemyActorBase(TYPE_PAWN)
    {
        _Clear();

        m_size = 64.f;
    }

    /*
            @brief	初期化
    */
    bool EnemyActorSplit::init(const int in_Handle)
    {
        C_EnemyActorBase::init(in_Handle);

        //	初回スクリプト実行
        EnemySplitInitParamEventData eventData(in_Handle);
        LuaStateManager::DoFile(s_aEnemySplitScriptFileName[m_eAblityType], "Init", &eventData);

        return true;
    }

    /*
            @brief	更新
    */
    bool EnemyActorSplit::update(void)
    {
        C_EnemyActorBase::update();

        return true;
    }

    /*
            @brief	描画
    */
    void EnemyActorSplit::draw(void)
    {
        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(255, 255, 0, 0));
    }

    //	データ通知
    void EnemyActorSplit::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
        switch (in_DataType)
        {
            case eACTOR_SEND_MESS_HIT_SHOT:
            {
                HIT_SHOT_SEND_DATA_ST* pData = (HIT_SHOT_SEND_DATA_ST*)(in_pData);
                if (pData != NULL)
                {
                    //	自機からの弾の場合はダメージを受ける
                    if (pData->proprety == ShotManagerActor::PLAER_PROPERTY)
                    {
                        printf("弾があたった～\n");

                        m_Hp -= 1;
                        if (m_Hp <= 0)
                        {
                            //	死亡
                            VDie();
                        }
                        else
                        {
                            //	ダメージ演出
                        }

                        // 弾を消す
                        if (pData->pShotEmit != NULL)
                        {
                            pData->pShotEmit->Delete(pData->bulletIndex);
                        }
                    }
                }

                break;
            }
        }
    }

    /*
            @brief	他アクターチェック
    */
    void EnemyActorSplit::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
    {
        //	RTTI判定衝突用のアクターかチェック
        if (in_pOtherActor->VGetRttiId() != C_ColisionActor::RTTI_ID)
        {
            return;
        }

        C_ColisionActor* pCollisionActor = NULL;
        pCollisionActor                  = (C_ColisionActor*)in_pOtherActor;

        //	衝突判定
        INTERSECT_RECT2D_DATA_ST chr01(pCollisionActor->GetPos().x, pCollisionActor->GetPos().y,
                                       pCollisionActor->GetSize(), pCollisionActor->GetSize());
        INTERSECT_RECT2D_DATA_ST chr02(GetPos().x, GetPos().y, GetSize(), GetSize());
        if (isIntersectedRect2D(chr01, chr02) == true)
        {
            //	接触した
            in_pOtherActor->VOnCallbackSendMessage(eACTOR_SEND_MESS_HIT_ENEMY, NULL);
        }
    }

    /*
            @brief	スクリプトで通知したデータを受け取る
    */
    void EnemyActorSplit::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
    {
        //	スクリプトからデータを復元
        if (in_pLuaState == NULL)
        {
            return;
        }

        //	データタイプを取得
        const char* pEventTypeName = lua_tostring(in_pLuaState, 1);
        if (pEventTypeName == NULL)
        {
            return;
        }

        //	初期パラメータ
        if (strncmp(pEventTypeName, "RequestEnemySplitInitParam", strlen(pEventTypeName)) == 0)
        {
            //	Luaからコードデータへ変更
            EnemySplitInitParamEventData eventData(VGetHandle());

            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            if (GetTypeNameToIndex(eventData.m_pTypeName) != m_eAblityType)
            {
                return;
            }

            //	スタックを戻す
            lua_pop(in_pLuaState, lua_gettop(in_pLuaState) + 1);

            //	ステートを決める
            _SetState(eventData.m_StateName);

            m_Hp           = eventData.m_Hp;
            m_Speed        = eventData.m_Speed;
            m_NextStateCnt = eventData.m_NextStateCnt;
            m_pos          = eventData.m_pos;
            m_DirVec.x     = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
            m_DirVec.y     = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
            m_ZakoNum      = eventData.m_ZakoNum;
            m_Point        = eventData.m_Point;
        }
        else if (strncmp(pEventTypeName, "RequestEnemySplitAtkAct", strlen(pEventTypeName)) == 0)
        {
        }
        else if (strncmp(pEventTypeName, "RequestEnemySplitDieAct", strlen(pEventTypeName)) == 0)
        {
            //	死亡
            VDie();
        }
    }

    /*
            @brief	変数クリア
    */
    void EnemyActorSplit::_Clear()
    {
        m_pZakoList = NULL;
        m_ZakoNum   = 0;
        m_Speed     = 0.f;
    }

    /*
            @brief	アクション通常
    */
    void EnemyActorSplit::_VNormalAct()
    {
        Vec3 vec = m_DirVec;
        vec.Scale(m_Speed);
        m_pos += vec;

        if (m_NextStateCnt > 0)
        {
            if ((--m_NextStateCnt) <= 0.f)
            {
                //	次のステートをスクリプト側で決めてもらう
                EnemySplitNormalParamEventData evetDat(VGetHandle());
                LuaStateManager::DoFile(s_aEnemySplitScriptFileName[m_eAblityType], "EndNormalAct",
                                        &evetDat);
            }
        }
    }

    /*
            @brief	死亡時の処理
    */
    void EnemyActorSplit::_VDieAct()
    {
        if (m_Hp <= 0)
        {
            SystemProprtyInterfaceInGame::AddPoint(m_Point);
        }

        C_ColisionActorManager& inst = C_ColisionActorManager::inst();

        //	敵を分裂させる
        float addPosY    = 0.f;
        unsigned zakoNum = m_ZakoNum;
        for (unsigned i = 0; i < zakoNum; ++i)
        {
            C_EnemyActorZako* pEnemy = new C_EnemyActorZako(TYPE_PAWN);

            Vec3 pos = m_pos;

            addPosY += 10.f;
            if (i % 2)
            {
                pos.y += addPosY;
            }
            else
            {
                pos.y -= addPosY;
            }

            pEnemy->SetPos(pos);
            inst.add(pEnemy);
        }
    }

}  // namespace InGame
#endif
