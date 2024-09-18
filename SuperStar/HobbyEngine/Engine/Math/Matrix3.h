#pragma once

#include "Engine/Core.h"

namespace Core::Math
{
    class Matrix3 final
    {
    public:
        explicit Matrix3();

        /// <summary>
        /// 行列式を取得.
        /// </summary>
        Float32 GetDet();

    public:
        Float32 _faaMat[3][3];
    };
}  // namespace Core::Math
