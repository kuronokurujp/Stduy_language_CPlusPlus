// ゲーム共通利用するロジック
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <vector>

// 安全にメモリ開放
#define SAFETY_MEM_RELEASE(_x_) { if ((_x_) != NULL) { delete _x_; _x_ = NULL; } }
#define SAFETY_MEM_ARRAY_RELEASE(_x_) { if ((_x_) != NULL) { delete[] _x_; _x_ = NULL; } }

// 静的配列の要素数取得
template<typename T, unsigned int SIZE>
unsigned int StaticSingleArrayLength(const T(&)[SIZE])
{
	return SIZE;
}

/// <summary>
/// vectorで登録している要素を安全に外す.
/// </summary>
template <class T>
static bool SafetyRemoveObjects(T *in_pObject, std::vector<T *> &in_rObjects)
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

#endif // __COMMON_H__
