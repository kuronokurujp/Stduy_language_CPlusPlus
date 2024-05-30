#pragma once

#include "MiniEngine.h"

#include "Core/Common/Handle.h"

namespace Actor
{
    // 前方宣言
    class Object;

    /// <summary>
    /// 管理クラスのデータ制御インターフェイス
    /// </summary>
    class ActorManagerlnterface
    {
    public:
        /// <summary>
        /// アクター取得
        /// </summary>
        /// <param name=""></param>
        /// <returns></returns>
        virtual Object* Get(const Core::Common::Handle&) = 0;

        /// <summary>
        /// Determines whether the specified in p actor is actor.
        /// </summary>
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
        virtual const Bool IsActor(const Core::Common::Handle&) = 0;

        /// <summary>
        /// Removes the actor.
        /// </summary>
        /// <param name="in_pActor">The in p actor.</param>
        /// <returns></returns>
        //void RemoveActor(Actor *in_pActor);
        virtual void Remove(const Core::Common::Handle&) = 0;
    };
}
