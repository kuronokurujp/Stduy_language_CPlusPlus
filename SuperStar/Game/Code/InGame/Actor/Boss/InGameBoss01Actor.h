#pragma once

#if 0

#include "actor/ActorSystem.h"
#include "actor/enemy/Base.h"

namespace InGame
{
    /*
            ステージ１用ボス
    */
    class BossStage1 : public C_EnemyActorBase
    {
    public:
        //	コンストラクタ
        BossStage1() : C_EnemyActorBase(TYPE_PAWN) { _Clear(); }
        virtual ~BossStage1() {}

        // 初期化、終了
        // 更新、描画で十分
        virtual bool init(const int in_Handle);
        virtual bool final(void);
        virtual void draw(void);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        //	定義
        enum
        {
            _ATK_TARGET_MOVE = 0,
            _ATK_PREV_MOVE,
            _ATK_END,
        };

        //	パーツアクター
        class _Parts : public C_ColisionActor
        {
        public:
            //	コンストラクタ
            _Parts(const float in_Angle) : C_ColisionActor(16.f)
            {
                _Clear();
                m_Angle = in_Angle;
            }
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

            //	関数
            void _Clear();

            //	変数
            float m_Angle;
            Vec3 m_TargetVec;
            Vec3 m_PrevVec;
            Vec3 m_OrgPos;
            float m_Time;

            int m_Hp;
            int m_Point;
        };

        //	登録した自分以外のアクターを取得する。
        virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
        //	スクリプトで通知したデータを受け取る
        virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

        //	アクション
        virtual void _VNormalAct();
        virtual void _VAttackAct();

        //	関数
        void _Clear();

        //	変数

        int* m_pPartsHandleList;
        int m_PartsNum;
        float m_PartsDistance;

        int m_ActFuncIndex;
        int m_NextStateCnt;
        int m_Point;
        int m_Hp;
        Vec3 m_DirVec;
        float m_Speed;

        int m_AttackState;
    };
}  // namespace InGame
#endif
