#include "InputComponent.h"

#include "Actor/Actor.h"

namespace Actor
{
    const Bool InputComponent::Begin()
    {
        if (Component::Begin() == FALSE) return FALSE;

        auto pAcceesor = this->_pOwner->GetManagerAcceesor();
        HE_ASSERT(pAcceesor);

        pAcceesor->RegistInputComponent(*this);

        return TRUE;
    }

    const Bool InputComponent::End()
    {
        auto pAcceesor = this->_pOwner->GetManagerAcceesor();
        HE_ASSERT(pAcceesor);

        pAcceesor->UnRegistInputComponent(*this);

        return Component::End();
    }

}  // namespace Actor
