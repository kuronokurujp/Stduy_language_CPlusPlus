#pragma once

#if 0

//	インクルード
#include "Base.h"
#include "common/Man.h"

namespace InGame
{
/*
        @brief	雑魚敵クラス
*/
class C_EnemyActorZako : public C_EnemyActorBase
{
public:
    //	コンストラクタ
    C_EnemyActorZako(const ABLITY_TYPE in_AblityType);
    virtual ~C_EnemyActorZako();

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
    virtual void _VEscapeAct();
    virtual void _VAttackAct() {}

    virtual void _VDieAct();
    virtual void _VDamageAct() {}

    //	変数
    float m_Speed;
    Vec3 m_DirVec;
    Vec3 m_Vec;
};
}

#endif
