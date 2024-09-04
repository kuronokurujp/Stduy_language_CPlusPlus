#pragma once

#include "Engine/Core.h"

namespace Core::Common
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
        HE_CLASS_COPY_NG(Singleton);
        HE_CLASS_MOVE_NG(Singleton);

    public:
        Singleton()
        {
            HE_ASSERT((T::_pInstance == NULL) && "インスタンス二重生成");
            T::_pInstance = static_cast<T*>(this);
        }

        virtual ~Singleton()
        {
            if (T::_pInstance)
            {
                if (T::_pInstance->VRelease() == FALSE) HE_ASSERT(FALSE && "リリース失敗");

                T::_pInstance = NULL;
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
        virtual const Bool VRelease() { return FALSE; }

        /// <summary>
        /// インスタンスの参照を返す
        /// </summary>
        /// <returns></returns>
        static T& I() HE_NOEXCEPT
        {
            HE_ASSERT(T::_pInstance && "シングルトンのインスタンスがないので参照している");
            return *T::_pInstance;
        }

        /// <summary>
        /// インスタンスがあるかどうか
        /// </summary>
        /// <returns></returns>
        static const Bool Exist() HE_NOEXCEPT { return (T::_pInstance != NULL); }

        /// <summary>
        /// シングルトン対象から解放
        /// </summary>
        static void Reset()
        {
            HE_ASSERT(T::_pInstance);
            if (T::_pInstance->VRelease() == FALSE) HE_ASSERT(FALSE && "リリース失敗");
            T::_pInstance = NULL;
        }

    private:
        // ユニークなインスタンス
        static inline T* _pInstance = NULL;
    };

}  // namespace Core::Common
