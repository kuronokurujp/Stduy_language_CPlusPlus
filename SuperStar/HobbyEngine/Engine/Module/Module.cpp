#include "Module.h"

#include "Engine/Engine.h"

namespace Module
{
    ModuleBase::ModuleBase(const Char* in_szName)
    {
        this->_szName = in_szName;
        _mModule.Add(in_szName, this);
    }

    Platform::PlatformModule* ModuleBase::GetPlatformModule() const
    {
        return HOBBY_ENGINE.GetPlatformModule();
    }

    ModuleBase* ModuleManager::Get(const Char* in_szName)
    {
        if (ModuleBase::_mModule.Contains(in_szName))
        {
            return ModuleBase::_mModule[in_szName];
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
            for (auto b = ModuleBase::_mModule.Begin(); b != ModuleBase::_mModule.End(); ++b)
            {
                b->_tData->Release();
            }
            // this->_modules.Empty();
            ModuleBase::_mModule.Clear();
        }

        return TRUE;
    }

    void ModuleManager::Update(const Float32 in_fDeltaTime)
    {
        // 更新処理は優先順位をつけて処理順を変える対応が必要かも
        {
            for (auto b = ModuleBase::_mModule.Begin(); b != ModuleBase::_mModule.End(); ++b)
            {
                // TODO: 依存しているモジュールが存在しているかチェック
                b->_tData->Update(in_fDeltaTime);
            }
        }
    }

    const Bool ModuleManager::Start()
    {
        for (auto b = ModuleBase::_mModule.Begin(); b != ModuleBase::_mModule.End(); ++b)
        {
            HE_LOG_LINE(HE_STR_TEXT("Start Module(") HE_STR_FORMAT_TEXT HE_STR_TEXT(")"),
                        b->_tData->Name());
            if (b->_tData->Start() == FALSE)
            {
                HE_ASSERT(FALSE && "モジュール開始に失敗");
            }
        }

        return TRUE;
    }
}  // namespace Module
