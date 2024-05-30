#pragma once

// VSコンパイラーのマクロ
#ifdef _MSC_VER

// Debug/Releaseの切り替えマクロ
#ifdef _DEBUG

#define _HOBBY_ENGINE_DEBUG

#else

#define _ENGINE_RELEASE

#endif

#endif
