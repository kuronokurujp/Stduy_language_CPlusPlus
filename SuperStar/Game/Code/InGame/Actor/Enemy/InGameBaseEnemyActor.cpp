#include "InGameBaseEnemyActor.h"

#if 0
#include "actor/ActorCommon.h"
#include "actor/shot/Manager.h"
#include "common/Man.h"
#include "game/GameSystem.h"
#include "tips/Collsion.h"

namespace InGame
{
    //	アクション関数テーブル
    C_EnemyActorBase::_FUNC_TABLE_ST C_EnemyActorBase::m_aFuncTable[] = {
        {"Normal", &C_EnemyActorBase::_VNormalAct}, {"Escape", &C_EnemyActorBase::_VEscapeAct},
        {"Attack", &C_EnemyActorBase::_VAttackAct}, {"Die", &C_EnemyActorBase::_VDieAct},
        {"Damage", &C_EnemyActorBase::_VDamageAct},
    };

    static const char* s_apEnemyTypeName[C_EnemyActorBase::TYPE_MAX] = {
        "Pawn",
        "Knight",
    };

    /*
            @brief	コンストラクタ
    */
    C_EnemyActorBase::C_EnemyActorBase(const ABLITY_TYPE in_AblityType)
        : C_ColisionActor(Vec3(0.f, 0.f, 0.f), 0.f)
    {
        _Clear();

        STRING_ASSERT(in_AblityType < TYPE_MAX, "AblityType Set Miss %d\n", in_AblityType);
        m_eAblityType = in_AblityType;
    }

    /*
     */
    C_EnemyActorBase::~C_EnemyActorBase(void)
    {
    }

    /*
            @brief	初期化
    */
    bool C_EnemyActorBase::init(const int in_Handle)
    {
        C_ColisionActor::init(in_Handle);

        return true;
    }

    /*
            @brief	更新
    */
    bool C_EnemyActorBase::update(void)
    {
        _ExeAct();

        return true;
    }

    /*
            @brief	タイプ名からIndex値を取得
            @return	インデックス値 / 失敗 : -1
    */
    int C_EnemyActorBase::GetTypeNameToIndex(const char* in_pName)
    {
        for (int i = 0; i < ARRAY_NUM(s_apEnemyTypeName); ++i)
        {
            if (strncmp(s_apEnemyTypeName[i], in_pName, strlen(s_apEnemyTypeName[i])) == 0)
            {
                return i;
            }
        }

        return TYPE_MAX;
    }

    /*
            @brief	死亡
    */
    void C_EnemyActorBase::VDie()
    {
        if (VIsDie() == false)
        {
            C_ColisionActor::VDie();

            //	死亡時に関数を呼ぶ
            _VDieAct();
        }
    }

    /*
            @brief	データ通知
    */
    void C_EnemyActorBase::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
    }

    /*
            @brief	他アクターチェック
    */
    void C_EnemyActorBase::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
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
            @brief	タイプ文字列からタイプ数に変換
    */
    const C_EnemyActorBase::ABLITY_TYPE C_EnemyActorBase::ChangeAblityTypeStringToNum(
        const char* in_pString)
    {
        if (in_pString == NULL)
        {
            TYPE_MAX;
        }

        int num = ARRAY_NUM(s_apEnemyTypeName);
        for (int i = 0; i < num; ++i)
        {
            if (strncmp(s_apEnemyTypeName[i], in_pString, strlen(in_pString)) == 0)
            {
                return (ABLITY_TYPE)i;
            }
        }

        return TYPE_MAX;
    }

    /*
            @brief	ステート設定
            @return	成功 : true / 失敗 : false
    */
    bool C_EnemyActorBase::_SetState(const char* in_pStateName)
    {
        int index = _GetIndexState(in_pStateName);
        if (index >= 0)
        {
            m_ActIndex = index;
            return true;
        }

        return false;
    }

    /*
            @brief
            @return
    */
    bool C_EnemyActorBase::_SetState(const unsigned in_stateIndex)
    {
        if (in_stateIndex < ARRAY_NUM(m_aFuncTable))
        {
            m_ActIndex = in_stateIndex;
            return true;
        }

        return false;
    }

    /*
            @brief	ステートがあるかチェック
            @param	[in]	:	ステート名
            @return	ステートIndex
    */
    int C_EnemyActorBase::_GetIndexState(const char* in_pStateName)
    {
        if (in_pStateName == NULL)
        {
            return -1;
        }

        for (int i = 0; i < ARRAY_NUM(m_aFuncTable); ++i)
        {
            if (strncmp(m_aFuncTable[i].pFuncName, in_pStateName,
                        strlen(m_aFuncTable->pFuncName)) == 0)
            {
                //	目的の関数があるか
                return i;
            }
        }

        return -1;
    }

    /*
            @brief	変数クリア
    */
    void C_EnemyActorBase::_Clear()
    {
        m_pos = Vec3(0.f, 0.f, 0.f);

        m_Hp           = 0;
        m_NextStateCnt = 0;
        m_ActIndex     = -1;
        m_eAblityType  = TYPE_PAWN;
    }

    /*
            @brief	アクション処理
    */
    void C_EnemyActorBase::_ExeAct()
    {
        int ActNum = ARRAY_NUM(m_aFuncTable);
        if ((m_ActIndex < 0) || (ActNum <= m_ActIndex))
        {
            return;
        }

        //	アクション実行
        void (C_EnemyActorBase::*pActFunc)() = m_aFuncTable[m_ActIndex].pActFunc;
        (this->*pActFunc)();
    }

}  // namespace InGame
#endif
