#include "EnhancedInputModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

// 依存モジュール
#include "Engine/Platform/PlatformModule.h"

namespace EnhancedInput
{
    EnhancedInputModule::EnhancedInputModule() : ModuleBase(ModuleName(), Module::eLayer_Logic, -10)
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<Platform::PlatformModule>();
    }

    void EnhancedInputModule::SetCommonMappingAction(const ActionMap& in_mrAction)
    {
        this->_mMappingAction = in_mrAction;
    }

    void EnhancedInputModule::AddCommonMappingAction(const ActionMap& in_mrAction)
    {
        for (auto it = in_mrAction.Begin(); it != in_mrAction.End(); ++it)
        {
            this->_mMappingAction.Add(it->key, it->data);
        }
    }

    void EnhancedInputModule::RemoveCommonMappingAction(const ActionMap& in_mrAction)
    {
        for (auto it = in_mrAction.Begin(); it != in_mrAction.End(); ++it)
        {
            this->_mMappingAction.Erase(it->key);
        }
    }

    void EnhancedInputModule::RemoveCommonMappingAction(const Char* in_pActonName)
    {
        HE_ASSERT(in_pActonName);
        this->_mMappingAction.Erase(in_pActonName);
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool EnhancedInputModule::_VStart()
    {
        return TRUE;
    }

    const Bool EnhancedInputModule::_VRelease()
    {
        return TRUE;
    }

    const Bool EnhancedInputModule::_VBeforeUpdate(const Float32 in_fDeltaTime)
    {
        this->_mInputAction.Clear();
        return TRUE;
    }

    const Bool EnhancedInputModule::_VUpdate(const Float32 in_fDeltaTime)
    {
        auto pPlatformModule = this->GetDependenceModule<Platform::PlatformModule>();
        HE_ASSERT(pPlatformModule);

        auto pInput = pPlatformModule->VInput();
        HE_ASSERT(pInput);

        // 入力処理をする
        if (this->_mMappingAction.Empty()) return TRUE;

        auto& inputState = pInput->GetState();

        // 登録した入力アクションが発生しているかチェック
        for (auto it = this->_mMappingAction.Begin(); it != this->_mMappingAction.End(); ++it)
        {
            // 発生している場合は発生リストに登録
            for (Uint32 i = 0; i < it->data.aKeyboardKeys.Size(); ++i)
            {
                const auto eKey = it->data.aKeyboardKeys[i];
                if (inputState._keyboard.GetKeyState(eKey))
                {
                    InputData inputData;
                    inputData.eType              = eInputType_Keyboard;
                    inputData.item.keyboard.eKey = eKey;

                    this->_mInputAction[it->key].PushBack(inputData);
                }
            }

            for (Uint32 i = 0; i < it->data.aTouchs.Size(); ++i)
            {
                const auto eTouch      = it->data.aTouchs[i];
                const auto eTouchState = inputState._touch.GetTouchState(it->data.aTouchs[i]);
                if (eTouchState == Platform::EInputState::EInputState_PRESSED)
                {
                    const auto pos = inputState._touch.GetWorldPos();

                    InputData inputData;
                    inputData.eType            = eInputType_Touch;
                    inputData.item.touch.fX    = pos._fX;
                    inputData.item.touch.fY    = pos._fY;
                    inputData.item.touch.eType = eTouch;

                    auto& r = this->_mInputAction[it->key];
                    r.PushBack(inputData);
                }
            }
        }

        return TRUE;
    }

}  // namespace EnhancedInput
