#include "PMD/PMDLoader.h"

#include "Common.h"

namespace PMD
{
    namespace Loader
    {
        /// <summary>
        /// PMDファイルからデータ同期ロード
        /// </summary>
        /// <param name="out_p_header"></param>
        /// <param name="in_p_pmd_filepath"></param>
        /// <returns></returns>
        errno_t SyncLoadFile(
            PMDDataPack* out_p_data_pack,
            std::vector<PMDMaterial>* out_p_material_datas,
            std::vector<PMDBone>* out_p_bone_datas,
            const char* in_p_pmd_filepath)
        {
            LOGD << "start load pmd file: " << in_p_pmd_filepath;

            FILE* fp = nullptr;
            auto error = fopen_s(&fp, in_p_pmd_filepath, "rb");
            if (error != 0)
                return error;

            // シグネチャーロード
            error = 1;
            {
                char signature[3] = {};
                if (fread(signature, sizeof(signature), 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
            }

            // TODO: シグネチャー名が正しいかのチェックは必要かな

            // ヘッダーをロード
            error = 2;
            {
                if (fread(&out_p_data_pack->header, sizeof(PMDHeader), 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
            }

            // 頂点データ群をロード
            error = 3;
            {
                constexpr size_t pmdvertex_size = 38;
                out_p_data_pack->vertex_size = pmdvertex_size;

                // 頂点数
                {
                    UINT32 vert_num = 0;
                    if (fread(&vert_num, sizeof(vert_num), 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }

                    out_p_data_pack->vert_num = vert_num;

                    // 頂点データ一覧を取得
                    out_p_data_pack->vertexs.resize(vert_num * pmdvertex_size);

                    auto size = out_p_data_pack->vertexs.size();
                    auto data_size = sizeof(out_p_data_pack->vertexs[0]);

                    if (fread(out_p_data_pack->vertexs.data(), size * data_size, 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }
                }

                {
                    // インデックス数
                    UINT32 indices_num = 0;
                    if (fread(&indices_num, sizeof(indices_num), 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }
                    out_p_data_pack->indices.resize(indices_num);

                    auto size = out_p_data_pack->indices.size();
                    auto data_size = sizeof(out_p_data_pack->indices[0]);

                    // インデックスデータ一覧を取得
                    if (fread(out_p_data_pack->indices.data(), size * data_size, 1, fp) == 0)
                    {
                        fclose(fp);
                        return error;
                    }
                }
            }

            // マテリアルデータ確保
            error = 4;
            {
                // マテリアルの数を取得
                UINT32 material_num = 0;
                if (fread(&material_num, sizeof(material_num), 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }

                // マテリアルの数分データを確保
                out_p_material_datas->resize(material_num);
                // 要素のデータサイズを取得
                auto data_size = sizeof((*out_p_material_datas)[0]);
                auto size = out_p_material_datas->size();

                if (fread(out_p_material_datas->data(), data_size * size, 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
            }

            // ボーンをロード
            error = 5;
            {
                UINT16 bone_num = 0;
                if (fread(&bone_num, sizeof(bone_num), 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
                LOGD << "bone_num: " << bone_num;

                // ボーン数分データを確保
                out_p_bone_datas->resize(bone_num);
                // 要素のデータサイズを取得
                auto data_size = sizeof((*out_p_bone_datas)[0]);
                auto size = out_p_bone_datas->size();

                if (fread(out_p_bone_datas->data(), data_size * size, 1, fp) == 0)
                {
                    fclose(fp);
                    return error;
                }
#ifdef _DEBUG
                // デバッグのため読み込んだボーン情報を出力
                for (auto& bone : *out_p_bone_datas)
                {
                    // 日本語文字列をデバッグ出力するためwstring型に変更
                    auto bone_name = Common::GetWideStringFromString(bone.bone_name);
                    LOGD << bone_name;
                }
#endif
            }

            fclose(fp);

            LOGD << "end succees load pmd file: " << in_p_pmd_filepath;

            error = 0;
            return error;
        }
    }
}
