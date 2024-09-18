#pragma once

#if 0
//	インクルード
#include "actor/ActorSystem.h"
#include "common/Common.h"

namespace InGame
{
    class C_EnemyActorBase : public C_ColisionActor
    {
    public:
        //	強さ
        typedef enum
        {
            TYPE_PAWN = 0,
            TYPE_KNIGHT,

            TYPE_MAX,
        } ABLITY_TYPE;

        //	コンストラクタ
        C_EnemyActorBase(const ABLITY_TYPE in_AblityType);
        virtual ~C_EnemyActorBase(void);

        //	関数
        virtual bool init(const int in_Handle);
        virtual bool update(void);
        virtual void draw(void) {}

        virtual void VDie();

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

        static ABLITY_TYPE ChangeAblityTypeStringToNum(const char* in_pString);

    protected:
        //	定義
        typedef struct
        {
            char* pFuncName;
            void (C_EnemyActorBase::*pActFunc)();
        } _FUNC_TABLE_ST;

        enum
        {
            _NORMA_ACT = 0,
            _ESCAPE_ACT,
            _ATTACK_ACT,
            _DIE_ACT,
            _DAMAGE_ACT,
        };

        //	関数
        //	ステートがあるかチェック
        bool _SetState(const char* in_pStateName);
        bool _SetState(const unsigned in_stateIndex);

        int _GetIndexState(const char* in_pStateName);

    protected:
        //	関数
        static int GetTypeNameToIndex(const char* in_pName);

        //	変数
        int m_Hp;
        int m_NextStateCnt;
        unsigned m_Point;

        ABLITY_TYPE m_eAblityType;

    private:
        //	メンバー関数テーブル
        static _FUNC_TABLE_ST m_aFuncTable[];

        //	関数
        virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
        //	スクリプトで通知したデータを受け取る
        virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState) {}

        void _Clear();
        void _ExeAct();

        //	アクション
        virtual void _VNormalAct() {}
        virtual void _VEscapeAct() {}
        virtual void _VAttackAct() {}

        virtual void _VDieAct() {}
        virtual void _VDamageAct() {}

        //	変数
        int m_ActIndex;
    };
}  // namespace InGame
#endif
