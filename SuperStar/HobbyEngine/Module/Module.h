#pragma once

// #include "Core/Common/FixArray.h"
#include "Core/Common/FixMap.h"
#include "Core/Common/FixString.h"
#include "Core/Common/Handle.h"
#include "Core/Common/Singleton.h"
#include "Core/Core.h"

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
        E_CLASS_COPY_CONSTRUCT_NG(ModuleBase);
        E_CLASS_MOVE_CONSTRUCT_NG(ModuleBase);

    public:
        ModuleBase(const Char* in_pName);

        Platform::PlatformModule* GetPlatformModule() const;

        const Char* Name() const E_NOEXCEPT { return this->_name.Str(); }

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

        virtual const Bool Update(const Float32 in_deltaTime) { return FALSE; }

    private:
        Core::Common::FixString128 _name;

    private:
        static inline Core::Common::FixMap<Core::Common::FixString128, ModuleBase*, 64> _modules;

        friend class ModuleManager;
    };

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager : public Core::Common::Singleton<ModuleManager>
    {
    public:
        ModuleBase* Get(const Char* in_pName);

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
        void Update(const Float32 in_deltaTime);
    };
}  // namespace Module

// モジュール宣言マクロ
// モジュールのhファイルの末尾に宣言する
// _name_にはModuleBaseクラスを継承したクラス名,
// namespace内でこのマクロは使ってはいけない
// ポインタにして弱参照にしている
#define MODULE_GENRATE_DECLARATION(_type_, _name_) \
    extern _type_ s_global_module_##_name_;        \
    extern _type_* Module##_name_();

// モジュール定義マクロ
// モジュールのcppファイルの先頭につける
#define MODULE_GENRATE_DEFINITION(_type_, _name_)                         \
    static _type_ s_global_module_##_name_ = _type_(E_STR_TEXT(#_name_)); \
    _type_* Module##_name_()                                              \
    {                                                                     \
        return &s_global_module_##_name_;                                 \
    }

// モジュール管理の定義マクロ
// モジュール管理を使う前に呼び出す
#define MODULE_MANAGER_DEFINITION                \
    if (Module::ModuleManager::Exist() == FALSE) \
    {                                            \
        static Module::ModuleManager manager;    \
    }
