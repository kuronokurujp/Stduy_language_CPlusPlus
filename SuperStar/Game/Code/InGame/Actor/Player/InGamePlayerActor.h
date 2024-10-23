#pragma once

#include "Actor/Actor.h"
#include "Engine/Common/CustomMap.h"
#include "Engine/MiniEngine.h"
#include "InGame/Component/Shot/InGameShotStrategyInterface.h"

#if 0

#include "actor/ActorSystem.h"
#include "actor/Shot/Manager.h"
#endif

namespace InGame
{
    class InGamePlayerActor final : public Actor::Object
    {
        HE_GENERATED_CLASS_BODY_HEADER(InGamePlayerActor, Actor::Object);

    public:
        /// <summary>
        /// 自機のパラメータ
        /// </summary>
        struct Parameter
        {
            // 移動スピード
            Float32 speed = 5.0f;

            // 無敵時間
            Float32 fInvincibleTimeSec = 1.5;
            // HP
            Uint32 ulife = 3;
        };

    public:
        InGamePlayerActor();
        virtual ~InGamePlayerActor() = default;

        /// <summary>
        /// 開始
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        Bool VBegin() final override;

        /// <summary>
        /// 終了
        /// 継承したクラスで必ず基本クラスのメソッドを呼ぶ
        /// </summary>
        Bool VEnd() final override;

        void VUpdate(const Float32 in_fDt) final override;

        void SetPos(const Core::Math::Vector2&);
        void SetSize(const Core::Math::Vector2&);
        void SetViewHandle(const Core::Common::Handle&);

        void Move(const Core::Math::Vector2&);
        void Shot();

        const Parameter& GetParameter() const { return this->_parameter; }

#if 0
          //	撃てる弾の名前を取得
        const char* GetActiveShotName() const;
        const char* GetShotName(const int in_Type) const;
        int GetShotTypeNum() const { return eSHOT_MAX; }

        int GetLifeNum() const { return m_life; }

        void MoveShotType(const int in_MoveNum);

        //	データ通知
        virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);
#endif

    private:
        void _Clear()
        {
            this->_pos.Zero();
            this->_size.Zero();
            this->_move.Zero();

            this->_transformHandle.Clear();
            this->_viewHandle.Clear();
        }

    private:
        /*
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
            */

        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _size;
        Core::Math::Vector2 _move;

        Core::Common::Handle _transformHandle;
        Core::Common::Handle _collisoinHandle;
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _shotHandle;

        // ランタイム中に変化するパラメータ
        Parameter _parameter;
        // 初期パラメータ
        Parameter _defaultParameter;

        Core::Common::CustomFixMap<Core::Common::FixString32,
                                   Core::Memory::SharedPtr<InGameShotStrategyInterface>, 6>
            _mShotStrategy;
    };

}  // namespace InGame
