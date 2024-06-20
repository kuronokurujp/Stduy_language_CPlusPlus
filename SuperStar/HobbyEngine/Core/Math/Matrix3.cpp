#include "Math/matrix3.h"

#include <string.h>

namespace Math
{
    Matrix3::Matrix3()
    {
        memset(&this->mat[0][0], 0, sizeof(this->mat[0][0]) * 3 * 3);
    }

    /// <summary>
    /// 行列式を取得.
    /// </summary>
    /// <returns></returns>
    float Matrix3::GetDet()
    {
        float a = this->mat[0][0] *
                  (this->mat[1][1] * this->mat[2][2] - this->mat[1][2] * this->mat[2][1]);
        float b = this->mat[0][1] *
                  (this->mat[1][2] * this->mat[2][0] - this->mat[1][0] * this->mat[2][2]);
        float c = this->mat[0][2] *
                  (this->mat[1][0] * this->mat[2][1] - this->mat[1][1] * this->mat[2][0]);

        return a + b - c;
    }
}  // namespace Math