#pragma once

#include "Engine/Core.h"

namespace Core
{
    namespace Math
    {
        class Matrix3
        {
        public:
            Float32 _faaMat[3][3];

            Matrix3();

            /// <summary>
            /// 行列式を取得.
            /// </summary>
            Float32 GetDet();
        };
    }  // namespace Math

}  // namespace Core
