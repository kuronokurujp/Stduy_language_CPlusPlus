#pragma once

#include "Core/Core.h"
#include "Core/Common/Handle.h"
#include "Core/Common/Singleton.h"
#include "Core/Common/FixArray.h"

namespace Module
{
    /// <summary>
    /// モジュールのインターフェイス
    /// </summary>
    class ModuleInterface
    {
    public:
        /// <summary>
        /// モジュールの開始と終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool Init() = 0;
        virtual const Bool End() = 0;

        virtual const Bool Update(const Float32 in_deltaTime) = 0;
    };

    /// <summary>
    /// 外部モジュールの基本クラス
    /// </summary>
    template<class T>
    class ModuleBase : public Core::Common::Singleton<T>, public ModuleInterface
    {
    public:
        /// <summary>
        /// モジュールの開始と終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool Init() override { return FALSE; }
        virtual const Bool End() override { return FALSE; }

        virtual const Bool Update(const Float32 in_deltaTime) override { return FALSE; }
    };

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager : public Core::Common::Singleton<ModuleManager>
    {
    public:
        /// <summary>
        /// 初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init();

        /// <summary>
        /// 終了
        /// </summary>
        void End();

        /// <summary>
        /// TODO: モジュール更新
        /// </summary>
        void Update(const Float32 in_deltaTime);

        /// <summary>
        /// モジュールを追加する
        /// 追加と同時にモジュールのインスタンスを生成
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <returns></returns>
        template<class T>
        T* Add()
        {
            T* module = new T();
            this->_modules.PushBack(reinterpret_cast<Module::ModuleInterface*>(module));

            return module;
        }

        /// <summary>
        /// 追加モジュールの適応
        /// </summary>
        /// <returns></returns>
        const Bool Apply();

    private:
        Core::Common::FastFixArray<Module::ModuleInterface*, 128> _modules;
    };
}

