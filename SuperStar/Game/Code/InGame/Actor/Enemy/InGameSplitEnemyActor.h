#pragma once

#if 0
//	インクルード
#include "Base.h"

namespace InGame
{
    /*
            @brief	敵分裂
    */
    class EnemyActorSplit : public C_EnemyActorBase
    {
    public:
        //	コンストラクタ
        EnemyActorSplit();
        virtual ~EnemyActorSplit() {}

        //	関数
        virtual bool init(const int in_Handle);
        virtual bool update(void);
        virtual void draw(void);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        //	関数
        void _Clear();

        virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
        //	スクリプトで通知したデータを受け取る
        virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

        //	アクション
        virtual void _VNormalAct();
        virtual void _VEscapeAct() {}
        virtual void _VAttackAct() {}

        virtual void _VDieAct();
        virtual void _VDamageAct() {}

        //	変数
        class C_EnemyActorZako* m_pZakoList;
        int m_ZakoNum;

        float m_Speed;
        Vec3 m_DirVec;
    };

}  // namespace InGame
#endif
