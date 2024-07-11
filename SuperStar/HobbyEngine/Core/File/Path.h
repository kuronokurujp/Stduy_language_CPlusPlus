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
            Path(const Char* in_pPath) { this->_Set(in_pPath); }

            /// <summary>
            /// パス文字列を複数設定
            /// </summary>
            /// <typeparam name="...Args"></typeparam>
            /// <param name="...in_args"></param>
            template <typename... Args>
            Path(Args... in_args)
            {
                (this->_Append(in_args), ...);
            }

#ifdef _WIN
            Path(const char* in_pPath) : _path(in_pPath) {}
#endif

            void operator=(const Path& in_path);
            Path& operator+=(const Path& in_path);

            inline const Char* Str() const { return this->_path.Str(); }
            inline const Bool Empty() const { return this->_path.Empty(); }

        private:
            inline void _Set(const Char* in_pPath) E_NOEXCEPT { this->_path = in_pPath; }
            void _Append(const Char* in_pPath);

        private:
            Core::Common::FixString256 _path;
        };

    }  // namespace File
}  // namespace Core
