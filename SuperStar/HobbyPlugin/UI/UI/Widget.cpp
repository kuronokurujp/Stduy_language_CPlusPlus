﻿#include "Widget.h"

#include "Component/Input/UIInputRouter.h"

namespace UI
{
    Bool Widget::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        return TRUE;
    }

    Bool Widget::VEnd()
    {
        if (Actor::Object::VEnd() == FALSE) return FALSE;

        return TRUE;
    }

}  // namespace UI
