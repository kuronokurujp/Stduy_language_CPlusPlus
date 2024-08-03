#include "Vector3.h"

#include <math.h>

namespace Core::Math
{
    Vector3 Vector3::UnitX = Vector3(1.0f, 0.0f, 0.0f);
    Vector3 Vector3::UnitY = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 Vector3::UnitZ = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 Vector3::Zero  = Vector3();
    Vector3 Vector3::One   = Vector3(1.0f, 1.0f, 1.0f);

    Vector3::Vector3(const Sint32 in_iX, const Sint32 in_iY, const Sint32 in_iZ)
    {
        _fX = static_cast<Float32>(in_iX);
        _fY = static_cast<Float32>(in_iY);
        _fZ = static_cast<Float32>(in_iZ);
    }

    // 値設定
    void Vector3::Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ)
    {
        this->_fX = in_fX;
        this->_fY = in_fY;
        this->_fZ = in_fZ;
    }

    void Vector3::Set(const Float32 in_fVal)
    {
        this->_fX = this->_fY = this->_fZ = in_fVal;
    }

    void Vector3::operator+=(const Vector3& a)
    {
        this->_fX += a._fX;
        this->_fY += a._fY;
        this->_fZ += a._fZ;
    }

    void Vector3::operator-=(const Vector3& a)
    {
        this->_fX -= a._fX;
        this->_fY -= a._fY;
        this->_fZ -= a._fZ;
    }

    void Vector3::operator=(const Vector3& a)
    {
        this->_fX = a._fX;
        this->_fY = a._fY;
        this->_fZ = a._fZ;
    }

    void Vector3::operator*=(const Vector3& a)
    {
        this->_fX *= a._fX;
        this->_fY *= a._fY;
        this->_fZ *= a._fZ;
    }

    // 割り算
    void Vector3::operator/=(const Vector3& a)
    {
        this->_fX /= a._fX;
        this->_fY /= a._fY;
        this->_fZ /= a._fZ;
    }

    // 掛け算
    void Vector3::Mul(const Float32 b)
    {
        this->_fX *= b;
        this->_fY *= b;
        this->_fZ *= b;
    }

    // 積和
    // a 方向ベクトル
    // b 方向ベクトルのスカラー
    void Vector3::Madd(const Vector3& a, const Float32 b)
    {
        this->_fX += (a._fX * b);
        this->_fY += (a._fY * b);
        this->_fZ += (a._fZ * b);
    }

    // 積和設定
    // a 方向ベクトル
    // b 方向ベクトルの長さスカラー
    // c 視点ベクトル
    void Vector3::SetMadd(const Vector3& a, const Float32 b, const Vector3& c)
    {
        this->_fX = a._fX * b + c._fX;
        this->_fY = a._fY * b + c._fY;
        this->_fZ = a._fZ * b + c._fZ;
    }

    // 加算
    void Vector3::SetAdd(const Vector3& a, const Vector3& b)
    {
        this->_fX = a._fX + b._fX;
        this->_fY = a._fY + b._fY;
        this->_fZ = a._fZ + b._fZ;
    }

    // 減算
    void Vector3::SetSub(const Vector3& a, const Vector3& b)
    {
        this->_fX = a._fX - b._fX;
        this->_fY = a._fY - b._fY;
        this->_fZ = a._fZ - b._fZ;
    }

    // 積算
    void Vector3::SetMul(const Vector3& a, const Vector3& b)
    {
        this->_fX = a._fX * b._fX;
        this->_fY = a._fY * b._fY;
        this->_fZ = a._fZ * b._fZ;
    }

    // ２次元補間を行う
    void Vector3::SetInterporation(const Vector3& a, const Vector3& ab, const Vector3& ac,
                                   Float32 u, Float32 v)
    {
        this->SetMadd(ab, u, a);  // a + u(b - a)
        this->Madd(ac, v);        // a + u(b - a) + v(c - a)
    }

    //	正規化
    void Vector3::Normalize()
    {
        Float32 fMagSq = this->_fX * this->_fX + this->_fY * this->_fY + this->_fZ * this->_fZ;
        if (fMagSq > 0.f)
        {
            Float32 onew_over_mag = 1.f / static_cast<Float32>(sqrt(fMagSq));
            this->_fX *= onew_over_mag;
            this->_fY *= onew_over_mag;
            this->_fZ *= onew_over_mag;
        }
    }

    // 内積
    Float32 Vector3::Dot(const Vector3& in_rV, const Vector3& in_rV2)
    {
        return (in_rV._fX * in_rV2._fX + in_rV._fY * in_rV2._fY + in_rV._fZ * in_rV2._fZ);
    }

    // 外積
    Vector3 Vector3::Cross(const Vector3& in_rV, const Vector3& in_rV2)
    {
        Vector3 temp;
        temp._fX = in_rV._fY * in_rV2._fZ - in_rV._fZ * in_rV2._fY;
        temp._fY = in_rV._fZ * in_rV2._fX - in_rV._fX * in_rV2._fZ;
        temp._fZ = in_rV._fX * in_rV2._fY - in_rV._fY * in_rV2._fX;

        return temp;
    }

    // ベクトルの大きさを2乗したのを取得
    // こちらの方が計算が早い
    // 比較などで利用できる
    Float32 Vector3::GetLengthSquared(const Vector3& in_rV)
    {
        return (in_rV._fX * in_rV._fX + in_rV._fY * in_rV._fY * in_rV._fZ * in_rV._fZ);
    }

    //	大きさ取得
    Float32 Vector3::GetVector3Mag(const Vector3& in_v)
    {
        return static_cast<Float32>(sqrt(GetLengthSquared(in_v)));
    }

    //	距離取得
    Float32 Vector3::GetVector3Distance(const Vector3& in_v, const Vector3& in_v2)
    {
        Vector3 size;
        size.SetSub(in_v2, in_v);

        return (GetVector3Mag(size));
    }
}  // namespace Core::Math
