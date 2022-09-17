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

        virtual void Begin() = 0;
        virtual void End() = 0;

        /// <summary>
        /// 毎フレーム更新.
        /// </summary>
        virtual void Update(const float in_deltaTimeSecond) = 0;

    protected:
        Actor::Actor* _pOwner;

    private:
        void _Clear();
    };
}
