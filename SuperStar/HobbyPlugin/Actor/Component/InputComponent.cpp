#include "Component/inputComponent.h"

#include "Actor/actor.h"

/// <summary>
/// The none key code
/// </summary>
const int InputComponent::NoneKeyCode = -1;

/// <summary>
/// Initializes a new instance of the <see cref="InputComponent"/> class.
/// </summary>
/// <param name="in_pOwner">The in p owner.</param>
InputComponent::InputComponent(Actor* in_pOwner) : MoveComponent(in_pOwner, 10)
{
    this->_Clear();
}

/// <summary>
/// Processes the input.
/// </summary>
/// <param name="in_pKeyState">State of the in p key.</param>
/// <returns></returns>
auto InputComponent::ProcessInput(const uint8_t* in_pKeyState) -> void
{
    // 前進と交代
    {
        float speed = 0.0f;
        // 前進移動のキー入力
        if (this->forwardKeyCode != NoneKeyCode && in_pKeyState[this->forwardKeyCode])
        {
            speed += this->maxForwardSpeed;
        }

        // 後退移動のキー入力
        if (this->backKeyCode != NoneKeyCode && in_pKeyState[this->backKeyCode])
        {
            speed -= this->maxForwardSpeed;
        }
        this->SetForwardSpeed(speed);
    }

    // 回転
    {
        float angular = 0.0f;
        // 時計回りで回転のキー入力
        if (this->clockWiseKeyCode != NoneKeyCode && in_pKeyState[this->clockWiseKeyCode])
        {
            angular -= this->maxAngularSpeed;
        }

        // 反時計周りで回転のキー入力
        if (this->counterClockWiseKeyCode != NoneKeyCode &&
            in_pKeyState[this->counterClockWiseKeyCode])
        {
            angular += this->maxAngularSpeed;
        }
        this->SetAngularSpeed(angular);
    }
}