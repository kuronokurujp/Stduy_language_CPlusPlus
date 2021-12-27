#pragma once

#include <vector>

namespace Actor
{
    // 前方宣言
    class Actor;

    class ActorManager
    {
    public:
        /// <summary>
        /// メモリ確保したアクターを登録.
        /// </summary>
        void AddActorMemData(Actor* in_pActor);

        void RemoveActor(Actor* in_pActor);

        const bool IsActor(Actor* in_pActor);

        const std::vector<Actor*> GetActors();

        /// <summary>
        /// 全てのアクターを破棄.
        /// アクターのメモリも同時に開放
        /// </summary>
        void DeleteAllActorsAndMemFree();

    protected:
        // Actor登録リスト
        std::vector<Actor*> _actors;

    private:
        void _Clear();
    };
}
