#pragma once

#include "Engine/Core.h"

namespace Core::Math
{
    class Vector4 final
    {
    public:
        explicit Vector4() : _fX(0.0f), _fY(0.0f), _fZ(0.0f), _fW(0.0f) {}
        explicit Vector4(const Float32 in_fX, const Float32 in_fY, const Float32 in_fZ,
                         const Float32 in_fW)
            : _fX(in_fX), _fY(in_fY), _fZ(in_fZ), _fW(in_fW)
        {
        }

        Vector4 Mul(const Vector4& in_rSrc)
        {
            const Float32 outputX = this->_fX * in_rSrc._fX;
            const Float32 outputY = this->_fY * in_rSrc._fY;
            const Float32 outputZ = this->_fZ * in_rSrc._fZ;
            const Float32 outputW = this->_fW * in_rSrc._fW;

            return Vector4(outputX, outputY, outputZ, outputW);
        }

        const Float32 AddMul(const Vector4& in_rSrc)
        {
            const Float32 outputX = this->_fX * in_rSrc._fX;
            const Float32 outputY = this->_fY * in_rSrc._fY;
            const Float32 outputZ = this->_fZ * in_rSrc._fZ;
            const Float32 outputW = this->_fW * in_rSrc._fW;

            return (outputX + outputY + outputZ + outputW);
        }

    public:
        // 頻繁にアクセスする変数にはprivate指定にはしない
        Float32 _fX = 0.0f, _fY = 0.0f, _fZ = 0.0f, _fW = 0.0f;
    };
}  // namespace Core::Math
