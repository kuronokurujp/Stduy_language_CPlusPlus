#pragma once

// エンジン最小インクルード
#include "Engine/Math/Vector2.h"
#include "Engine/MiniEngine.h"
#include "InGameBulletInterface.h"

namespace InGame
{
    struct InGameBulletNormalWork
    {
        // 弾の位置
        Core::Math::Vector2 pos;

        // 弾の移動方向
        Core::Math::Vector2 dir;

        Float32 fFirstSpeed = 6.0f;
        Float32 fSpeed      = 0.0f;
        Float32 fAccele     = 0.0f;

        // 弾のサイズ
        Float32 fSize = 32.0f;
    };

    /// <summary>
    /// 通常弾の生成
    /// </summary>
    class InGameBulletNormalFactory final : public InGameBulletFactoryInterface
    {
        HE_CLASS_COPY_NG(InGameBulletNormalFactory);

    public:
        InGameBulletNormalFactory(Core::Math::Vector2& in_rPos, Core::Math::Vector2& in_rDir,
                                  const Uint32 in_uCollisionHashCode)
            : _pos(in_rPos), _dir(in_rDir), _uCollisionHashCode(in_uCollisionHashCode)
        {
        }

        const Char* VName() const;
        void VConfiguration(InGameBulletWork* in_pWork);

    private:
        Core::Math::Vector2 _pos;
        Core::Math::Vector2 _dir;
        Uint32 _uCollisionHashCode = 0;
    };

    /// <summary>
    /// 通常弾の制御アルゴリズム
    /// </summary>
    class InGameBulletNormalStrategy final : public InGameBulletStrategyInterface
    {
        HE_CLASS_COPY_NG(InGameBulletNormalStrategy);

    public:
        //	コンストラクタ
        InGameBulletNormalStrategy() = default;

        Uint32 VWorkSize() const override final;
        const Char* VName() const override final;

        Bool VUpdate(InGameBulletWork* in_pWork,
                     Core::Common::Handle& in_rViewHandle) override final;

        Bool VIsScreenInSide(const InGameBulletWork* const,
                             const Core::Math::Rect2&) override final;

        Bool VOutputCollisionData(CollisionData* out, const InGameBulletWork* const) override final;
#if 0
        void init();
        void release();
        void draw() const;

        bool isShot() const { return mb_shot; }
        void SetShotFlg(const bool in_Flg) { mb_shot = in_Flg; }

        /*
                @brief	打つ角度
                @param	角度指定
        */
        void setBulletAngle(float i_angle);

        /*
                @brief	弾を撃つ
                @param	i_spos	:	弾の初期座標
        */
        void shot(const D3DXVECTOR3& i_spos);

        /*
                @brief	交差判定
                @param	i_cpos		:	オブジェクトの中心座標
                @param	i_size_x	:	横サイズ
                @param	i_size_y	:	縦サイズ
                @return	衝突[true]	/	非衝突[false]
        */
        bool isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x, float i_size_y);

    private:
        bool mb_shot;
        D3DXVECTOR3 m_dir;

        struct _tag_Core
        {
            D3DXVECTOR3 pos;
            float speed;
            float accele;
        };
        struct _tag_Core m_core;
#endif
    };
}  // namespace InGame
