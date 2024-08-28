#pragma once

#if 0
//	インクルード
#include "Base.h"
#include "common/Man.h"

namespace InGame
{
    class EnemyActorSnake : public C_EnemyActorBase
    {
    public:
        //	コンストラクタ
        EnemyActorSnake();
        virtual ~EnemyActorSnake() {}

        //	オーバーライド関数
        virtual bool init(const int in_Handle);
        virtual bool final(void);

        virtual bool update(void);
        virtual void draw(void);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        //	パーツアクター
        class _Parts : public C_ColisionActor
        {
        public:
            //	コンストラクタ
            _Parts() : C_ColisionActor(16.f) { _Clear(); }
            virtual ~_Parts() {}

            //	オーバーライド関数
            virtual bool init(const int in_Handle);
            virtual bool final();
            virtual bool update(void);
            virtual void draw(void);

            //	データ通知
            virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

            //	オーバーライド関数
            virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
            //	スクリプトで通知したデータを受け取る
            virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

            void _Clear();

            //	変数
            int m_Handle;

            unsigned m_OldPosIndex;
            unsigned m_PosIndexNum;

            C_ColisionActor* m_pPrec;
            Vec3* m_pOldPosList;
        };

        //	オーバーライド関数
        virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
        //	スクリプトで通知したデータを受け取る
        virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

        //	アクション
        virtual void _VNormalAct();
        virtual void _VEscapeAct();
        virtual void _VAttackAct() {}

        virtual void _VDieAct();
        virtual void _VDamageAct() {}

        //	関数
        void _Clear();

        _Parts** m_ppPartsList;
        unsigned m_PartsNum;
        unsigned m_OldPosIndex;
        unsigned m_PosIndexNum;

        float m_Speed;
        float m_Angle;

        Vec3 m_DirVec;
        Vec3* m_pOldPosList;
    };
}  // namespace InGame
#endif
