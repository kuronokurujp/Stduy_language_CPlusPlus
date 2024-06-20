#pragma once

#include "Core/Math/Math.h"
#include "HobbyPlugin/Actor/Component/Component.h"
#include "Platform/PlatformInput.h"

namespace UI
{
    /// <summary>
    /// UIのWidgetの基本コンポーネント
    /// Widgetコンポーネントは必ずこのクラスを継承する
    /// </summary>
    class UIWidgetComponent : public Actor::Component
    {
        E_CLASS_COPY_CONSTRUCT_NG(UIWidgetComponent);
        GENERATED_CLASS_BODY_HEADER();

    public:
        UIWidgetComponent() : Actor::Component() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Init(const Bool in_bReleaseMem = TRUE) override;

        virtual void OnTouch(const Platform::TouchInput& in_rTouch) {}
        virtual void OnKey(const Platform::KeyboardInput& in_rKeyboard) {}

        void SetRect(const Core::Math::Rect2& in_rect) { this->_rect = in_rect; }
        void SetPos(const Core::Math::Vector2& in_pos);

        void SetColor(const Uint32 in_color) { this->_color = in_color; }

    private:
        void _Clear()
        {
            this->_rect.Clear();
            this->_color = 0;
        }

    protected:
        Core::Math::Rect2 _rect;
        Uint32 _color = 0;
    };
}  // namespace UI
