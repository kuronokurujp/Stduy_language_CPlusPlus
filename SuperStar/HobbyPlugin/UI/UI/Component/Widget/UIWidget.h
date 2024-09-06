#pragma once

#include "ActorModule.h"
#include "Engine/Math/Math.h"
#include "Engine/Platform/PlatformInput.h"
#include "EnhancedInputModule.h"

namespace UI
{
    /// <summary>
    /// UIのWidgetの基本コンポーネント
    /// Widgetコンポーネントは必ずこのクラスを継承する
    /// </summary>
    class UIWidgetComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(UIWidgetComponent);
        HE_GENERATED_CLASS_BODY_HEADER(UIWidgetComponent, Actor::Component);

    public:
        UIWidgetComponent() : Actor::Component() { this->_Clear(); }
        virtual ~UIWidgetComponent() = default;

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void VSetup(const Bool in_bReleaseMem = TRUE) override;

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual const Bool VBegin() override;

        /// <summary>
        /// タスク終了
        /// </summary>
        /// <returns></returns>
        virtual const Bool VEnd() override;

        virtual void VOnTouch(const EnhancedInput::InputData::Item::Touch& in_rTouch) {}
        virtual void VOnKey(const EnhancedInput::InputData::Item::Key& in_rKeyboard) {}

        void SetPos(const Core::Math::Vector2 in_pos);
        void SetColor(const Uint32 in_color) { this->_color = in_color; }
        void SetViewHandle(const Core::Common::Handle& in_rHandle)
        {
            this->_viewHandle = in_rHandle;
        }

        Actor::TransformComponent* GetTransformComponent();

    protected:
        void _Clear()
        {
            this->_pos.Zero();
            this->_color = 0;
            this->_viewHandle.Clear();
            this->_transformHandle.Clear();
        }

    protected:
        Core::Math::Vector2 _pos;
        Uint32 _color = 0;
        Core::Common::Handle _viewHandle;
        Core::Common::Handle _transformHandle;
    };
}  // namespace UI
