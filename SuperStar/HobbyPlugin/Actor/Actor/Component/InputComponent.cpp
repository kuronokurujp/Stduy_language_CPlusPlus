#include "InputComponent.h"

namespace Actor
{
    InputComponent::~InputComponent()
    {
        if (this->_spStrategy) this->_spStrategy = NULL;
    }

    Bool InputComponent::VEnd()
    {
        if (this->_spStrategy)
        {
            this->_spStrategy = NULL;
        }

        return Component::VEnd();
    }

    void InputComponent::ProcessInput(const void* in_pInputMap)
    {
        if (this->_spStrategy == NULL) return;

        this->_spStrategy->VProcessInput(in_pInputMap, this->_pOwner);
    }

    void InputComponent::SetStrategy(std::shared_ptr<InputStrategyBase> in_pStrategy)
    {
        this->_spStrategy = in_pStrategy;
    }

}  // namespace Actor
