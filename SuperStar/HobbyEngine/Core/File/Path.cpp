#include "Path.h"

#ifdef _WIN
#include <pathcch.h>
#endif

namespace Core
{
    namespace File
    {
        Path& Path::operator+=(const Path& in_path)
        {
#ifdef _WIN
            Char tmp[512] = {};

            // WindowsAPIを利用
            // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
            ::PathCchCombine(tmp, 512, this->_path.Str(), in_path._path.Str());
            this->_path = tmp;
#endif
            return *this;
        }
    }
}
