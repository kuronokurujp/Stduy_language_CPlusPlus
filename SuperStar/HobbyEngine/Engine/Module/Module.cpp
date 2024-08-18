#include "Module.h"

#include "Engine/Engine.h"

namespace Module
{
    namespace Local
    {
        static Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 16> mAppModule;
        static Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 32> mLogicModule;
        static Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 16> mViewModule;
    }  // namespace Local

    ModuleBase::ModuleBase(const UTF8* in_szName, const eLayer in_eLayer, const Sint32 in_priority)
        : _szName(in_szName), _eLayer(in_eLayer), _priority(in_priority)
    {
        // モジュールレイヤーに応じたリストに登録
        if (this->Layer() == eLayer_App)
        {
            Local::mAppModule.Add(this->_szName.Str(), this);
        }
        if (this->Layer() == eLayer_Logic) Local::mLogicModule.Add(this->_szName.Str(), this);
        if (this->Layer() == eLayer_View) Local::mViewModule.Add(this->_szName.Str(), this);
    }

    ModuleBase::~ModuleBase()
    {
        // モジュールを解放したのでモジュールリストから除外する
        {
            auto it = Local::mAppModule.FindKey(this->_szName);
            if (it != Local::mAppModule.End()) Local::mAppModule.Erase(it);
        }

        {
            auto it = Local::mLogicModule.FindKey(this->_szName);
            if (it != Local::mLogicModule.End()) Local::mLogicModule.Erase(it);
        }

        {
            auto it = Local::mViewModule.FindKey(this->_szName);
            if (it != Local::mViewModule.End()) Local::mViewModule.Erase(it);
        }
    }

    ModuleBase* ModuleManager::Get(const Char* in_szName) const
    {
        if (Local::mAppModule.Contains(in_szName))
        {
            return Local::mAppModule[in_szName];
        }

        if (Local::mLogicModule.Contains(in_szName))
        {
            return Local::mLogicModule[in_szName];
        }

        if (Local::mViewModule.Contains(in_szName))
        {
            return Local::mViewModule[in_szName];
        }

        HE_ASSERT(FALSE);
        return NULL;
    }

    const Bool ModuleManager::Release()
    {
        // 全モジュール解放
        {
            for (auto b = Local::mViewModule.Begin(); b != Local::mViewModule.End(); ++b)
            {
                b->data->_Release();
            }
            Local::mViewModule.Clear();
            this->_vViewModule.Clear();
        }

        {
            for (auto b = Local::mLogicModule.Begin(); b != Local::mLogicModule.End(); ++b)
            {
                b->data->_Release();
            }
            Local::mLogicModule.Clear();
            this->_vLogicModule.Clear();
        }

        {
            for (auto b = Local::mAppModule.Begin(); b != Local::mAppModule.End(); ++b)
            {
                b->data->_Release();
            }
            Local::mAppModule.Clear();
            this->_vAppModule.Clear();
        }

        return TRUE;
    }

    void ModuleManager::BeforeUpdate(const Float32 in_fDeltaTime)
    {
        for (Uint32 i = 0; i < this->_vAppModule.Size(); ++i)
        {
            this->_vAppModule[i]->_BeforeUpdate(in_fDeltaTime);
        }

        for (Uint32 i = 0; i < this->_vLogicModule.Size(); ++i)
        {
            this->_vLogicModule[i]->_BeforeUpdate(in_fDeltaTime);
        }

        for (Uint32 i = 0; i < this->_vViewModule.Size(); ++i)
        {
            this->_vViewModule[i]->_BeforeUpdate(in_fDeltaTime);
        }
    }

    void ModuleManager::Update(const Float32 in_fDeltaTime)
    {
        for (Uint32 i = 0; i < this->_vLogicModule.Size(); ++i)
        {
            this->_vLogicModule[i]->_Update(in_fDeltaTime);
        }

        for (Uint32 i = 0; i < this->_vViewModule.Size(); ++i)
        {
            this->_vViewModule[i]->_Update(in_fDeltaTime);
        }

        // ロジックとビューのモジュール更新結果を元にアプリモジュールは更新される
        for (Uint32 i = 0; i < this->_vAppModule.Size(); ++i)
        {
            this->_vAppModule[i]->_Update(in_fDeltaTime);
        }
    }

    void ModuleManager::LateUpdate(const Float32 in_fDeltaTime)
    {
        for (Uint32 i = 0; i < this->_vViewModule.Size(); ++i)
        {
            this->_vViewModule[i]->_LateUpdate(in_fDeltaTime);
        }

        for (Uint32 i = 0; i < this->_vLogicModule.Size(); ++i)
        {
            this->_vLogicModule[i]->_LateUpdate(in_fDeltaTime);
        }

        for (Uint32 i = 0; i < this->_vAppModule.Size(); ++i)
        {
            this->_vAppModule[i]->_LateUpdate(in_fDeltaTime);
        }
    }

    const Bool ModuleManager::Start(const eLayer in_eLayer)
    {
        switch (in_eLayer)
        {
            case eLayer_App:
            {
                // AppモジュールはプラットフォームなどのOS関連のモジュール
                // newでメモリ確保とかもあり得る
                // ゲーム特有処理の前段となる
                for (auto b = Local::mAppModule.Begin(); b != Local::mAppModule.End(); ++b)
                {
                    if (this->_StartModule(*b->data))
                    {
                        this->_vAppModule.PushBack(b->data);
                    }
                }
                this->_SortModuleVector(&this->_vAppModule);

                break;
            }
            case eLayer_Logic:
            {
                for (auto b = Local::mLogicModule.Begin(); b != Local::mLogicModule.End(); ++b)
                {
                    if (this->_StartModule(*b->data))
                    {
                        this->_vLogicModule.PushBack(b->data);
                    }
                }

                this->_SortModuleVector(&this->_vLogicModule);
                break;
            }
            case eLayer_View:
            {
                for (auto b = Local::mViewModule.Begin(); b != Local::mViewModule.End(); ++b)
                {
                    if (this->_StartModule(*b->data))
                    {
                        this->_vViewModule.PushBack(b->data);
                    }
                }

                this->_SortModuleVector(&this->_vViewModule);
                break;
            }
        }

        return TRUE;
    }

    void ModuleManager::_SortModuleVector(Core::Common::VectorBase<ModuleBase*>* out)
    {
        if (out->Size() <= 1) return;

        // モジュール数が3桁もいかないので高速ソートは不要
        for (Uint32 i = 0; i < out->Size() - 1; ++i)
        {
            Sint32 nowPriority = (*out)[i]->Priority();
            for (Uint32 j = i; j < out->Size(); ++j)
            {
                Sint32 cmpPriority = (*out)[j]->Priority();
                if (cmpPriority < nowPriority)
                {
                    auto tmp  = (*out)[i];
                    (*out)[i] = (*out)[j];
                    (*out)[j] = tmp;
                }
            }
        }
    }

    const Bool ModuleManager::_StartModule(ModuleBase& in_rModule)
    {
        HE_LOG_LINE(HE_STR_TEXT("Start Module(") HE_STR_FORMAT_TEXT HE_STR_TEXT(")"),
                    in_rModule.Name());
        if (in_rModule._Start() == FALSE)
        {
            HE_ASSERT(FALSE && "モジュール開始に失敗");
        }

        // 依存情報を見る
        auto& vDependenceModuleNames = in_rModule.GetDependenceModuleNames();
        if (0 < vDependenceModuleNames.Size())
        {
            // 依存しているモジュールリストを作成
            for (Uint32 i = 0; i < vDependenceModuleNames.Size(); ++i)
            {
                auto szName            = vDependenceModuleNames[i];
                auto pDependenceModule = this->Get(szName.Str());
                if (pDependenceModule == NULL) continue;

                in_rModule._vDependenceModule.PushBack(&in_rModule);
            }
        }

        return TRUE;
    }

}  // namespace Module
