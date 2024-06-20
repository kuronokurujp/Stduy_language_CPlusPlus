#ifndef __INPUTCOMPONENT_H__
#define __INPUTCOMPONENT_H__

#include "moveComponent.h"

class Actor;

class InputComponent : public MoveComponent
{
public:
    /// <summary>
    /// Initializes a new instance of the <see cref="InputComponent"/> class.
    /// </summary>
    /// <param name="in_pOwner">The in p owner.</param>
    InputComponent(Actor* in_pOwner);

    // 各変数のGetter/Setter
    const float GetMaxForwardSpeed() const { return this->maxForwardSpeed; }
    const float GetMaxAngularSpeed() const { return this->maxAngularSpeed; }
    const int GetForwardKey() const { return this->forwardKeyCode; }
    const int GetBackKey() const { return this->backKeyCode; }
    const int GetClockWiseKey() const { return this->clockWiseKeyCode; }
    const int GetCounterClockWiseKey() const { return this->counterClockWiseKeyCode; }

    void SetMaxForwardSpeed(const float in_speed) { this->maxForwardSpeed = in_speed; }
    void SetMaxAngularSpeed(const float in_speed) { this->maxAngularSpeed = in_speed; }
    void SetForwardKeyCode(const int in_key) { this->forwardKeyCode = in_key; }
    void SetBackKeyCode(const int in_key) { this->backKeyCode = in_key; }
    void SetClockWiseKeyCode(const int in_key) { this->clockWiseKeyCode = in_key; }
    void SetCounterClockWiseKeyCode(const int in_key) { this->counterClockWiseKeyCode = in_key; }

    /// <summary>
    /// Processes the input.
    /// </summary>
    /// <param name="in_pKeyState">State of the in p key.</param>
    /// <returns></returns>
    void ProcessInput(const uint8_t* in_pKeyState) override final;

private:
    /// <summary>
    /// Clears this instance.
    /// </summary>
    /// <returns></returns>
    void _Clear()
    {
        this->maxAngularSpeed         = 0.0f;
        this->maxForwardSpeed         = 0.0f;
        this->forwardKeyCode          = NoneKeyCode;
        this->backKeyCode             = NoneKeyCode;
        this->clockWiseKeyCode        = NoneKeyCode;
        this->counterClockWiseKeyCode = NoneKeyCode;
    }

    // 未指定のキー値
    static const int NoneKeyCode;

    float maxForwardSpeed;
    float maxAngularSpeed;
    int forwardKeyCode;
    int backKeyCode;
    // 時計周りのキー
    int clockWiseKeyCode;
    // 反時計周りのキー
    int counterClockWiseKeyCode;
};

#endif  //__INPUTCOMPONENT_H__