#include "UIWidget.h"

namespace UI
{

    void UIWidgetComponent::Setup(const Bool in_bAutoDelete)
    {
        Actor::Component::Setup(in_bAutoDelete);
        this->_Clear();
    }

}  // namespace UI
