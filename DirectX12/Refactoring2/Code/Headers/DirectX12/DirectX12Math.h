#pragma once

#include <DirectXMath.h>

namespace DirectX12
{
    /// <summary>
    /// Z軸をLockAt先へ回転する回転行列を出力
    /// </summary>
    /// <param name="in_lockat"></param>
    /// <param name="in_up"></param>
    /// <param name="in_right"></param>
    /// <returns></returns>
    extern DirectX::XMMATRIX LockAtMatrixByZAxis(const DirectX::XMVECTOR& in_lockat, const DirectX::XMFLOAT3& in_up, const DirectX::XMFLOAT3& in_right);

    /// <summary>
    /// 特定ベクトルをLockAt先へ回転する回転行列を出力
    /// </summary>
    /// <param name="in_lockat"></param>
    /// <param name="in_up"></param>
    /// <param name="in_right"></param>
    /// <returns></returns>
    extern DirectX::XMMATRIX LockAtMatrix(
        const DirectX::XMVECTOR& in_org,
        const DirectX::XMVECTOR& in_lockat,
        const DirectX::XMFLOAT3& in_up,
        const DirectX::XMFLOAT3& in_right);

    static const DirectX::XMFLOAT3 XFloat3UP = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
    static const DirectX::XMFLOAT3 XFloat3Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
    static const DirectX::XMFLOAT3 XFloat3RFront = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
}
