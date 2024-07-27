#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomString.h"
#include "Engine/Common/Handle.h"
#include "Engine/Common/Singleton.h"
#include "Engine/Core.h"

namespace Platform
{
    class PlatformModule;
}

namespace Module
{
    /// <summary>
    /// 外部モジュールの基本クラス
    /// </summary>
    class ModuleBase
    {
        HE_CLASS_COPY_CONSTRUCT_NG(ModuleBase);
        HE_CLASS_MOVE_CONSTRUCT_NG(ModuleBase);

    public:
        ModuleBase(const Char* in_szName);

        Platform::PlatformModule* GetPlatformModule() const;

        const Char* Name() const HE_NOEXCEPT { return this->_szName.Str(); }

    protected:
        /// <summary>
        /// モジュールの開始
        /// </summary>
        /// <returns></returns>
        virtual const Bool Start() = 0;

        /// <summary>
        /// モジュールの解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() = 0;

        virtual const Bool Update(const Float32 in_fDeltaTime) { return FALSE; }

    private:
        Core::Common::FixString128 _szName;

    private:
        static inline Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 64>
            _mModule;

        friend class ModuleManager;
    };

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager : public Core::Common::Singleton<ModuleManager>
    {
    public:
        ModuleBase* Get(const Char* in_szName);

        /// <summary>
        /// モジュールの利用開始
        /// </summary>
        const Bool Start();

        /// <summary>
        /// 解放
        /// </summary>
        const Bool Release() override final;

        /// <summary>
        /// モジュール更新
        /// </summary>
        void Update(const Float32 in_fDeltaTime);
    };
}  // namespace Module

// モジュール宣言マクロ
// モジュールのhファイルの末尾に宣言する
// _name_にはModuleBaseクラスを継承したクラス名,
// namespace内でこのマクロは使ってはいけない
// ポインタにして弱参照にしている
#define MODULE_GENRATE_DECLARATION(_type_, _name_) extern "C" _type_* Module##_name_();

// モジュール定義マクロ
// モジュールのcppファイルの先頭につける
#define MODULE_GENRATE_DEFINITION(_type_, _name_)                          \
    static _type_ s_global_module_##_name_ = _type_(HE_STR_TEXT(#_name_)); \
    _type_* Module##_name_()                                               \
    {                                                                      \
        return &s_global_module_##_name_;                                  \
    }

// モジュールのインポート
#define MODULE_IMPORT(_name_)      \
    do                             \
    {                              \
        auto m = Module##_name_(); \
    } while (0);
