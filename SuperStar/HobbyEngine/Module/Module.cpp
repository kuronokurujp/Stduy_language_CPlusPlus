#include "Module.h"

#include "../Engine.h"

namespace Module
{
    ModuleBase::ModuleBase(const Char* in_pName)
    {
        this->_name = in_pName;
        _modules.Add(in_pName, this);
    }

    Platform::PlatformModule* ModuleBase::GetPlatformModule() const
    {
#ifdef _HOBBY_ENGINE
        return HOBBY_ENGINE.GetPlatformModule();
#else
        return NULL;
#endif
    }

    ModuleBase* ModuleManager::Get(const Char* in_pName)
    {
        E_ASSERT(ModuleBase::_modules.Contains(in_pName));
        return ModuleBase::_modules[in_pName];
    }

    const Bool ModuleManager::Release()
    {
        // 全モジュール解放
        {
            for (auto b = ModuleBase::_modules.Begin(); b != ModuleBase::_modules.End(); ++b)
            {
                b->_data->Release();
            }
            // this->_modules.Empty();
            ModuleBase::_modules.Clear();
        }

        return TRUE;
    }

    void ModuleManager::Update(const Float32 in_deltaTime)
    {
        // 更新処理は優先順位をつけて処理順を変える対応が必要かも
        {
            for (auto b = ModuleBase::_modules.Begin(); b != ModuleBase::_modules.End(); ++b)
            {
                // TODO: 依存しているモジュールが存在しているかチェック
                b->_data->Update(in_deltaTime);
            }
        }
    }

    const Bool ModuleManager::Start()
    {
        for (auto b = ModuleBase::_modules.Begin(); b != ModuleBase::_modules.End(); ++b)
        {
            E_LOG_LINE(E_STR_TEXT("Start Module(") E_STR_FORMAT_TEXT E_STR_TEXT(")"), b->_data->Name());
            if (b->_data->Start() == FALSE)
            {
                E_ASSERT(FALSE && "モジュール開始に失敗");
            }
        }

        return TRUE;
    }
}  // namespace Module
