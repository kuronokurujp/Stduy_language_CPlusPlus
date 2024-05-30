#pragma once

#include "Core/Core.h"

namespace Core
{
    namespace Common
    {
        /// <summary>
        /// シングルトンテンプレート
        /// クラスのインスタンスを複数生成したくないクラスの防止用に使える
        /// インスタンスを複数生成している場合はエラーにしている
        /// 複数生成している場合は使い方が間違っているので使っている側が複数生成しないように修正する
        /// </summary>
        /// <typeparam name="T"></typeparam>
        template <typename T>
        class Singleton
        {
        public:
            Singleton()
            {
                E_ASSERT(!_pInstance && "インスタンス二重生成");
                _pInstance = static_cast<T*>(this);
            }

            Singleton(const Singleton& singleton)
            {
                //	コピーコンストラクタは使用禁止
                E_STATIC_ASSERT(0);
            };

            ~Singleton()
            {
                E_ASSERT(_pInstance && "インスタンス二重破棄");
                _pInstance = NULL;
            }

            /// <summary>
            /// インスタンスの参照を返す
            /// </summary>
            /// <returns></returns>
            static T& I()
            {
                E_ASSERT(_pInstance);
                return *_pInstance;
            }

            /// <summary>
            /// インスタンスがあるかどうか
            /// </summary>
            /// <returns></returns>
            static const Bool Have()
            {
                return (_pInstance != NULL);
            }

        private:
            static T* _pInstance;
        };

        // static変数
        template <typename T>
        T* Singleton <T>::_pInstance = NULL;
    }
};

