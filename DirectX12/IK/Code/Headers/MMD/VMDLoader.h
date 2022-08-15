#pragma once

#include <stdio.h>
#include <vector>
#include <unordered_map>

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
            UINT32 frame_no;
            // 位置
            DirectX::XMFLOAT3 location;
            // 回転のクォータニオン
            DirectX::XMFLOAT4 quaternion;
            // ベジェ補間パラメータ
            UINT8 bezier[64];
        };

        // 表情データ(23byte)
#pragma pack(1)
        struct VMDMorph
        {
            char name[15];
            UINT32 frame_no;
            float weight;
        };
#pragma pack()

        // カメラデータ(61byte)
#pragma pack(1)
        struct VMDCamera
        {
            UINT32 frame_no;
            float distance;
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT4 euler_angle;
            UINT8 inter_polation[24];
            UINT32 fov;
            UINT8 pers_flg;
        };
#pragma pack()

        // 照明データ
        struct VMDLight
        {
            UINT32 frame_no;
            DirectX::XMFLOAT3 rgb;
            DirectX::XMFLOAT3 vec;
        };

        // 影データ
#pragma pack(1)
        struct VMDSelfShadow
        {
            UINT32 frame_no;
            UINT8 mode;
            float distance;
        };
#pragma pack()

        // IKオン / オフデータ
        struct VMDIkEnable
        {
            UINT32 frame_no;
            std::unordered_map<std::string, bool> ik_enable_table;
        };

        /// <summary>
        /// ロードしたVMDデータパック
        /// </summary>
        struct VMDDataPack
        {
            std::vector<VMDMotionData> motions;
            std::vector<VMDIkEnable> ik_enables;
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
