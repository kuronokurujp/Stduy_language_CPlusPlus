#pragma once

#include "Core/Common/FixArray.h"
#include "Core/Common/Handle.h"
#include "Core/Common/Singleton.h"
#include "Core/Core.h"

namespace Module
{
    // 前方宣言
    class ModuleInterface;

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager : public Core::Common::Singleton<ModuleManager>
    {
        friend class ModuleInterface;

    public:
        /// <summary>
        /// 解放
        /// </summary>
        void Release();

        /// <summary>
        /// モジュール更新
        /// </summary>
        void Update(const Float32 in_deltaTime);

        /// <summary>
        /// 追加モジュールの適応
        /// </summary>
        /// <returns></returns>
        const Bool Apply();

    private:
        /// <summary>
        /// モジュール登録
        /// </summary>
        /// <param name="in_pModule"></param>
        void _Add(Module::ModuleInterface* in_pModule);

    private:
        Core::Common::FastFixArray<Module::ModuleInterface*, 128> _modules;
    };

    /// <summary>
    /// モジュールのインターフェイス
    /// </summary>
    class ModuleInterface
    {
        E_CLASS_COPY_CONSTRUCT_NG(ModuleInterface);
        E_CLASS_MOVE_CONSTRUCT_NG(ModuleInterface);

    public:
        ModuleInterface();

        /// <summary>
        /// モジュールの開始と終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool Init() = 0;
        virtual const Bool End()  = 0;

        virtual const Bool Update(const Float32 in_deltaTime) = 0;
    };

    /// <summary>
    /// 外部モジュールの基本クラス
    /// </summary>
    template <class T>
    class ModuleBase : public Core::Common::Singleton<T>, public ModuleInterface
    {
        E_CLASS_COPY_CONSTRUCT_NG(ModuleBase);
        E_CLASS_MOVE_CONSTRUCT_NG(ModuleBase);

    public:
        ModuleBase() : Core::Common::Singleton<T>(), ModuleInterface() {}

        /// <summary>
        /// モジュールの開始と終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool Init() override { return FALSE; }
        virtual const Bool End() override { return FALSE; }

        virtual const Bool Update(const Float32 in_deltaTime) override { return FALSE; }
    };
}  // namespace Module

// モジュール宣言マクロ
// モジュールのhファイルの末尾に宣言する
// _name_にはModuleBaseクラスを継承したクラス名,
// namespace内でこのマクロは使ってはいけない
#define MODULE_GENRATE_DECLARATION(_type_, _name_) \
    extern _type_ s_global_module_##_name_;        \
    extern _type_& Module##_name_()

// モジュール定義マクロ
// モジュールのcppファイルの先頭につける
#define MODULE_GENRATE_DEFINITION(_type_, _name_) \
    _type_& Module##_name_()                      \
    {                                             \
        return s_global_module_##_name_;          \
    }                                             \
    static _type_ s_global_module_##_name_

// モジュール管理の定義マクロ
// モジュール管理を使う前に呼び出す
#define MODULE_MANAGER_DEFINITION               \
    if (Module::ModuleManager::Have() == FALSE) \
    {                                           \
        static Module::ModuleManager manager;   \
    }
