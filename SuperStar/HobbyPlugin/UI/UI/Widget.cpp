#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    const Bool Widget::Begin()
    {
        if (Actor::Object::Begin() == FALSE) return FALSE;

        return TRUE;
    }

    const Bool Widget::End()
    {
        if (Actor::Object::End() == FALSE) return FALSE;

        return TRUE;
    }

}  // namespace UI
