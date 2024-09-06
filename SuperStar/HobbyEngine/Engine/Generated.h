#pragma once

#include "Engine/Common/RTTI.h"

// RTTIを付与する基本クラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BASE_BODY_HEADER(name)            \
public:                                                      \
    virtual inline const Core::Common::RTTI& VGetRTTI() = 0; \
                                                             \
public:                                                      \
    static inline const Core::Common::RTTI CLASS_RTTI = Core::Common::RTTI(HE_STR_TEXT(#name));

// RTTIを付与した基本クラスを継承したクラスの宣言にRTTI情報を付与するマクロ
// nameにはクラス型を指定
#define HE_GENERATED_CLASS_BODY_HEADER(name, parentName)         \
public:                                                          \
    virtual inline const Core::Common::RTTI& VGetRTTI() override \
    {                                                            \
        return CLASS_RTTI;                                       \
    }                                                            \
                                                                 \
public:                                                          \
    static inline const Core::Common::RTTI CLASS_RTTI =          \
        Core::Common::RTTI(HE_STR_TEXT(#name), parentName::CLASS_RTTI);

#define HE_GENERATED_CHECK_RTTI(rtti, checkRtti) \
    ((rtti).VGetRTTI().DerivesFrom(&checkRtti::CLASS_RTTI))
