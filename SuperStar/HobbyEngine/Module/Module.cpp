#include "Module.h"

namespace Module
{
    const Bool ModuleManager::Init()
    {
        return TRUE;
    }

    void ModuleManager::End()
    {
        // TODO: 全モジュール解放
        {
            for (Uint32 i = 0; i < this->_modules.Size(); ++i)
            {
                this->_modules[i]->End();
                E_DELETE(this->_modules[i]);
            }
            this->_modules.IsEmpty();
        }
    }

    void ModuleManager::Update(const Float32 in_deltaTime)
    {
        // TODO: 更新処理は優先順位をつけて処理順を変える対応が必要かも
        {
            Uint32 size = this->_modules.Size();
            for (Uint32 i = 0; i < size; ++i)
            {
                // TODO: 依存しているモジュールが存在しているかチェック
                this->_modules[i]->Update(in_deltaTime);
            }
        }
    }

    const Bool ModuleManager::Apply()
    {
        for (Uint32 i = 0; i < this->_modules.Size(); ++i)
        {
            if (this->_modules[i]->Init() == FALSE)
            {
                E_ASSERT(FALSE && "モジュール初期化に失敗");
            }
        }

        return TRUE;
    }
}
