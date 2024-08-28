#include "InGameZakoEnemyActor.h"

#if 0
#include "actor/ActorCommon.h"
#include "actor/shot/Manager.h"
#include "event/data/EnemyZako.h"
#include "game/GameSystem.h"
#include "scripting/LuaStateManager.h"
#include "system/System.h"
#include "tips/Collsion.h"
#include "tips/Primitive.h"

namespace InGame
{
    //	変数
    static const char* s_aEnemyZakoScriptFileName[C_EnemyActorBase::TYPE_MAX] = {
        "./resource/script/enemy/zako.lua",
        "./resource/script/enemy/zako_knight.lua",
    };

    /*
            @brief	コンストラクタ
    */
    C_EnemyActorZako::C_EnemyActorZako(const C_EnemyActorZako::ABLITY_TYPE in_AblityType)
        : C_EnemyActorBase(in_AblityType)
    {
        _Clear();

        m_size = 16.f;
    }

    /*
     */
    C_EnemyActorZako::~C_EnemyActorZako(void)
    {
    }

    /*
            @brief	初期化
    */
    bool C_EnemyActorZako::init(const int in_Handle)
    {
        C_EnemyActorBase::init(in_Handle);

        //	初回スクリプト実行
        EnemyZakoInitParamEventData eventData(in_Handle);
        LuaStateManager::DoFile(s_aEnemyZakoScriptFileName[m_eAblityType], "Init", &eventData);

        return true;
    }

    /*
            @brief	更新
    */
    bool C_EnemyActorZako::update(void)
    {
        C_EnemyActorBase::update();

        //	画面外かどうかチェック
        {
            INTERSECT_RECT2D_DATA_ST rect(m_pos.x, m_pos.y, m_size * 2.f, m_size * 2.f);
            if (IsScreenOutScreen2D(rect) == true)
            {
                //	画面外なら消滅する。
                VDie();
            }
        }

        return true;
    }

    /*
            @brief	描画
    */
    void C_EnemyActorZako::draw(void)
    {
        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(255, 255, 255, 0));
    }

    //	データ通知
    void C_EnemyActorZako::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
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
    void C_EnemyActorZako::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
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
    void C_EnemyActorZako::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
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
        if (strncmp(pEventTypeName, "RequestEnemyZakoInitParam", strlen(pEventTypeName)) == 0)
        {
            //	Luaからコードデータへ変更
            EnemyZakoInitParamEventData eventData(VGetHandle());
            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            if (GetTypeNameToIndex(eventData.m_pTypeName) != m_eAblityType)
            {
                return;
            }

            //	ステートを決める
            _SetState(eventData.m_StateName);

            m_Hp           = eventData.m_Hp;
            m_Speed        = eventData.m_Speed;
            m_NextStateCnt = eventData.m_NextStateCnt;
            if (m_pos.x == -1.f)
            {
                m_pos.x = eventData.m_pos.x;
            }
            if (m_pos.y == -1.f)
            {
                m_pos.y = eventData.m_pos.y;
            }

            m_DirVec.x = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
            m_DirVec.y = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
            m_Point    = eventData.m_Point;
        }
        else if (strncmp(pEventTypeName, "RequestEnemyZakoAtkAct", strlen(pEventTypeName)) == 0)
        {
        }
        else if (strncmp(pEventTypeName, "RequestEnemyZakoNormalAct", strlen(pEventTypeName)) == 0)
        {
        }
        else if (strncmp(pEventTypeName, "RequestEnemyZakoEscapeAct", strlen(pEventTypeName)) == 0)
        {
            EnemyZakoEscapeParamEventData eventData(VGetHandle());
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

            _SetState(_ESCAPE_ACT);

            m_Speed    = eventData.m_Speed;
            m_DirVec.x = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
            m_DirVec.y = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
        }
    }

    /*
            @brief	変数クリア
    */
    void C_EnemyActorZako::_Clear()
    {
        m_Hp           = 0;
        m_NextStateCnt = 0;
        m_Speed        = 0.f;

        m_pos = Vec3(-1.f, -1.f, 0.f);
    }

    /*
            @brief	アクション通常
    */
    void C_EnemyActorZako::_VNormalAct()
    {
        m_Vec = m_DirVec;
        m_Vec.Scale(m_Speed);
        m_pos += m_Vec;

        if (m_NextStateCnt > 0)
        {
            if ((--m_NextStateCnt) <= 0.f)
            {
                //	次のステートをスクリプト側で決めてもらう
                //	パラメータを作成
                EnemyZakoNormalParamEventData eventData(VGetHandle());
                eventData.m_Angle = atan2(m_DirVec.y, m_DirVec.x) * RADIAN_TO_ANGLE;
                eventData.m_Hp    = m_Hp;
                eventData.m_Speed = m_Speed;
                LuaStateManager::DoFile(s_aEnemyZakoScriptFileName[m_eAblityType], "EndNormalAct",
                                        &eventData);
            }
        }
    }

    /*
            @brief
            @note	逃げる
    */
    void C_EnemyActorZako::_VEscapeAct()
    {
        Vec3 vec = m_DirVec;
        vec *= m_Speed;
        m_pos.Add(vec);
    }

    /*
            @brief	死亡
    */
    void C_EnemyActorZako::_VDieAct()
    {
        SystemProprtyInterfaceInGame::AddPoint(m_Point);
    }
}  // namespace InGame
#endif
