#pragma once

#include "Core/Core.h"
#include "Vector2.h"

namespace Core
{
    namespace Math
    {
        /// <summary>
        /// ベクトルクラス.
        /// このクラスはメモリを直接参照するので仮想関数を作ってはだめ.
        /// </summary>
        class Vector3
        {
        public:
            explicit Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
            explicit Vector3(Float32 aX, Float32 aY, Float32 aZ) : x(aX), y(aY), z(aZ) {}
            explicit Vector3(const Vector2& in_rVec2) : x(in_rVec2.x), y(in_rVec2.y), z(0.0f) {}

            // 整数設定
            explicit Vector3(Sint32 aX, Sint32 aY, Sint32 aZ);

            // 値設定
            void Set(const Float32 in_x, const Float32 in_y, const Float32 in_z);
            void Set(const Float32 in_val);

            // 掛け算
            void Mul(const Float32 b);

            // 積和
            void Madd(const Vector3& a, const Float32 b);

            // 積和設定
            void SetMadd(const Vector3& a, const Float32 b, const Vector3& c);

            // 加算
            void SetAdd(const Vector3& a, const Vector3& b);

            // 減算
            void SetSub(const Vector3& a, const Vector3& b);

            // 積算
            void SetMul(const Vector3& a, const Vector3& b);

            // ２次元補間を行う
            void SetInterporation(const Vector3& a, const Vector3& ab, const Vector3& ac, Float32 u,
                                  Float32 v);

            //	正規化
            void Normalize();

            // 演算定義
            void operator+=(const Vector3& a);  // 足し算
            void operator-=(const Vector3& a);  // 引き算
            void operator=(const Vector3& a);   // コピー
            void operator*=(const Vector3& a);  // 掛け算
            void operator/=(const Vector3& a);  // 割り算

            /// <summary>
            /// Operator*s the specified scale.
            /// オペレーターで戻り値はconstにしない方が良い
            /// 戻り値はどのようにアクセスするのか予測が出来ない
            /// constを入れるのか入れないのか、汎用として扱えるようにするためには制約は付けてはいけない
            /// </summary>
            /// <param name="scale">The scale.</param>
            /// <returns></returns>
            Vector3 operator*(const Float32 scale) E_NOEXCEPT
            {
                return Vector3(this->x * scale, this->y * scale, this->z * scale);
            }

            /// <summary>
            /// Operator+s the specified in a.
            /// </summary>
            /// <param name="in_a">The in a.</param>
            /// <returns></returns>
            Vector3 operator+(const Vector3 in_a) E_NOEXCEPT
            {
                return Vector3(this->x + in_a.x, this->y + in_a.y, this->z + in_a.z);
            }

            /// <summary>
            /// ベクトル要素をfloatポインタで返す.
            /// </summary>
            /// <returns></returns>
            const Float32* GetAsFloatPtr() E_NOEXCEPT
            {
                return reinterpret_cast<Float32*>(&this->x);
            }

            // 頻繁にアクセスする変数にはprivate指定にはしない
            Float32 x = 0.0f;
            Float32 y = 0.0f;
            Float32 z = 0.0f;

            // 各要素を1にしてそれ以外の要素を0にしたベクトル
            static Vector3 Unit_X;
            static Vector3 Unit_Y;
            static Vector3 Unit_Z;
            static Vector3 Zero;
            static Vector3 One;

            // 内積
            static float Dot(const Vector3& in_rV, const Vector3& in_rV2);

            // 外積
            static Vector3 Cross(const Vector3& in_rV, const Vector3& in_rV2);

            // ベクトルの大きさを2乗したのを取得
            // こちらの方が計算が早い
            // 比較などで利用できる
            static float GetLengthSquared(const Vector3& in_rV);

            // 大きさ取得
            static float GetVector3Mag(const Vector3& in_v);

            // 距離取得
            static float GetVector3Distance(const Vector3& in_v, const Vector3& in_v2);
        };
    }  // namespace Math
}  // namespace Core
