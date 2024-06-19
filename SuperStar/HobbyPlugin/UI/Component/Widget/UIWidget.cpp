#include "UIWidget.h"

namespace UI
{
    GENERATED_CLASS_BODY(UIWidgetComponent, Actor::Component);

    void UIWidgetComponent::Init(const Bool in_bAutoDelete)
    {
        Actor::Component::Init(in_bAutoDelete);
        this->_Clear();
    }

    void UIWidgetComponent::SetPos(const Core::Math::Vector2& in_pos)
    {
        this->_rect = in_pos;
    }
}
