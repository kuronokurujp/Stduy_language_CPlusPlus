#ifndef __MATH__MATRIX3_H__
#define __MATH__MATRIX3_H__

namespace Math
{
    class Matrix3
    {
    public:
        float mat[3][3];

        Matrix3();

        /// <summary>
        /// 行列式を取得.
        /// </summary>
        /// <returns></returns>
        float GetDet();
    };
}  // namespace Math

#endif  // __MATH__MATRIX3_H__
