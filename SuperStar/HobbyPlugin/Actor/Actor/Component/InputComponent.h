#pragma once

#include "Component.h"
#include "Engine/Common/CustomList.h"

namespace Actor
{
    // 前方宣言
    class InputComponent;

    /// <summary>
    /// 入力コンポーネントのストラテジークラス
    /// </summary>
    class InputStrategyBase
    {
    public:
        virtual ~InputStrategyBase() {}

        virtual const Bool VBegin() { return TRUE; }
        virtual const Bool VEnd() { return TRUE; }

        virtual void VProcessInput(const void* in_pInputMap, Object*) = 0;
    };

    /// <summary>
    /// 入力の基礎コンポーネント
    /// 機能拡張は継承したストラテジークラスで行う
    /// </summary>
    class InputComponent final : public Component,
                                 public Core::Common::LinkedListNode<InputComponent>
    {
        HE_CLASS_COPY_NG(InputComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InputComponent, Component);

    public:
        InputComponent() : Component() {}
        virtual ~InputComponent();

        /// <summary>
        /// タスク終了
        /// </summary>
        const Bool VEnd() override final;

        /// <summary>
        /// 入力処理
        /// </summary>
        void ProcessInput(const void* in_pInputMap);

        /// <summary>
        /// ストラテジー設定
        /// </summary>
        void SetStrategy(std::shared_ptr<InputStrategyBase>);

    private:
        std::shared_ptr<InputStrategyBase> _spStrategy = NULL;
    };
}  // namespace Actor
