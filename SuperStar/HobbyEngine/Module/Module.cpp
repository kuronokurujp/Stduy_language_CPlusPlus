#include "Module.h"

namespace Module
{
    ModuleInterface::ModuleInterface()
    {
        MODULE_MANAGER_DEFINITION;
        ModuleManager::I()._Add(this);
    }

    void ModuleManager::Release()
    {
        // 全モジュール解放
        {
            for (Uint32 i = 0; i < this->_modules.Size(); ++i)
            {
                this->_modules[i]->End();
            }
            this->_modules.IsEmpty();
        }
    }

    void ModuleManager::Update(const Float32 in_deltaTime)
    {
        // 更新処理は優先順位をつけて処理順を変える対応が必要かも
        {
            Uint32 size = this->_modules.Size();
            for (Uint32 i = 0; i < size; ++i)
            {
                // 依存しているモジュールが存在しているかチェック
                this->_modules[i]->Update(in_deltaTime);
            }
        }
    }

    void ModuleManager::_Add(Module::ModuleInterface* in_pModule)
    {
        this->_modules.PushBack(in_pModule);
    }

    const Bool ModuleManager::Apply()
    {
        for (Uint32 i = 0; i < this->_modules.Size(); ++i)
        {
            E_LOG_LINE(E_STR_TEXT("No(%d) Module(%hs)"), i, typeid(*this->_modules[i]).name());
            if (this->_modules[i]->Init() == FALSE)
            {
                E_ASSERT(FALSE && "モジュール初期化に失敗");
            }
        }

        return TRUE;
    }
}  // namespace Module
