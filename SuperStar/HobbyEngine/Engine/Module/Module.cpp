#include "Module.h"

#include "Engine/Engine.h"

namespace Module
{
    ModuleBase::ModuleBase(const UTF8* in_szName, const eLayer in_eLayer, const Sint32 in_priority)
        : _szName(in_szName), _eLayer(in_eLayer), _priority(in_priority)
    {
    }

    ModuleBase::~ModuleBase()
    {
    }

#ifdef HE_ENGINE_DEBUG
    const Bool ModuleBase::_ValidateDependenceModule()
    {
        for (Uint32 i = 0; i < this->_vDependenceModule.Size(); ++i)
        {
            // 依存モジュールに設定している依存モジュールが自身の場合はエラー
            // 依存の循環参照は認めない
            const auto& vCheckModule = this->_vDependenceModule[i]->_vDependenceModule;
            for (Uint32 j = 0; j < vCheckModule.Size(); ++j)
            {
                if (this->_szName == vCheckModule[j]->Name())
                {
                    HE_PG_LOG_LINE(HE_STR_FORMAT_TEXT HE_STR_TEXT("モジュールに依存している")
                                       HE_STR_FORMAT_TEXT HE_STR_TEXT("と循環参照関係になっている"),
                                   this->_szName.Str(), vCheckModule[j]->Name());
                    return FALSE;
                }
            }
        }

        return TRUE;
    }
#endif

    ModuleBase* ModuleManager::Get(const Char* in_szName) const
    {
        if (this->_mAppModule.Contains(in_szName))
        {
            return this->_mAppModule.FindKey(in_szName)->data;
        }

        if (this->_mLogicModule.Contains(in_szName))
        {
            return this->_mLogicModule.FindKey(in_szName)->data;
        }

        if (this->_mViewModule.Contains(in_szName))
        {
            return this->_mViewModule.FindKey(in_szName)->data;
        }

        // HE_ASSERT(FALSE);
        return NULL;
    }

    const Bool ModuleManager::Release()
    {
        // 全モジュール解放
        // TODO: 依存関係に応じてモジュールを破棄する順序を変える事はできないか？
        {
            for (Uint32 i = 0; i < this->_vViewModule.Size(); ++i)
            {
                this->_vViewModule[i]->_Release();
            }
            this->_vViewModule.Clear();

            for (auto b = this->_mViewModule.Begin(); b != this->_mViewModule.End(); ++b)
            {
                HE_DELETE(b->data);
            }
            this->_mViewModule.Clear();
        }

        {
            for (Uint32 i = 0; i < this->_vLogicModule.Size(); ++i)
            {
                this->_vLogicModule[i]->_Release();
            }
            this->_vLogicModule.Clear();

            for (auto b = this->_mLogicModule.Begin(); b != this->_mLogicModule.End(); ++b)
            {
                HE_DELETE(b->data);
            }
            this->_mLogicModule.Clear();
        }

        {
            for (Uint32 i = 0; i < this->_vAppModule.Size(); ++i)
            {
                this->_vAppModule[i]->_Release();
            }
            this->_vAppModule.Clear();

            for (auto b = this->_mAppModule.Begin(); b != this->_mAppModule.End(); ++b)
            {
                HE_DELETE(b->data);
            }
            this->_mAppModule.Clear();
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

    /// <summary>
    /// ヒープ作成したモジュールを登録
    /// </summary>
    const Bool ModuleManager::RegistHeapModule(ModuleBase* in_pModule)
    {
        HE_ASSERT(in_pModule);
        // モジュールレイヤーに応じたリストに登録
        const auto eLayer = in_pModule->Layer();
        if (eLayer == eLayer_App)
        {
            this->_mAppModule.Add(in_pModule->_szName, in_pModule);
            return TRUE;
        }

        if (eLayer == eLayer_Logic)
        {
            this->_mLogicModule.Add(in_pModule->_szName, in_pModule);
            return TRUE;
        }

        if (eLayer == eLayer_View)
        {
            this->_mViewModule.Add(in_pModule->_szName, in_pModule);
            return TRUE;
        }

        return FALSE;
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
                for (auto b = this->_mAppModule.Begin(); b != this->_mAppModule.End(); ++b)
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
                for (auto b = this->_mLogicModule.Begin(); b != this->_mLogicModule.End(); ++b)
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
                for (auto b = this->_mViewModule.Begin(); b != this->_mViewModule.End(); ++b)
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

#ifdef HE_ENGINE_DEBUG
                in_rModule._vDependenceModule.PushBack(pDependenceModule);
#endif
            }
#ifdef HE_ENGINE_DEBUG
            HE_ASSERT(in_rModule._ValidateDependenceModule() && "依存設定が循環になっている");
#endif
        }

        return TRUE;
    }

}  // namespace Module
