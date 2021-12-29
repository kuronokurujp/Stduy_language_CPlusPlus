#include "MMD/VMDLoader.h"

#include "Common.h"

// MMDのVMDファイルローダー
namespace VMD
{
    namespace Loader
    {
        /// <summary>
        /// VMDファイルからデータロード
        /// </summary>
        /// <returns></returns>
        errno_t SyncLoadFile(
            VMDDataPack* out_p_data_pack,
            const char* in_p_vmd_filepath)
        {
            LOGD << "start vmd file load: " << in_p_vmd_filepath;
            errno_t error = 0;

            FILE* fp = nullptr;
            // ファイルを開く
            error = fopen_s(&fp, in_p_vmd_filepath, "rb");
            if (error != 0)
                return error;

            // ヘッダー部分をスキップ
            // データ先頭から50byteスキップ
            fseek(fp, 50, SEEK_SET);

            UINT32 motion_data_num = 0;
            error = 1;
            // モーションデータ数を取得
            {
                if (fread(&motion_data_num, sizeof(motion_data_num), 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
                LOGD << "vmd motion data num => " << motion_data_num;
            }

            // モーションデータ取得
            error = 2;
            auto& motions = out_p_data_pack->motions;
            {
                motions.resize(motion_data_num);

                size_t read_size =
                    // アニメフレーム番号
                    sizeof(motions[0].frame_no)
                    // 位置
                    + sizeof(motions[0].location)
                    // 回転のクォータニオン
                    + sizeof(motions[0].quaternion)
                    // ベジェ補間パラメータ
                    + sizeof(motions[0].bezier);

                for (auto& motion : motions)
                {
                    // パディング対策
                    // パディングとなる箇所をスキップして読み込みしている
                    // しかしこれでは構造体の要素が変わるたびに修正が必要になる
                    // 最初はpack(1)でアライメントなしにして一括読み込みにして
                    // 構造体が決まったらパディングスキップでの読み込みに切り替えるのがいい
                    if (fread(motion.bone_name, sizeof(motion.bone_name), 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }

                    if (fread(&motion.frame_no, read_size, 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }

#ifdef _DEBUG
                    LOGD << " - read bone info -";
                    LOGD << " bone_name: " << motion.bone_name;
                    LOGD << " frame_no: " << motion.frame_no;
                    LOGD << " location(" << motion.location.x << "," << motion.location.y << "," << motion.location.z << ")";
                    LOGD << " quaternion(" << motion.quaternion.x << "," << motion.quaternion.y << "," << motion.quaternion.z << motion.quaternion.w << ")";
                    LOGD << " bezier: " << motion.bezier;
                    LOGD << " ----------------- ";
#endif
                }
            }

            // ファイルを閉じる
            fclose(fp);

            LOGD << "end vmd file load: " << in_p_vmd_filepath;
            return error;
        }
    }
}
