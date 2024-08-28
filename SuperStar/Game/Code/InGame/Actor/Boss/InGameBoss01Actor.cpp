#include "InGameBoss01Actor.h"

// ステージボス1

#if 0
#include "actor/ActorCommon.h"
#include "actor/shot/Manager.h"
#include "common/Man.h"
#include "event/data/BossStage1.h"
#include "scripting/LuaStateManager.h"
#include "tips/Collsion.h"
#include "tips/Primitive.h"

namespace InGame
{
    //	変数
    static float g_BossStage1PartsOffsetDistance  = 20.f;
    static float g_BossStage1PartsMoveAngle       = 4.f;
    static int g_BossStage1PartsNum               = 10;
    static const char* s_BossStage1ScriptFileName = "./resource/script/enemy/boss1.lua";

    /*
            @brief	初期化
    */
    bool BossStage1::init(const int in_Handle)
    {
        C_ColisionActor::init(in_Handle);

        SetSize(32.f);

        //	ボスのパーツ作成
        m_PartsDistance = (float)m_size + g_BossStage1PartsOffsetDistance;

        int partsNum = m_PartsNum = g_BossStage1PartsNum;
        m_pPartsHandleList        = new int[partsNum];

        //	初回スクリプト実行
        BossStage1InitParamEventData eventData(in_Handle);
        LuaStateManager::DoFile(s_BossStage1ScriptFileName, "Init", &eventData);

        return true;
    }

    /*
            @brief	終了
    */
    bool BossStage1::final(void)
    {
        int partsNum = m_PartsNum;
        for (int i = 0; i < partsNum; ++i)
        {
            C_ColisionActorManager::inst().destroy(m_pPartsHandleList[i]);
        }

        SAFE_DELETE_ARRAY(m_pPartsHandleList);

        return true;
    }

    /*
            @brief	描画
    */
    void BossStage1::draw(void)
    {
        C_ColisionActorManager& rActorMan = C_ColisionActorManager::inst();
        int max                           = m_PartsNum;
        for (int i = 0; i < max; ++i)
        {
            //	パーツ同士の線を出す。
            _Parts* pParts = (_Parts*)rActorMan.GetData(m_pPartsHandleList[i]);
            if (pParts == NULL)
            {
                continue;
            }

            const Vec3& pos = pParts->GetPos();
            DrawPrimitive2DSimpleLine(Vec2(pos.x, pos.y), Vec2(m_pos.x, m_pos.y),
                                      D3DCOLOR_ARGB(0, 255, 255, 255));
        }

        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(0, 255, 255, 0));
    }

    /*
            @brief	データ通知
    */
    void BossStage1::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
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
            @brief	登録した自分以外のアクターを取得する。
    */
    void BossStage1::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
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
    void BossStage1::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
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
        if (strncmp(pEventTypeName, "RequestBossStage1InitParam", strlen(pEventTypeName)) == 0)
        {
            //	Luaからコードデータへ変更
            BossStage1InitParamEventData eventData(VGetHandle());
            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
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

            if (eventData.m_Angle == -1.f)
            {
                ::memset(&m_DirVec, 0, sizeof(m_DirVec));
            }
            else
            {
                m_DirVec.x = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
                m_DirVec.y = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
            }

            C_ColisionActorManager& rActorMan = C_ColisionActorManager::inst();

            int partsNum         = m_PartsNum;
            float initPartsAngle = 0.f;
            float addAngle       = 360.f / (float)partsNum;
            for (int i = 0; i < partsNum; ++i, initPartsAngle += addAngle)
            {
                _Parts* pParts        = new _Parts(initPartsAngle);
                m_pPartsHandleList[i] = rActorMan.add(pParts);

                pParts->SetPos(
                    Vec3(m_pos.x + cos(initPartsAngle * ANGLE_TO_RADIAN) * m_PartsDistance,
                         m_pos.y + sin(initPartsAngle * ANGLE_TO_RADIAN) * m_PartsDistance,
                         m_pos.z));

                pParts->m_Hp    = eventData.m_PartsHp;
                pParts->m_Point = eventData.m_PartsPoint;
            }

            SetPos(eventData.m_pos);

            m_Point = eventData.m_Point;
        }
        else if (strncmp(pEventTypeName, "RequestBossStage1AtkAct", strlen(pEventTypeName)) == 0)
        {
            C_ColisionActorManager& rActorMan = C_ColisionActorManager::inst();

            int max = m_PartsNum;
            for (int i = 0; i < max; ++i)
            {
                _Parts* pParts = (_Parts*)rActorMan.GetData(m_pPartsHandleList[i]);
                if (pParts == NULL)
                {
                    continue;
                }

                Vec3 vec((const float)(rand() % 640), (const float)(rand() % 480), 1.f);
                vec.Sub(pParts->GetPos());

                pParts->m_TargetVec = vec;
                pParts->m_OrgPos    = pParts->GetPos();
                pParts->m_Time      = 0.f;
            }

            BossStage1AttackParamEventData eventData(VGetHandle());
            eventData.VBuildLuaEventData(in_pLuaState);

            if (eventData.m_Angle == -1)
            {
                ::memset(&m_DirVec, 0, sizeof(m_DirVec));
            }
            else
            {
                m_DirVec.x = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
                m_DirVec.y = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
            }

            m_AttackState = _ATK_TARGET_MOVE;

            //	ステートを決める
            _SetState(eventData.m_pStateName);
        }
        else if (strncmp(pEventTypeName, "RequestBossStage1NormalAct", strlen(pEventTypeName)) == 0)
        {
            BossStage1NormalParamEventData eventData(VGetHandle());
            eventData.VBuildLuaEventData(in_pLuaState);

            //	ステートを決める
            _SetState(eventData.m_pStateName);

            if (eventData.m_Angle == -1)
            {
                ::memset(&m_DirVec, 0, sizeof(m_DirVec));
            }
            else
            {
                m_DirVec.x = cos(eventData.m_Angle * ANGLE_TO_RADIAN);
                m_DirVec.y = sin(eventData.m_Angle * ANGLE_TO_RADIAN);
            }

            m_NextStateCnt = eventData.m_NextStateCnt;
        }
    }

    /*
            @brief	通常
    */
    void BossStage1::_VNormalAct()
    {
        C_ColisionActorManager& rActorMan = C_ColisionActorManager::inst();

        Vec3 nowPos = m_DirVec;
        nowPos *= m_Speed;
        nowPos.Add(GetPos());
        SetPos(nowPos);

        int partsNum = m_PartsNum;
        for (int i = 0; i < partsNum; ++i)
        {
            _Parts* pParts = (_Parts*)rActorMan.GetData(m_pPartsHandleList[i]);
            if (pParts == NULL)
            {
                continue;
            }

            pParts->m_Angle += g_BossStage1PartsMoveAngle;
            Vec3 pos(cos(pParts->m_Angle * ANGLE_TO_RADIAN) * m_PartsDistance,
                     sin(pParts->m_Angle * ANGLE_TO_RADIAN) * m_PartsDistance, 1.f);

            pos.Add(m_pos);

            pParts->SetPos(pos);
        }

        --m_NextStateCnt;
        if (m_NextStateCnt <= 0)
        {
            //	次のターン
            BossStage1NormalParamEventData eventData(VGetHandle());
            LuaStateManager::DoFile(s_BossStage1ScriptFileName, "EndNormalAct", &eventData);
        }
    }

    /*
            @brief	攻撃
    */
    void BossStage1::_VAttackAct()
    {
        C_ColisionActorManager& rActorMan = C_ColisionActorManager::inst();
        int max                           = m_PartsNum;

        switch (m_AttackState)
        {
            case _ATK_TARGET_MOVE:
            {
                for (int i = 0; i < max; ++i)
                {
                    _Parts* pParts = (_Parts*)rActorMan.GetData(m_pPartsHandleList[i]);
                    if (pParts == NULL)
                    {
                        continue;
                    }

                    pParts->m_Time += 0.01f;
                    Vec3 targetVec(pParts->m_TargetVec);
                    targetVec *= pParts->m_Time;

                    Vec3 pos(pParts->m_OrgPos);
                    pos.Add(targetVec);
                    pParts->SetPos(pos);

                    if (pParts->m_Time >= 1.f)
                    {
                        m_AttackState = _ATK_PREV_MOVE;

                        Vec3 vec(pParts->m_OrgPos);
                        vec.Sub(pParts->GetPos());

                        pParts->m_OrgPos = pParts->GetPos();

                        pParts->m_PrevVec = vec;
                        pParts->m_Time    = 0.f;
                    }
                }

                break;
            }
            case _ATK_PREV_MOVE:
            {
                for (int i = 0; i < max; ++i)
                {
                    _Parts* pParts = (_Parts*)rActorMan.GetData(m_pPartsHandleList[i]);
                    if (pParts == NULL)
                    {
                        continue;
                    }

                    pParts->m_Time += 0.01f;
                    Vec3 prevVec(pParts->m_PrevVec);
                    prevVec *= pParts->m_Time;

                    Vec3 pos(pParts->m_OrgPos);
                    pos.Add(prevVec);
                    pParts->SetPos(pos);

                    if (pParts->m_Time >= 1.f)
                    {
                        m_AttackState = _ATK_END;
                    }
                }

                break;
            }
            case _ATK_END:
            {
                BossStage1AttackParamEventData eventData(VGetHandle());
                LuaStateManager::DoFile(s_BossStage1ScriptFileName, "EndAttackAct", &eventData);

                break;
            }
        }
    }

    /*
            @brief	クリア
    */
    void BossStage1::_Clear()
    {
        m_pPartsHandleList = NULL;
        m_PartsNum         = 0;
        m_PartsDistance    = 0.f;
        m_ActFuncIndex     = _NORMA_ACT;
        m_NextStateCnt     = 0;
        m_Point            = 0;
        m_Hp               = 0;
        m_Speed            = 0.f;
        ::memset(&m_DirVec, 0, sizeof(m_DirVec));
        m_AttackState = _ATK_TARGET_MOVE;
    }

    /*
            @brief	初期化
    */
    bool BossStage1::_Parts::init(const int in_Handle)
    {
        C_ColisionActor::init(in_Handle);

        return true;
    }

    /*
            @brief	終了
    */
    bool BossStage1::_Parts::final()
    {
        return true;
    }

    /*
            @brief	更新
    */
    bool BossStage1::_Parts::update(void)
    {
        return true;
    }

    /*
            @brief	描画
    */
    void BossStage1::_Parts::draw(void)
    {
        DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                  D3DCOLOR_ARGB(0, 0, 255, 255));
    }

    /*
            @brief	データ通知
    */
    void BossStage1::_Parts::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
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
            @brief	衝突検知
    */
    void BossStage1::_Parts::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
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
            @brief	スクリプト通知
    */
    void BossStage1::_Parts::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
    {
    }

    void BossStage1::_Parts::_Clear()
    {
        m_Angle = 0.f;
        ::memset(&m_TargetVec, 0, sizeof(m_TargetVec));
        ::memset(&m_PrevVec, 0, sizeof(m_PrevVec));
        ::memset(&m_OrgPos, 0, sizeof(m_OrgPos));

        m_Time = 0.f;

        m_Hp    = 0;
        m_Point = 0;
    }

}  // namespace InGame
#endif
