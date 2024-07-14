#include "Path.h"

#ifdef HE_WIN
#include <pathcch.h>
#endif

namespace Core
{
    namespace File
    {
        void Path::operator=(const Path& in_rPath)
        {
            this->_szPath = in_rPath._szPath;
        }

        Path& Path::operator+=(const Path& in_rPath)
        {
#ifdef HE_WIN
            Char szTmp[512] = {};

            // WindowsAPIを利用
            // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
            ::PathCchCombine(szTmp, 512, this->_szPath.Str(), in_rPath._szPath.Str());
            this->_szPath = szTmp;
#else
            E_ASSERT(0);
#endif
            return *this;
        }

        void Path::_Append(const Char* in_szPath)
        {
            if (this->_szPath.Empty())
            {
                this->_Set(in_szPath);
            }
            else
            {
#ifdef HE_WIN
                Char szTmp[512] = {};

                // WindowsAPIを利用
                // https://learn.microsoft.com/ja-jp/windows/win32/api/pathcch/nf-pathcch-pathcchcombine
                ::PathCchCombine(szTmp, 512, this->_szPath.Str(), in_szPath);
                this->_szPath = szTmp;
#else
                E_ASSERT(0);
#endif
            }
        }
    }  // namespace File
}  // namespace Core
