#include "UIWidget.h"

namespace UI
{
    void UIWidgetComponent::Setup(const Bool in_bAutoDelete)
    {
        Actor::Component::Setup(in_bAutoDelete);
        this->_Clear();
    }

    const Bool UIWidgetComponent::Begin()
    {
        if (this->_viewHandle.Null()) return FALSE;
        return TRUE;
    }

}  // namespace UI
