#include "DirectX12/DirectX12Math.h"

#include <memory>

namespace DirectX12
{
    /// <summary>
    /// Z軸をLockAt先へ回転する回転行列を出力
    /// </summary>
    /// <param name="in_lockat"></param>
    /// <param name="in_up"></param>
    /// <param name="in_right"></param>
    /// <returns></returns>
    DirectX::XMMATRIX LockAtMatrixByZAxis(
        const DirectX::XMVECTOR& in_lockat,
        const DirectX::XMFLOAT3& in_up,
        const DirectX::XMFLOAT3& in_right)
    {
        // 向かせたいZ方向ベクトル
        DirectX::XMVECTOR vz = in_lockat;

        // 回転の座標軸の仮のy軸
        DirectX::XMVECTOR vy = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&in_up));

        // 回転の座標軸のx軸を向かせたいzベクトルと仮のy軸の外積計算で求める
        DirectX::XMVECTOR vx = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vy, vz));

        // 仮のy軸から本番のy軸を外積計算で求める
        vy = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vz, vx));

        // 計算して出来た回転座標軸のyとz軸が同じ方向だったらrightベクトルを使って再計算する
        if (std::abs(DirectX::XMVector3Dot(vy, vz).m128_f32[0]) == 1.0f)
        {
            // 回転の座標軸の仮のx軸
            vx = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&in_right));

            // 回転の座標軸のy軸を向かせたいzベクトルと仮のy軸の外積計算で求める
            vy = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vz, vx));

            // 仮のy軸から本番のy軸を外積計算で求める
            vx = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vy, vz));
        }

        DirectX::XMMATRIX lockat_mat = DirectX::XMMatrixIdentity();
        lockat_mat.r[0] = vx;
        lockat_mat.r[1] = vy;
        lockat_mat.r[2] = vz;

        return lockat_mat;
    }

    /// <summary>
    /// 特定ベクトルをLockAt先へ回転する回転行列を出力
    /// TODO: 製作中
    /// </summary>
    /// <param name="in_lockat"></param>
    /// <param name="in_up"></param>
    /// <param name="in_right"></param>
    /// <returns></returns>
    DirectX::XMMATRIX LockAtMatrix(
        const DirectX::XMVECTOR& in_org,
        const DirectX::XMVECTOR& in_lockat,
        const DirectX::XMFLOAT3& in_up, const DirectX::XMFLOAT3& in_right)
    {
        // 1: Z軸ベクトルを特定方向に回転
        // 2: Z軸ベクトルをLockAtに回転
        // 3: 2の回転に1の逆回転を加える
        // Z軸を特定ベクトルへ逆回転してLockAtへ回転させる事で特定ベクトルがLockAtへ回転する行列が求められる
        return DirectX::XMMatrixTranspose(LockAtMatrixByZAxis(in_org, in_up, in_right))
            * LockAtMatrixByZAxis(in_lockat, in_up, in_right);
    }
}
