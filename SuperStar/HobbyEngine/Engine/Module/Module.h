﻿#pragma once

#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Common/Singleton.h"
#include "Engine/MiniEngine.h"

namespace Module
{
    class ModuleBase;

    enum eLayer
    {
        eLayer_App = 0,
        eLayer_Logic,
        eLayer_View,
    };

    /// <summary>
    /// モジュール管理クラス
    /// </summary>
    class ModuleManager : public Core::Common::Singleton<ModuleManager>
    {
    public:
        ModuleBase* Get(const Char* in_szName) const;

        template <class T>
        T* Get() const
        {
            Core::Common::FixString128 szName(T::ModuleName());
            return reinterpret_cast<T*>(this->Get(szName.Str()));
        }

        /// <summary>
        /// ヒープ作成したモジュールを登録
        /// </summary>
        const Bool RegistHeapModule(ModuleBase* in_pModule);

        /// <summary>
        /// モジュールの利用開始
        /// </summary>
        const Bool Start(const eLayer);

        /// <summary>
        /// 解放
        /// </summary>
        const Bool VRelease() override final;

        /// <summary>
        /// モジュール群の前更新
        /// </summary>
        /// <param name="in_fDeltaTime"></param>
        void BeforeUpdate(const Float32 in_fDeltaTime);

        /// <summary>
        /// モジュール群の更新
        /// </summary>
        void Update(const Float32 in_fDeltaTime);

        /// <summary>
        /// モジュール群の後更新
        /// </summary>
        void LateUpdate(const Float32 in_fDeltaTime);

    private:
        const Bool _StartModule(ModuleBase&);
        void _SortModuleVector(Core::Common::VectorBase<ModuleBase*>* out);

    private:
        Core::Common::CustomFixVector<ModuleBase*, 16> _vAppModule;
        Core::Common::CustomFixVector<ModuleBase*, 32> _vLogicModule;
        Core::Common::CustomFixVector<ModuleBase*, 16> _vViewModule;

        Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 16> _mAppModule;
        Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 32> _mLogicModule;
        Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 16> _mViewModule;
    };

    // アプリ層とロジック層とビュー層で種類を分けるべきか
    /// <summary>
    /// 外部モジュールの基本クラス
    /// </summary>
    class ModuleBase
    {
        HE_CLASS_COPY_NG(ModuleBase);
        HE_CLASS_MOVE_NG(ModuleBase);

    public:
        ModuleBase(const char* in_szName, eLayer const in_eLayer = eLayer::eLayer_Logic,
                   const Sint32 in_prioriry = 0);
        virtual ~ModuleBase();

        template <typename T>
        T* GetDependenceModule()
        {
            Core::Common::FixString64 szName(T::ModuleName());
            auto pTargetModule = reinterpret_cast<T*>(ModuleManager::I().Get(szName.Str()));
            if (pTargetModule == NULL)
            {
                HE_PG_LOG_LINE(HE_STR_TEXT("指定したモジュール(")
                                   HE_STR_FORMAT_TEXT HE_STR_TEXT(")が存在しない"),
                               szName.Str());
                return NULL;
            }

            // 依存対象のモジュールかどうかチェック
            Bool bHit = FALSE;
            for (Uint32 i = 0; i < this->_vDependenceModuleName.Size(); ++i)
            {
                if (this->_vDependenceModuleName[i] == szName)
                {
                    bHit = TRUE;
                    break;
                }
            }

            if (bHit == FALSE) return NULL;

            return pTargetModule;
        }

        /// <summary>
        /// モジュールが依存しているモジュール名リスト
        /// </summary>
        const Core::Common::VectorBase<Core::Common::FixString64>& GetDependenceModuleNames() const
        {
            return this->_vDependenceModuleName;
        }

        inline const Char* Name() const HE_NOEXCEPT { return this->_szName.Str(); }
        inline const eLayer Layer() const HE_NOEXCEPT { return this->_eLayer; }

        /// <summary>
        /// モジュール処理優先度
        /// 値が大きいほど後に処理
        /// </summary>
        inline const Sint32 Priority() const { return this->_priority; }

    protected:
        /// <summary>
        /// モジュールの開始
        /// </summary>
        virtual const Bool _VStart() = 0;

        /// <summary>
        /// モジュールの解放
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _VRelease() = 0;

        /// <summary>
        /// モジュール前更新
        /// </summary>
        virtual const Bool _VBeforeUpdate(const Float32 in_fDeltaTime) { return TRUE; }

        /// <summary>
        /// モジュール更新
        /// </summary>
        virtual const Bool _VUpdate(const Float32 in_fDeltaTime) { return TRUE; }

        /// <summary>
        /// モジュール後更新
        /// </summary>
        virtual const Bool _VLateUpdate(const Float32 in_fDeltaTime) { return TRUE; }

        template <class T>
        void _AppendDependenceModule()
        {
            this->_vDependenceModuleName.PushBack(T::ModuleName());
        }

    private:
#ifdef HE_ENGINE_DEBUG
        const Bool _ValidateDependenceModule();
#endif

    private:
        Core::Common::FixString128 _szName;
        eLayer _eLayer   = eLayer_Logic;
        Sint32 _priority = 0;

#ifdef HE_ENGINE_DEBUG
        Core::Common::CustomFixVector<ModuleBase*, 64> _vDependenceModule;
#endif
        Core::Common::CustomFixVector<Core::Common::FixString64, 64> _vDependenceModuleName;

    private:
        friend class ModuleManager;
    };

}  // namespace Module

// モジュールクラスに宣言するマクロ
// _type_にクラス型を記述
#define HE_MODULE_GENRATE_DECLARATION(_type_) \
public:                                       \
    static const UTF8* ModuleName()           \
    {                                         \
        return #_type_;                       \
    }
