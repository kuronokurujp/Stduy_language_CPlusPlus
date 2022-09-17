#pragma once

#include "Component/Component.h"

namespace Component
{
    class SimpleCameraComponent : public Component
    {
    public:
        virtual void Begin() override {}
        virtual void End() override {}

        /// <summary>
        /// 毎フレーム更新.
        /// </summary>
        virtual void Update(const float in_deltaTimeSecond) override {}
    };
}
