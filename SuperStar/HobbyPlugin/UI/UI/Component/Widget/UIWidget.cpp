#include "UIWidget.h"

namespace UI
{
    void UIWidgetComponent::VSetup(const Bool in_bAutoDelete)
    {
        Actor::Component::VSetup(in_bAutoDelete);
        this->_Clear();
    }

    const Bool UIWidgetComponent::VBegin()
    {
        if (this->_viewHandle.Null()) return FALSE;
        return TRUE;
    }

}  // namespace UI
