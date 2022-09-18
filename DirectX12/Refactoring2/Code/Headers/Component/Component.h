#pragma once

#include <stdio.h>
#include <cstdint>

// 前方宣言
namespace Actor { class Actor; }

namespace Component
{
    /// <summary>
    /// Actorクラスにくっつけるコンポーネント.
    /// </summary>
    class Component
    {
    public:
        Component(Actor::Actor* in_pOwner);
        virtual ~Component() {}

        virtual void Begin() {}
        virtual void End() {}

        /// <summary>
        /// 毎フレーム更新.
        /// </summary>
        virtual void Update(const float in_deltaTimeSecond) {}

    protected:
        Actor::Actor* _pOwner;

    private:
        void _Clear();
    };
}
