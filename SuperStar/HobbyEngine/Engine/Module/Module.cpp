#include "Module.h"

#include "Engine/Engine.h"

namespace Module
{
    namespace Local
    {
        static Core::Common::CustomFixMap<Core::Common::FixString128, ModuleBase*, 64> mModule;
    }

    ModuleBase::ModuleBase(const UTF8* in_szName) : _szName(in_szName)
    {
        Local::mModule.Add(this->_szName.Str(), this);
    }

    ModuleBase::~ModuleBase()
    {
        auto it = Local::mModule.FindKey(this->_szName);
        if (it != Local::mModule.End()) Local::mModule.Erase(it);
    }

    ModuleBase* ModuleManager::Get(const Char* in_szName) const
    {
        if (Local::mModule.Contains(in_szName))
        {
            return Local::mModule[in_szName];
        }
        else
        {
            HE_ASSERT(FALSE);
        }

        return NULL;
    }

    const Bool ModuleManager::Release()
    {
        // 全モジュール解放
        {
            for (auto b = Local::mModule.Begin(); b != Local::mModule.End(); ++b)
            {
                b->data->_Release();
            }
            Local::mModule.Clear();
            this->_vModule.Clear();
        }

        return TRUE;
    }

    void ModuleManager::Update(const Float32 in_fDeltaTime)
    {
        for (Uint32 i = 0; i < this->_vModule.Size(); ++i)
        {
            this->_vModule[i]->_Update(in_fDeltaTime);
        }
    }

    const Bool ModuleManager::Start()
    {
        for (auto b = Local::mModule.Begin(); b != Local::mModule.End(); ++b)
        {
            HE_LOG_LINE(HE_STR_TEXT("Start Module(") HE_STR_FORMAT_TEXT HE_STR_TEXT(")"),
                        b->data->Name());
            if (b->data->_Start() == FALSE)
            {
                HE_ASSERT(FALSE && "モジュール開始に失敗");
            }

            // モジュール処理の配列に追加
            this->_vModule.PushBack(b->data);

            // 依存情報を見る
            auto vDependenceModuleNames = b->data->GetDependenceModuleNames();
            if (vDependenceModuleNames.Size() <= 0)
            {
                continue;
            }

            // 依存しているモジュールリストを作成
            for (Uint32 i = 0; i < vDependenceModuleNames.Size(); ++i)
            {
                auto szName            = vDependenceModuleNames[i];
                auto pDependenceModule = this->Get(szName.Str());
                if (pDependenceModule == NULL) continue;

                b->data->_vDependenceModule.PushBack(b->data);
            }
        }

        // 優先度に従ってソートする
        // モジュール数が3桁もいかないので高速ソートは不要
        {
            for (Uint32 i = 0; i < this->_vModule.Size() - 1; ++i)
            {
                Sint32 nowPriority = this->_vModule[i]->Prioryty();
                for (Uint32 j = i; j < this->_vModule.Size(); ++j)
                {
                    Sint32 cmpPriority = this->_vModule[j]->Prioryty();
                    if (cmpPriority < nowPriority)
                    {
                        auto tmp          = this->_vModule[i];
                        this->_vModule[i] = this->_vModule[j];
                        this->_vModule[j] = tmp;
                    }
                }
            }
        }

        return TRUE;
    }
}  // namespace Module
