#include "Path.h"

#ifdef _WIN
#include <pathcch.h>
#endif

namespace Core
{
    namespace File
    {
        void Path::operator=(const Path& in_path)
        {
            this->_path = in_path._path;
        }

        Path& Path::operator+=(const Path& in_path)
        {
#ifdef _WIN
            Char tmp[512] = {};

            // WindowsAPIを利用
            // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
            ::PathCchCombine(tmp, 512, this->_path.Str(), in_path._path.Str());
            this->_path = tmp;
#else
            E_ASSERT(0);
#endif
            return *this;
        }

        void Path::_Append(const Char* in_pPath)
        {
            if (this->_path.Empty())
            {
                this->_Set(in_pPath);
            }
            else
            {
#ifdef _WIN
                Char tmp[512] = {};

                // WindowsAPIを利用
                // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
                ::PathCchCombine(tmp, 512, this->_path.Str(), in_pPath);
                this->_path = tmp;
#else
                E_ASSERT(0);
#endif
            }
        }
    }  // namespace File
}  // namespace Core
