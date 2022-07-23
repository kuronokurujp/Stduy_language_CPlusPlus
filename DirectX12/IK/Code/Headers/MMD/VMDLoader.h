#pragma once

#include <stdio.h>
#include <vector>
// UINT32など型定義
#include <basetsd.h>

#include <DirectXMath.h>

// MMDのVMDファイルローダー
namespace VMD
{
    namespace Loader
    {
        /// <summary>
        /// モーションデータ
        /// </summary>
        struct VMDMotionData
        {
            // ボーン名
            char bone_name[15];
            // アニメフレーム番号
            UINT32  frame_no;
            // 位置
            DirectX::XMFLOAT3 location;
            // 回転のクォータニオン
            DirectX::XMFLOAT4 quaternion;
            // ベジェ補間パラメータ
            UINT8 bezier[64];
        };

        /// <summary>
        /// ロードしたVMDデータパック
        /// </summary>
        struct VMDDataPack
        {
            std::vector<VMDMotionData> motions;
        };

        /// <summary>
        /// VMDファイルからデータロード
        /// </summary>
        /// <returns></returns>
        extern errno_t LoadFileBySync(
            VMDDataPack* out_p_data_pack,
            const char* in_p_vmd_filepath);
    }
}
