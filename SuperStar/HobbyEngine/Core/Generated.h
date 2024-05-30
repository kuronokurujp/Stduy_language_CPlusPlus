#pragma once

#include "Core/Common/RTTI.h"

// 基本クラスにRTTIなどの情報を付与するマクロ(ヘッダーとソース用)
#define GENERATED_CLASS_BASE_BODY_HEADER() \
    public: \
        virtual const Core::Common::RTTI& GetRTTI() = 0; \
        static const Core::Common::RTTI CLASS_RTTI;

#define GENERATED_CLASS_BASE_BODY(name) \
    const Core::Common::RTTI name::CLASS_RTTI(E_STR_TEXT(#name));

#define GENERATED_CLASS_BODY_HEADER() \
    public: \
        virtual const Core::Common::RTTI& GetRTTI() override { return CLASS_RTTI; } \
        static const Core::Common::RTTI CLASS_RTTI;

#define GENERATED_CLASS_BODY(name, parentName) \
    const Core::Common::RTTI name::CLASS_RTTI(E_STR_TEXT(#name), parentName::CLASS_RTTI);

