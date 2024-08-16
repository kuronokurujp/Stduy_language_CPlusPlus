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

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual const Bool Begin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual const Bool End() override;

        /// <summary>
        /// 入力処理
        /// </summary>
        /// <param name="in_pInputMap"></param>
        virtual void ProcessInput(const void* in_pInputMap) = 0;
    };
}  // namespace Actor
