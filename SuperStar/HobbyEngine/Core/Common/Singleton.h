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
            E_CLASS_COPY_CONSTRUCT_NG(Singleton);
            E_CLASS_MOVE_CONSTRUCT_NG(Singleton);

        public:
            Singleton()
            {
                E_ASSERT(!_pInstance && "インスタンス二重生成");
                _pInstance = static_cast<T*>(this);
            }

            ~Singleton()
            {
                if (_pInstance == NULL)
                    E_PG_LOG_LINE(E_STR_TEXT("インスタンス二重破棄"));
                else
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
            static const Bool Have() { return (_pInstance != NULL); }

            /// <summary>
            /// シングルトン対象から解放
            /// </summary>
            static void Reset()
            {
                E_ASSERT(_pInstance);
                _pInstance = NULL;
            }

        private:
            static T* _pInstance;
        };

        // static変数
        template <typename T>
        T* Singleton<T>::_pInstance = NULL;
    }  // namespace Common
};     // namespace Core
