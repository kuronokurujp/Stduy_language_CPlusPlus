#include "Math/math.h"

namespace Math
{
    /// <summary>
    /// Vector3s the transform.
    /// </summary>
    /// <param name="in_rVec3">The in r vec3.</param>
    /// <param name="in_rQuaternion">The in r quaternion.</param>
    /// <returns></returns>
    auto Vector3Transform(const Vector3& in_rVec3, const Quaternion& in_rQuaternion) -> Vector3
    {
        // todo: ベクトルをクォータニオンで計算
        Vector3 qv(in_rQuaternion.x, in_rQuaternion.y, in_rQuaternion.z);

        Vector3 retV = Vector3();

        // なぜこうなる？
        {
            Vector3 temp(in_rVec3);
            Vector3 temp2 =
                Vector3::Cross(qv, Vector3::Cross(qv, in_rVec3) + temp * in_rQuaternion.w);
            // 2.0fを-2.0fにすると90度に回転したベクトルになる
            // 反射ベクトルに使えそう
            auto vec3 = temp2 * 2.0f;

            retV.SetAdd(in_rVec3, vec3);
        }

        return retV;
    }

    /// <summary>
    /// Matrix4s the form quaternion.
    /// </summary>
    /// <param name="in_rQuaternion">The in r quaternion.</param>
    /// <returns></returns>
    auto Matrix4FormQuaternion(const Quaternion& in_rQuaternion) -> Matrix4
    {
        Matrix4 mat4;
        auto q = in_rQuaternion;

        // 1行n列
        mat4.mat[0][0] = 1.0f - 2.0f * q.y * q.y - 2.0f * q.z * q.z;
        mat4.mat[0][1] = 2.0f * q.x * q.y + 2.0f * q.w * q.z;
        mat4.mat[0][2] = 2.0f * q.x * q.z - 2.0f * q.w * q.y;
        mat4.mat[0][3] = 0.0f;

        // 2行n列
        mat4.mat[1][0] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
        mat4.mat[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
        mat4.mat[1][2] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;
        mat4.mat[1][3] = 0.0f;

        // 3行n列
        mat4.mat[2][0] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
        mat4.mat[2][1] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
        mat4.mat[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;
        mat4.mat[2][3] = 0.0f;

        // 4行n列
        mat4.mat[3][0] = 0;
        mat4.mat[3][1] = 0;
        mat4.mat[3][2] = 0;
        mat4.mat[3][3] = 1.0f;

        return mat4;
    }
}  // namespace Math