#pragma once

#include "Actor/Actor.h"
#include "Engine/MiniEngine.h"

#if 0

#include "actor/ActorSystem.h"
#include "actor/Shot/Manager.h"
#endif

namespace InGame
{
    class InGamePlayerActor final : public Actor::Object
    {
    public:
        InGamePlayerActor();
        virtual ~InGamePlayerActor() = default;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        const Bool VBegin() final override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        const Bool VEnd() final override;

        void VUpdate(const Float32 in_fDt) final override;

        void SetPos(const Core::Math::Vector2&);
        void SetSize(const Core::Math::Vector2&);

#if 0
          //	撃てる弾の名前を取得
        const char* GetActiveShotName() const;
        const char* GetShotName(const int in_Type) const;
        const int GetShotTypeNum() const { return eSHOT_MAX; }

        const int GetLifeNum() const { return m_life; }

        void Shot();
        void MoveShotType(const int in_MoveNum);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);
#endif

    private:
        void _Clear()
        {
            m_shotType      = eSHOT_BASIC;
            m_life          = 4;
            m_InvincibleCnt = 0;

            this->_pos.Zero();
            this->_transformHandle.Clear();
        }

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

        Sint32 m_aShotHandle[eSHOT_MAX];

        Uint32 m_shotType;
        Uint32 m_life;
        Sint32 m_InvincibleCnt;

        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _size;
        Core::Common::Handle _transformHandle;
    };
}  // namespace InGame
