#pragma once

#include <math.h>

#include "Core.h"

namespace Core
{
    namespace Math
    {
        class Vector2
        {
        public:
            explicit Vector2() {}

            // コントラクト
            explicit Vector2(const Sint32 in_iX, const Sint32 in_iY)
            {
                this->_fX = static_cast<Float32>(in_iX);
                this->_fY = static_cast<Float32>(in_iY);
            }

            explicit Vector2(const Float32 in_fX, const Float32 in_fY) : _fX(in_fX), _fY(in_fY) {}

            /// <summary>
            /// Zeroes this instance.
            /// </summary>
            void Zero() HE_NOEXCEPT
            {
                this->_fX = 0.f;
                this->_fY = 0.f;
            }

            /// <summary>
            /// Madds the specified a.
            /// </summary>
            void Madd(const Vector2& in_rBase, const Float32 in_fScale) HE_NOEXCEPT
            {
                this->_fX += (in_rBase._fX * in_fScale);
                this->_fY += (in_rBase._fY * in_fScale);
            }

            /// <summary>
            /// Sets the madd.
            /// </summary>
            void SetMadd(const Vector2& in_rOffset, const Float32 in_fScale,
                         const Vector2& in_rBase) HE_NOEXCEPT
            {
                this->_fX = in_rOffset._fX * in_fScale + in_rBase._fX;
                this->_fY = in_rOffset._fY * in_fScale + in_rBase._fY;
            }

            /// <summary>
            /// Sets the add.
            /// </summary>
            void SetAdd(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
            {
                this->_fX = in_rLeft._fX + in_rRight._fX;
                this->_fY = in_rLeft._fY + in_rRight._fY;
            }

            /// <summary>
            /// Sets the sub.
            /// </summary>
            void SetSub(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
            {
                this->_fX = in_rLeft._fX - in_rRight._fX;
                this->_fY = in_rLeft._fY - in_rRight._fY;
            }

            /// <summary>
            /// Sets the mul.
            /// </summary>
            void SetMul(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
            {
                this->_fX = in_rLeft._fX * in_rRight._fX;
                this->_fY = in_rLeft._fY * in_rRight._fY;
            }

            /// <summary>
            /// Sets the specified in value.
            /// </summary>
            inline void Set(const Float32 in_fValue) HE_NOEXCEPT
            {
                this->_fX = this->_fY = in_fValue;
            }

            /// <summary>
            /// ベクトル型で設定
            /// </summary>
            inline void Set(const Vector2& in_rValue) HE_NOEXCEPT
            {
                this->_fX = in_rValue._fX;
                this->_fY = in_rValue._fY;
            }

            /// <summary>
            /// ２次元補間を行う
            /// </summary>
            void SetInterporation(const Vector2& in_rBase, const Vector2& in_rULine,
                                  const Vector2& in_rVLIne, const Float32 in_fUScale,
                                  const Float32 in_fVScale) HE_NOEXCEPT
            {
                // a + u(b - a)
                this->SetMadd(in_rULine, in_fUScale, in_rBase);
                // a + u(b - a) + v(c - a)
                this->Madd(in_rVLIne, in_fVScale);
            }

            /// <summary>
            /// 正規化
            /// </summary>
            void Normalize() HE_NOEXCEPT
            {
                const Float32 fMagSq = this->_fX * this->_fX + this->_fY * this->_fY;
                if (fMagSq > 0.f)
                {
                    const Float32 fNewOverMag = 1.0f / sqrtf(fMagSq);
                    this->_fX *= fNewOverMag;
                    this->_fY *= fNewOverMag;
                }
            }

            // 演算定義
            // 足し算
            void operator+=(const Vector2& a) HE_NOEXCEPT
            {
                this->_fX += a._fX;
                this->_fY += a._fY;
            }

            // 引き算
            void operator-=(const Vector2& a) HE_NOEXCEPT
            {
                this->_fX -= a._fX;
                this->_fY -= a._fY;
            }

            // 掛け算
            void operator*=(const Vector2& a) HE_NOEXCEPT
            {
                this->_fX *= a._fX;
                this->_fY *= a._fY;
            }

            /// <summary>
            /// Operator*s the specified scale.
            /// </summary>
            const Vector2 operator*(const Float32 in_fScale) HE_NOEXCEPT
            {
                return Vector2(this->_fX * in_fScale, this->_fY * in_fScale);
            }

            // 割り算
            void operator/=(const Vector2& a) HE_NOEXCEPT
            {
                if ((this->_fX != 0.f) && (a._fX != 0.f))
                {
                    this->_fX /= a._fX;
                }

                if ((this->_fY != 0.f) && (a._fY != 0.f))
                {
                    this->_fY /= a._fY;
                }
            }

            // static メンバー関数で補助として利用

            /// <summary>
            /// Subs the vector2.
            /// </summary>
            static inline const Vector2 Sub(const Vector2& in_rLeft,
                                            const Vector2& in_rRight) HE_NOEXCEPT
            {
                return Vector2(in_rLeft._fX - in_rRight._fX, in_rLeft._fY - in_rRight._fY);
            }

            // ベクトルの大きさを2乗したのを取得
            // こちらの方が計算が早い
            // 比較などで利用できる
            static inline Float32 GetLengthSquared(const Vector2& in_rVec) HE_NOEXCEPT
            {
                return (in_rVec._fX * in_rVec._fX + in_rVec._fY * in_rVec._fY);
            }

            /// <summary>
            /// ベクトル同士の距離を2乗で取得(距離の比較のみでは計算の早いこちらを利用).
            /// </summary>
            /// <param name="in_v">The in v.</param>
            /// <param name="in_v2">The in v2.</param>
            /// <returns></returns>
            static inline Float32 GetDistanceSquared(const Vector2& in_rRight,
                                                     const Vector2& in_rLeft) HE_NOEXCEPT
            {
                Vector2 size;
                size.SetSub(in_rLeft, in_rRight);

                return GetLengthSquared(size);
            }

            //	ベクトルの大きさ取得
            static inline Float32 GetLength(const Vector2& in_v)
            {
                return sqrtf(GetLengthSquared(in_v));
            }

            //	2つのベクトルの距離取得
            static inline float GetDistance(const Vector2& in_rRight,
                                            const Vector2& in_rLeft) HE_NOEXCEPT
            {
                Vector2 size;
                size.SetSub(in_rLeft, in_rRight);

                return GetLength(size);
            }

            /// <summary>
            /// Dots the specified in v.
            /// </summary>
            static inline float Dot(const Vector2& in_rLeft, const Vector2& in_rRight) HE_NOEXCEPT
            {
                return (in_rLeft._fX * in_rRight._fX) + (in_rLeft._fY * in_rRight._fY);
            }

            /// <summary>
            /// Outputs the refection.
            /// </summary>
            static inline void OutputRefection(Vector2* out, const Vector2& in_nv,
                                               const Vector2& in_v2) HE_NOEXCEPT
            {
                Vector2 flipV(-in_v2._fX, -in_v2._fY);
                const Float32 cfDot = 2 * Dot(in_nv, flipV);
                Vector2 v(in_nv._fX * cfDot, in_nv._fY * cfDot);
                v -= flipV;
                out->Set(v);
            }

            // 頻繁にアクセスする変数にはprivate指定にはしない
            Float32 _fX = 0.0f;
            Float32 _fY = 0.0f;
        };
    }  // namespace Math
}  // namespace Core
