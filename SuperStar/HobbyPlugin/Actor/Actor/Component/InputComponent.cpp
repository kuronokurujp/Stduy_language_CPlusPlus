#include "InputComponent.h"

namespace Actor
{
    InputComponent::~InputComponent()
    {
        if (this->_pStrategy) this->_pStrategy = NULL;
    }

    const Bool InputComponent::VEnd()
    {
        if (this->_pStrategy)
        {
            this->_pStrategy = NULL;
        }

        return Component::VEnd();
    }

    void InputComponent::ProcessInput(const void* in_pInputMap)
    {
        if (this->_pStrategy == NULL) return;

        this->_pStrategy->VProcessInput(in_pInputMap, this->_pOwner);
    }

    void InputComponent::SetStrategy(std::shared_ptr<InputStrategyBase> in_pStrategy)
    {
        this->_pStrategy = in_pStrategy;
    }

}  // namespace Actor
