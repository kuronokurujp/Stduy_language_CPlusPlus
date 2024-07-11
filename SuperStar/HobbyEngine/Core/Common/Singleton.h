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
        template <class T>
        class Singleton
        {
            E_CLASS_COPY_CONSTRUCT_NG(Singleton);
            E_CLASS_MOVE_CONSTRUCT_NG(Singleton);

        public:
            Singleton()
            {
                E_ASSERT((_pInstance == NULL) && "インスタンス二重生成");
                _pInstance = static_cast<T*>(this);
            }

            virtual ~Singleton()
            {
                if (_pInstance)
                {
                    if (_pInstance->Release() == FALSE) E_ASSERT(FALSE && "リリース失敗");

                    _pInstance = NULL;
                }
                else
                {
                    E_PG_LOG_LINE(E_STR_TEXT("インスタンス二重破棄"));
                }
            }

            /// <summary>
            /// インスタンスがリリースされたときに呼ばれる
            /// 終了処理だが, 大抵はアプリ終了時に呼ばれる
            /// </summary>
            virtual const Bool Release() { return FALSE; }

            /// <summary>
            /// インスタンスの参照を返す
            /// </summary>
            /// <returns></returns>
            static T& I() E_NOEXCEPT
            {
                E_ASSERT(_pInstance && "シングルトンのインスタンスがないので参照している");
                return *_pInstance;
            }

            /// <summary>
            /// インスタンスがあるかどうか
            /// </summary>
            /// <returns></returns>
            static const Bool Exist() E_NOEXCEPT { return (_pInstance != NULL); }

            /// <summary>
            /// シングルトン対象から解放
            /// </summary>
            static void Reset()
            {
                E_ASSERT(_pInstance);
                if (_pInstance->Release() == FALSE) E_ASSERT(FALSE && "リリース失敗");
                _pInstance = NULL;
            }

        private:
            // ユニークなインスタンス
            static inline T* _pInstance = NULL;
        };

    }  // namespace Common
};     // namespace Core
