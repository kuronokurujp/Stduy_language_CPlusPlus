#pragma once

// エンジン最小インクルード
#include "Engine/Math/Rect2.h"
#include "Engine/MiniEngine.h"
#include "InGame/Component/InGameCollisionComponent.h"

namespace InGame
{
    /// <summary>
    /// 弾データ
    /// 弾アルゴリズムが弾データを使って弾の挙動処理をする
    /// </summary>
    struct InGameBulletWork
    {
        Uint32 uCollisionHashCode = 0;
        Uint8 aWork[256]          = {0};
    };

    struct InGameBulletObject
    {
        Char aName[64] = {};
        InGameBulletWork work;
    };

    /// <summary>
    /// 弾生成のインターフェイス
    /// </summary>
    class InGameBulletFactoryInterface
    {
        HE_CLASS_COPY_NG(InGameBulletFactoryInterface);

    public:
        explicit InGameBulletFactoryInterface() = default;

        virtual const Char* VName() const                       = 0;
        virtual void VConfiguration(InGameBulletWork* in_pWork) = 0;
    };

    /// <summary>
    /// 弾のアルゴリズムのインターフェイス
    /// </summary>
    class InGameBulletStrategyInterface
    {
        HE_CLASS_COPY_NG(InGameBulletStrategyInterface);

    public:
        InGameBulletStrategyInterface()          = default;
        virtual ~InGameBulletStrategyInterface() = default;

        virtual Uint32 VWorkSize() const  = 0;
        virtual const Char* VName() const = 0;

        virtual Bool VUpdate(InGameBulletWork*, Core::Common::Handle&) = 0;

        virtual Bool VIsScreenInSide(const InGameBulletWork* const, const Core::Math::Rect2&) = 0;

        virtual Bool VOutputCollisionData(CollisionData* out, const InGameBulletWork* const) = 0;

#if 0
        //	関数
        virtual void init()       = 0;
        virtual void release()    = 0;

        /*
                @brief	打つ角度
                @param	角度指定
        */
        virtual void setBulletAngle(float i_angle) = 0;

        /*
                @brief	弾を撃つ
                @param	i_spos	:	弾の初期座標
        */
        virtual void shot(const D3DXVECTOR3& i_spos) = 0;

        /*
                @brief	交差判定
                @param	i_cpos		:	オブジェクトの中心座標
                @param	i_size_x	:	横サイズ
                @param	i_size_y	:	縦サイズ
                @return	衝突[true]	/	非衝突[false]
        */
        virtual bool isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x, float i_size_y) = 0;
#endif
    };

}  // namespace InGame
