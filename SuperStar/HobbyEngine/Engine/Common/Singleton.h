#pragma once

#include "Engine/Core.h"

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
            HE_CLASS_COPY_CONSTRUCT_NG(Singleton);
            HE_CLASS_MOVE_CONSTRUCT_NG(Singleton);

        public:
            Singleton()
            {
                HE_ASSERT((_tpInstance == NULL) && "インスタンス二重生成");
                _tpInstance = static_cast<T*>(this);
            }

            virtual ~Singleton()
            {
                if (_tpInstance)
                {
                    if (_tpInstance->Release() == FALSE) HE_ASSERT(FALSE && "リリース失敗");

                    _tpInstance = NULL;
                }
                else
                {
                    HE_PG_LOG_LINE(HE_STR_TEXT("インスタンス二重破棄"));
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
            static T& I() HE_NOEXCEPT
            {
                HE_ASSERT(_tpInstance && "シングルトンのインスタンスがないので参照している");
                return *_tpInstance;
            }

            /// <summary>
            /// インスタンスがあるかどうか
            /// </summary>
            /// <returns></returns>
            static const Bool Exist() HE_NOEXCEPT { return (_tpInstance != NULL); }

            /// <summary>
            /// シングルトン対象から解放
            /// </summary>
            static void Reset()
            {
                HE_ASSERT(_tpInstance);
                if (_tpInstance->Release() == FALSE) HE_ASSERT(FALSE && "リリース失敗");
                _tpInstance = NULL;
            }

        private:
            // ユニークなインスタンス
            static inline T* _tpInstance = NULL;
        };

    }  // namespace Common
};     // namespace Core
