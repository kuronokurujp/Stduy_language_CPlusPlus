#pragma once

#include "Core/Common/FixString.h"
#include "Core/Core.h"

// ファイルのパス制御ができる
namespace Core
{
    namespace File
    {
        /// <summary>
        /// ファイルパス制御
        /// パスの連結とかできる
        /// </summary>
        class Path
        {
        public:
            /// <summary>
            /// パス未設定
            /// </summary>
            Path() {}

            /// <summary>
            /// パス文字列を一つのみ設定
            /// </summary>
            /// <param name="in_pPath"></param>
            Path(const Char* in_szPath) { this->_Set(in_szPath); }
            // Path(const Core::Common::FixStringBase* in_szrName) { this->_Set(in_szrName->Str());
            // }

            /// <summary>
            /// パス文字列を複数設定
            /// </summary>
            template <typename... Args>
            Path(Args... in_args)
            {
                // TODO: 未対応の型でエラーになった場合, 追跡ができない
                // これ非対応にして変わりのを用意する
                (this->_Append(in_args), ...);
            }

#ifdef HE_WIN
            Path(const UTF8* in_szPathUTF8) : _szPath(in_szPathUTF8) {}
#endif

            void operator=(const Path& in_crPath);
            Path& operator+=(const Path& in_crPath);

            inline const Char* Str() const { return this->_szPath.Str(); }
            inline const Bool Empty() const { return this->_szPath.Empty(); }

        private:
            inline void _Set(const Char* in_szPath) HE_NOEXCEPT { this->_szPath = in_szPath; }
            void _Append(const Char* in_szPath);

        private:
            Core::Common::FixString256 _szPath;
        };

    }  // namespace File
}  // namespace Core
