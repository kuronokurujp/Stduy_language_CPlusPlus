#ifndef __COMMON_H__
#define __COMMON_H__

#include <SDL/SDL.h>

#include <vector>

// 色々な箇所でよく使う機能をまとめる

/// <summary>
/// 色情報
/// </summary>
struct Color32
{
    Uint8 r, g, b, a;
};

// 色のデータ一式
static const Color32 WhiteColor{255, 255, 255, 255};
static const Color32 BlackColor{0, 0, 0, 255};

// 安全にメモリ開放
#define SAFETY_MEM_RELEASE(_x_) \
    {                           \
        if ((_x_) != NULL)      \
        {                       \
            delete _x_;         \
            _x_ = NULL;         \
        }                       \
    }
#define SAFETY_MEM_ARRAY_RELEASE(_x_) \
    {                                 \
        if ((_x_) != NULL)            \
        {                             \
            delete[] _x_;             \
            _x_ = NULL;               \
        }                             \
    }

// デフォルトコンストラクタを封印
#define NOUSE_DEFAULT_CONSTRUCT(_x_) \
private:                             \
    _x_()                            \
    {                                \
    }
// コピーコンストラクタを封印
#define NOUSE_COPY_CONSTRUCT(_x_) \
private:                          \
    _x_(const _x_&)               \
    {                             \
    }

// 静的配列の要素数取得
template <typename T, unsigned int SIZE>
unsigned int StaticSingleArrayLength(const T (&)[SIZE])
{
    return SIZE;
}

// 2次元配列の行と列の要素数取得
#define ARRAY_TWO_DIMENSIONAL_ROW_LENGTH(_x_) sizeof(_x_) / sizeof(_x_[0])
#define ARRAY_TWO_DIMENSIONAL_COL_LENGTH(_x_) sizeof(_x_[0]) / sizeof(_x_[0][0])

/// <summary>
/// Removes the objects.
/// </summary>
/// <param name="in_pObject">The in p object.</param>
/// <param name="in_rObjects">The in r objects.</param>
/// <returns></returns>
template <class T>
static bool RemoveObjects(T* in_pObject, std::vector<T*>& in_rObjects)
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
#endif  // __COMMON_H__