#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    const Bool Widget::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        return TRUE;
    }

    const Bool Widget::VEnd()
    {
        if (Actor::Object::VEnd() == FALSE) return FALSE;

        return TRUE;
    }

}  // namespace UI
