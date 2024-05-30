﻿#pragma once

// 乱数処理の便利処理をまとめたコード

#include <random>

// Coreディレクトリ管理なので相対パスで指定可能
#include "Core/Common/Singleton.h"

// #include "Math/vector2.h"

namespace Core
{
    namespace Math
    {
        /// <summary>
        /// 乱数関連の処理をまとめた管理クラス
        /// </summary>
        class RandomManager : public Core::Common::Singleton<RandomManager>
        {
        public:
            /// <summary>
            /// 利用する前に必ず最初に呼び出す
            /// </summary>
            /// <returns></returns>
            const Bool Init(const Sint32 in_seed);

            /// <summary>
            /// Sets the seed.
            /// </summary>
            /// <param name="in_seed">The in seed.</param>
            /// <returns></returns>
            void SetSeed(const Sint32 in_seed);

            /// <summary>
            /// 0 - 1の乱数を取得
            /// </summary>
            /// <returns></returns>
            const Float32 FloatRange0_1();

            /// <summary>
            /// 最小と最大値を指定してその間をランダムで取得
            /// 型を使う側が指定する
            /// </summary>
            /// <param name="in_min"></param>
            /// <param name="in_max"></param>
            /// <returns></returns>
            template<typename T>
            const T Range(const T in_min, const T in_max)
            {
                std::uniform_real_distribution<T> dist(in_min, in_max);
                return dist(this->_generator);
            }

            /// <summary>
            /// Gets the vector2.
            /// </summary>
            /// <param name="in_rMin">The in r minimum.</param>
            /// <param name="in_rMax">The in r maximum.</param>
            /// <returns></returns>
            // const Vector2 GetVector2(const Vector2& in_rMin, const Vector2& in_rMax);

        private:
            std::mt19937 _generator;
        };
    }
}