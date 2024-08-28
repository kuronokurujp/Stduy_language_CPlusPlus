#include "IngameSnakeEnemyActor.h"

#if 0
#include "actor/ActorCommon.h"
#include "actor/shot/Manager.h"
#include "event/data/EnemySnake.h"
#include "game/GameSystem.h"
#include "scripting/LuaStateManager.h"
#include "system/System.h"
#include "tips/Collsion.h"
#include "tips/Primitive.h"

namespace InGame
{
    //	変数
    static const char* s_EnemySnakeScriptFileName = "./resource/script/enemy/snake.lua";

    /*
            @brief	コンストラクタ
    */
    EnemyActorSnake::EnemyActorSnake() : C_EnemyActorBase(TYPE_PAWN)
    {
        _Clear();

        m_size = 16.f;
    }

    /*
            @brief	初期化
    */
    bool EnemyActorSnake::init(const int in_Handle)
    {
        C_EnemyActorBase::init(in_Handle);

        //	初回スクリプト実行
        EnemySnakeInitPaeramEventData eventData(in_Handle);
        LuaStateManager::DoFile(s_EnemySnakeScriptFileName, "Init", &eventData);

        return true;
    }

    /*
            @brief	終了
    */
    bool EnemyActorSnake::final(void)
    {
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();

        int partsNum = m_PartsNum;
        for (int i = 0; i < partsNum; ++i)
        {
            //	メモリ削除も同時にやってくれる
            inst.destroy(m_ppPartsList[i]->m_Handle);
        }

        SAFE_DELETE_ARRAY(m_pOldPosList);
        SAFE_DELETE_ARRAY(m_ppPartsList);

        return true;
    }

    /*
            @brief	更新
    */
    bool EnemyActorSnake::update(void)
    {
        if (m_ppPartsList != NULL && m_pOldPosList != NULL)
        {
            m_ppPartsList[0]->SetPos(m_pOldPosList[m_OldPosIndex]);

            int partsNum = m_PartsNum;
            for (int i = 0; i < (partsNum - 1); ++i)
            {
                _Parts* pParts = m_ppPartsList[i];
                m_ppPartsList[i + 1]->SetPos(pParts->m_pOldPosList[pParts->m_OldPosIndex]);
            }
        }

        C_EnemyActorBase::update();

        if ((m_ppPartsList != NULL) && (m_pOldPosList != NULL))
        {
            m_pOldPosList[m_OldPosIndex] = m_pos;
            m_OldPosIndex                = ++m_OldPosIndex % m_PosIndexNum;

            int partsNum = m_PartsNum;
            for (int i = 0; i < partsNum; ++i)
            {
                _Parts* pParts                               = m_ppPartsList[i];
                pParts->m_pOldPosList[pParts->m_OldPosIndex] = pParts->GetPos();
                pParts->m_OldPosIndex = ++pParts->m_OldPosIndex % pParts->m_PosIndexNum;
            }
        }

        return true;
    }

    /*
            @brief	描画
    */
    void EnemyActorSnake::draw(void)
    {
        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(255, 255, 255, 0));
    }

    //	データ通知
    void EnemyActorSnake::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
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
    void EnemyActorSnake::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
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
    void EnemyActorSnake::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
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
        if (strncmp(pEventTypeName, "RequestEnemySnakeInitParam", strlen(pEventTypeName)) == 0)
        {
            //	Luaからコードデータへ変更
            EnemySnakeInitPaeramEventData EventData(VGetHandle());
            if (EventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            //	ステートを決める
            _SetState(EventData.m_StateName);

            m_Hp           = EventData.m_Hp;
            m_Speed        = EventData.m_Speed;
            m_NextStateCnt = EventData.m_NextStateCn;
            m_pos          = EventData.m_pos;
            m_PosIndexNum  = EventData.m_LinkPosNum;
            m_Point        = EventData.m_Point;

            m_pOldPosList = new Vec3[m_PosIndexNum];
            for (unsigned i = 0; i < m_PosIndexNum; ++i)
            {
                m_pOldPosList[i] = EventData.m_pos;
            }

            int partsNum = m_PartsNum = 3;
            ASSERT(m_ppPartsList == NULL);

            m_ppPartsList = new EnemyActorSnake::_Parts*[partsNum];

            C_ColisionActorManager& inst = C_ColisionActorManager::inst();
            for (int i = 0; i < partsNum; ++i)
            {
                m_ppPartsList[i]           = new EnemyActorSnake::_Parts();
                m_ppPartsList[i]->m_Handle = inst.add(m_ppPartsList[i]);

                m_ppPartsList[i]->SetPos(EventData.m_pos);

                m_ppPartsList[i]->m_PosIndexNum = EventData.m_LinkPosNum;
                m_ppPartsList[i]->m_pOldPosList = new Vec3[EventData.m_LinkPosNum];
                for (unsigned j = 0; j < EventData.m_LinkPosNum; ++j)
                {
                    m_ppPartsList[i]->m_pOldPosList[j] = EventData.m_pos;
                }
            }

            //	各パーツリンク設定
            m_ppPartsList[0]->m_pPrec = this;
            for (int i = 1; i < (partsNum - 1); ++i)
            {
                m_ppPartsList[i + 1]->m_pPrec = m_ppPartsList[i];
            }

            m_Angle    = 180.f;
            m_DirVec.x = cos(m_Angle * ANGLE_TO_RADIAN);
            m_DirVec.y = sin(m_Angle * ANGLE_TO_RADIAN);
        }
        else if (strncmp(pEventTypeName, "RequestEnemySnakeAtkAct", strlen(pEventTypeName)) == 0)
        {
        }
        else if (strncmp(pEventTypeName, "RequestEnemySnakeEscapeAct", strlen(pEventTypeName)) == 0)
        {
            EnemySnakeEscapeParamEventData eventData(VGetHandle());
            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            _SetState(_ESCAPE_ACT);

            m_Speed = eventData.m_Speed;
        }
    }

    /*
            @brief	変数クリア
    */
    void EnemyActorSnake::_Clear()
    {
        m_Speed       = 0.f;
        m_PartsNum    = 0;
        m_OldPosIndex = 0;
        m_ppPartsList = NULL;
        m_pOldPosList = NULL;
        m_PosIndexNum = 0;
        m_Angle       = 0;

        ::memset(&m_DirVec, 0, sizeof(m_DirVec));
    }

    /*
            @brief	アクション通常
    */
    void EnemyActorSnake::_VNormalAct()
    {
        m_Angle += 1.f;
        m_DirVec.x = cos(m_Angle * ANGLE_TO_RADIAN);
        m_DirVec.y = sin(m_Angle * ANGLE_TO_RADIAN);

        m_pos = m_pos.Add(m_DirVec.Scale(m_Speed));
        if (m_NextStateCnt > 0)
        {
            if ((--m_NextStateCnt) <= 0.f)
            {
                //	次のステートをスクリプト側で決めてもらう
                //	パラメータを作成
                EnemySnakeNormalPaeramEventData EventData(VGetHandle());
                LuaStateManager::DoFile(s_EnemySnakeScriptFileName, "EndNormalAct", &EventData);
            }
        }
    }

    /*
            @brief
            @note	逃げる
    */
    void EnemyActorSnake::_VEscapeAct()
    {
        m_pos = m_pos.Add(m_DirVec);
    }

    /*
            @brief	死亡
    */
    void EnemyActorSnake::_VDieAct()
    {
        SystemProprtyInterfaceInGame::AddPoint(m_Point);
    }

    /*
            @brief	初期化
    */
    bool EnemyActorSnake::_Parts::init(const int in_Handle)
    {
        C_ColisionActor::init(in_Handle);

        return true;
    }

    /*
            @brief	初期化
    */
    bool EnemyActorSnake::_Parts::final()
    {
        SAFE_DELETE_ARRAY(m_pOldPosList);

        return true;
    }

    /*
            @brief	更新
    */
    bool EnemyActorSnake::_Parts::update(void)
    {
        return true;
    }

    /*
            @brief	描画
    */
    void EnemyActorSnake::_Parts::draw(void)
    {
        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(0, 255, 255, 0));
    }

    /*
            @brief	データ通知
    */
    void EnemyActorSnake::_Parts::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
    }

    /*
            @brief
    */
    void EnemyActorSnake::_Parts::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
    {
    }

    /*
            @brief	スクリプトで通知したデータを受け取る
    */
    void EnemyActorSnake::_Parts::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
    {
    }

    /*
            @brief
    */
    void EnemyActorSnake::_Parts::_Clear()
    {
        m_Handle      = -1;
        m_OldPosIndex = 0;
        m_pPrec       = NULL;
        m_pOldPosList = NULL;
        m_PosIndexNum = 0;
    }
}  // namespace InGame
#endif
