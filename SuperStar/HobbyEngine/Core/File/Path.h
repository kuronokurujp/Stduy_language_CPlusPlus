#pragma once

#include "Core/Core.h"
#include "Core/Common/FixString.h"

// TODO: ファイルのパス制御ができる
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

            Path& operator+=(const Path& in_path);

            const Char* Str() { return this->_path.Str(); }

        private:
            Core::Common::FixString256 _path;
        };
    }
}

