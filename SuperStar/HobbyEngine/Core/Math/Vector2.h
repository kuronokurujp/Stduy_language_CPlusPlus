#pragma once

#include <math.h>

#include "Core/Core.h"

namespace Core
{
    namespace Math
    {
        class Vector2
        {
        public:
            explicit Vector2() {}

            // コントラクト
            explicit Vector2(const Sint32 in_x, const Sint32 in_y)
            {
                this->x = static_cast<Float32>(in_x);
                this->y = static_cast<Float32>(in_y);
            }

            explicit Vector2(const Float32 in_x, const Float32 in_y) : x(in_x), y(in_y) {}

            /// <summary>
            /// Zeroes this instance.
            /// </summary>
            void Zero() E_NOEXCEPT
            {
                this->x = 0.f;
                this->y = 0.f;
            }

            /// <summary>
            /// Madds the specified a.
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="b">The b.</param>
            void Madd(const Vector2& a, Float32 b) E_NOEXCEPT
            {
                this->x += (a.x * b);
                this->y += (a.y * b);
            }

            /// <summary>
            /// Sets the madd.
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="b">The b.</param>
            /// <param name="c">The c.</param>
            void SetMadd(const Vector2& a, Float32 b, const Vector2& c) E_NOEXCEPT
            {
                this->x = a.x * b + c.x;
                this->y = a.y * b + c.y;
            }

            /// <summary>
            /// Sets the add.
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="b">The b.</param>
            void SetAdd(const Vector2& a, const Vector2& b) E_NOEXCEPT
            {
                this->x = a.x + b.x;
                this->y = a.y + b.y;
            }

            /// <summary>
            /// Sets the sub.
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="b">The b.</param>
            void SetSub(const Vector2& a, const Vector2& b) E_NOEXCEPT
            {
                this->x = a.x - b.x;
                this->y = a.y - b.y;
            }

            /// <summary>
            /// Sets the mul.
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="b">The b.</param>
            void SetMul(const Vector2& a, const Vector2& b) E_NOEXCEPT
            {
                this->x = a.x * b.x;
                this->y = a.y * b.y;
            }

            /// <summary>
            /// Sets the specified in value.
            /// </summary>
            /// <param name="in_value">The in value.</param>
            /// <returns></returns>
            inline void Set(const Float32 in_value) E_NOEXCEPT { this->x = this->y = in_value; }

            /// <summary>
            /// ベクトル型で設定
            /// </summary>
            /// <param name="in_value"></param>
            inline void Set(const Vector2& in_value) E_NOEXCEPT
            {
                this->x = in_value.x;
                this->y = in_value.y;
            }

            /// <summary>
            /// ２次元補間を行う
            /// </summary>
            /// <param name="a">a.</param>
            /// <param name="ab">The ab.</param>
            /// <param name="ac">The ac.</param>
            /// <param name="u">The u.</param>
            /// <param name="v">The v.</param>
            void SetInterporation(const Vector2& a, const Vector2& ab, const Vector2& ac, Float32 u,
                                  Float32 v) E_NOEXCEPT
            {
                this->SetMadd(ab, u, a);  // a + u(b - a)
                this->Madd(ac, v);        // a + u(b - a) + v(c - a)
            }

            /// <summary>
            /// 正規化
            /// </summary>
            void Normalize() E_NOEXCEPT
            {
                float magSq = this->x * this->x + this->y * this->y;
                if (magSq > 0.f)
                {
                    float newOverMag = 1.0f / sqrtf(magSq);
                    this->x *= newOverMag;
                    this->y *= newOverMag;
                }
            }

            // 演算定義
            // 足し算
            void operator+=(const Vector2& a) E_NOEXCEPT
            {
                this->x += a.x;
                this->y += a.y;
            }

            // 引き算
            void operator-=(const Vector2& a) E_NOEXCEPT
            {
                this->x -= a.x;
                this->y -= a.y;
            }

            // 掛け算
            void operator*=(const Vector2& a) E_NOEXCEPT
            {
                this->x *= a.x;
                this->y *= a.y;
            }

            /// <summary>
            /// Operator*s the specified scale.
            /// </summary>
            /// <param name="scale">The scale.</param>
            /// <returns></returns>
            const Vector2 operator*(const float scale) E_NOEXCEPT
            {
                return Vector2(this->x * scale, this->y * scale);
            }

            // 割り算
            void operator/=(const Vector2& a) E_NOEXCEPT
            {
                if ((this->x != 0.f) && (a.x != 0.f))
                {
                    this->x /= a.x;
                }

                if ((this->y != 0.f) && (a.y != 0.f))
                {
                    this->y /= a.y;
                }
            }

            // static メンバー関数で補助として利用

            /// <summary>
            /// Subs the vector2.
            /// </summary>
            /// <param name="in_rA">The in r a.</param>
            /// <param name="in_rB">The in r b.</param>
            /// <returns></returns>
            static inline const Vector2 Sub(const Vector2& in_rA, const Vector2& in_rB) E_NOEXCEPT
            {
                return Vector2(in_rA.x - in_rB.x, in_rA.y - in_rB.y);
            }

            // ベクトルの大きさを2乗したのを取得
            // こちらの方が計算が早い
            // 比較などで利用できる
            static inline float GetLengthSquared(const Vector2& in_rV) E_NOEXCEPT
            {
                return (in_rV.x * in_rV.x + in_rV.y * in_rV.y);
            }

            /// <summary>
            /// ベクトル同士の距離を2乗で取得(距離の比較のみでは計算の早いこちらを利用).
            /// </summary>
            /// <param name="in_v">The in v.</param>
            /// <param name="in_v2">The in v2.</param>
            /// <returns></returns>
            static inline float GetDistanceSquared(const Vector2& in_v,
                                                   const Vector2& in_v2) E_NOEXCEPT
            {
                Vector2 size;
                size.SetSub(in_v2, in_v);

                return GetLengthSquared(size);
            }

            //	ベクトルの大きさ取得
            static inline float GetLength(const Vector2& in_v)
            {
                return sqrtf(GetLengthSquared(in_v));
            }

            //	2つのベクトルの距離取得
            static inline float GetDistance(const Vector2& in_v, const Vector2& in_v2) E_NOEXCEPT
            {
                Vector2 size;
                size.SetSub(in_v2, in_v);

                return GetLength(size);
            }

            /// <summary>
            /// Dots the specified in v.
            /// </summary>
            /// <param name="in_v">The in v.</param>
            /// <param name="in_v2">The in v2.</param>
            /// <returns></returns>
            static inline float Dot(const Vector2& in_v, const Vector2& in_v2) E_NOEXCEPT
            {
                return (in_v.x * in_v2.x) + (in_v.y * in_v2.y);
            }

            /// <summary>
            /// Outputs the refection.
            /// </summary>
            /// <param name="out_pRefection">The out p refection.</param>
            /// <param name="in_nv">The in nv.</param>
            /// <param name="in_v2">The in v2.</param>
            /// <returns></returns>
            static inline void OutputRefection(Vector2* out_pRefection, const Vector2& in_nv,
                                               const Vector2& in_v2) E_NOEXCEPT
            {
                Vector2 flipV(-in_v2.x, -in_v2.y);
                const float dot = 2 * Dot(in_nv, flipV);
                Vector2 v(in_nv.x * dot, in_nv.y * dot);
                v -= flipV;
                out_pRefection->Set(v);
            }

            // 頻繁にアクセスする変数にはprivate指定にはしない
            Float32 x = 0.0f;
            Float32 y = 0.0f;
        };
    }  // namespace Math
}  // namespace Core
