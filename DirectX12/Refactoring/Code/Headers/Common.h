#pragma once

// 外部ライブラリ「plog」導入していないとエラーになるので注意
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <stdio.h>
#include <string>
#include <vector>

// 安全にメモリ開放
#define SAFETY_MEM_RELEASE(_x_) { if ((_x_) != nullptr) { delete _x_; _x_ = nullptr; } }
#define SAFETY_MEM_ARRAY_RELEASE(_x_) { if ((_x_) != nullptr) { delete[] _x_; _x_ = nullptr; } }

namespace Common
{
    /// <summary>
    /// vectorで登録している要素を安全に外す.
    /// </summary>
    template <class T>
    static bool SafetyRemoveObjects(T* in_pObject, std::vector<T*>& in_rObjects)
    {
        // 解除するObjectがキャッシュリストに含まれているかチェック
        auto iter = std::find(in_rObjects.begin(), in_rObjects.end(), in_pObject);
        if (iter != in_rObjects.end())
        {
            // キャッシュリストから引数のObjectを外す
            // 引数のObjectをリスト最後にする
            // そして最後のアイテムを開放することで外している
            std::iter_swap(iter, in_rObjects.end() - 1);
            in_rObjects.pop_back();

            return true;
        }

        return false;
    }

    /// <summary>
    /// ファイル名から拡張子を取得
    /// </summary>
    /// <param name="in_r_filename"></param>
    /// <returns></returns>
    static std::string GetFileExtension(const std::string& in_r_filename)
    {
        auto idx = in_r_filename.rfind('.');
        return in_r_filename.substr(idx + 1, in_r_filename.length() - idx - 1);
    }

    /// <summary>
    /// 文字列内に区切り文字があった場合に2つの文字列に分割する
    /// 区切り文字が1つしかない想定なので注意
    /// </summary>
    /// <param name="in_r_filename"></param>
    /// <param name="in_spliter"></param>
    /// <returns></returns>
    static std::pair<std::string, std::string> SplitFileName(const std::string& in_r_filename, const char in_spliter = '*')
    {
        int idx = in_r_filename.find(in_spliter);

        auto first = in_r_filename.substr(0, idx);
        auto second = in_r_filename.substr(idx + 1, in_r_filename.length() - idx - 1);

        return std::pair<std::string, std::string>(first, second);
    }

    /// <summary>
    /// stringの文字列からwstringの文字列に変換したのを取得
    /// </summary>
    /// <param name="in_r_str"></param>
    /// <returns></returns>
    static std::wstring GetWideStringFromString(const std::string& in_r_str)
    {
        // wstringの文字列数を取得
        auto num_1 = MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            in_r_str.c_str(),
            -1,
            nullptr,
            0);

        // 文字列数からwstringの文字列を作成
        std::wstring conv_string;
        conv_string.resize(num_1);

        // string to wstringにする
        auto num_2 = MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
            in_r_str.c_str(),
            -1,
            &conv_string[0],
            num_1);

        assert(num_1 == num_2);

        return conv_string;
    }

    /// <summary>
    /// ディレクトリパスの末尾にパスを追加したパスを返す
    /// 指定パスをディレクトリパスに変換
    /// そのディレクトリパスの末尾に引数で指定したパスを追加
    /// </summary>
    /// <param name="in_currnet_path"></param>
    /// <param name="in_p_append_path"></param>
    /// <returns></returns>
    static std::string GetPathForAddPathToDirectoryEndPath(
        const std::string& in_currnet_path,
        const char* in_p_append_path)
    {
        // windowsだとパス区切りが/ or \と2種類あるのでどちらでも対応するようにしている
        INT path_index1 = in_currnet_path.rfind('/');
        INT path_index2 = in_currnet_path.rfind('\\');

        auto path_index = max(path_index1, path_index2);

        // 区切りの文字は残す
        auto dir_path = in_currnet_path.substr(0, path_index + 1);
        // パス同士の結合
        return dir_path + in_p_append_path;
    }

    /// <summary>
    /// GUID文字列を生成
    /// </summary>
    /// <returns></returns>
    static std::string CreateGUIDString()
    {
        GUID gid_ref;
        // WindowAPIを利用
        HRESULT h_result = CoCreateGuid(&gid_ref);
        assert(SUCCEEDED(h_result));

        return std::string(std::to_string(gid_ref.Data1));
    }
}
