#pragma once

#include "Engine/MiniEngine.h"

#if 0

#include "actor/ActorSystem.h"
#include "actor/Shot/Manager.h"

namespace InGame
{
    class C_PlayerActor : public C_ColisionActor
    {
    public:
        C_PlayerActor(void);
        virtual ~C_PlayerActor(void);

        //	関数
        bool update(void);
        void draw(void);

        //	撃てる弾の名前を取得
        const char* GetActiveShotName() const;
        const char* GetShotName(const int in_Type) const;
        const int GetShotTypeNum() const { return eSHOT_MAX; }

        const int GetLifeNum() const { return m_life; }

        void Shot();
        void MoveShotType(const int in_MoveNum);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);

    private:
        //	定数
        enum
        {
            eSHOT_BASIC = 0,
            eSHOT_LASER,
            eSHOT_HOMING,
            eSHOT_WAY,

            eSHOT_MAX
        };

        //	変数
        int m_aShotHandle[eSHOT_MAX];

        int m_shotType;
        unsigned m_life;
        int m_InvincibleCnt;
    };
}  // namespace InGame
#endif
