#pragma once

#include "Component.h"
#include "Engine/Common/CustomList.h"

namespace Actor
{
    /// <summary>
    /// 入力の基礎コンポーネント
    /// 継承前提のクラス
    /// このクラスのみのインスタンスを作れない
    /// </summary>
    class InputComponent : public Component, public Core::Common::LinkedListNode<InputComponent>
    {
        HE_CLASS_COPY_NG(InputComponent);
        GENERATED_CLASS_BODY_HEADER(InputComponent, Component);

    public:
        InputComponent() : Component() {}
        virtual ~InputComponent() = default;

        /// <summary>
        /// 入力処理
        /// </summary>
        virtual void VProcessInput(const void* in_pInputMap) = 0;
    };
}  // namespace Actor
