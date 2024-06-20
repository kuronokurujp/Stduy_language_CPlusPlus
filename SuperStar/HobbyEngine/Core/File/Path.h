#pragma once

#include "Core/Common/FixString.h"
#include "Core/Core.h"

// ファイルのパス制御ができる
namespace Core
{
    namespace File
    {
        /// <summary>
        /// パス制御
        /// パスの連結とかできる
        /// </summary>
        class Path
        {
        public:
            Path() {}

            Path(const Char* in_pPath) : _path(in_pPath) {}
#ifdef _WIN
            Path(const char* in_pPath) : _path(in_pPath) {}
#endif

            void operator=(const Path& in_path);
            Path& operator+=(const Path& in_path);

            inline const Char* Str() const { return this->_path.Str(); }
            inline const Bool IsEmpty() const { return this->_path.IsEmpty(); }

        private:
            Core::Common::FixString256 _path;
        };
    }  // namespace File
}  // namespace Core
