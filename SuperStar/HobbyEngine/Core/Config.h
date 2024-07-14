#pragma once

// VSコンパイラーのマクロ
#ifdef _MSC_VER

// Debug/Releaseの切り替えマクロ
#ifdef _DEBUG

#define HE_ENGINE_DEBUG

#else

#define HE_ENGINE_RELEASE

#endif

#endif
