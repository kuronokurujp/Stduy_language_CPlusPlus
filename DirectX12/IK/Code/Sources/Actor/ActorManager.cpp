#include "Actor/ActorMiniHeader.h"

#include "Common.h"

namespace Actor
{
    void ActorManager::AddActor(Actor* in_pActor)
    {
        this->_actors.emplace_back(in_pActor);
    }

    void ActorManager::RemoveActor(Actor* in_pActor)
    {
        Common::SafeRemoveObject<Actor>(in_pActor, this->_actors);
    }

    const bool ActorManager::IsActor(class Actor* in_pActor)
    {
        int actorSize = static_cast<int>(this->_actors.size());
        for (int i = 0; i < actorSize; ++i)
        {
            if (this->_actors[i] == in_pActor)
                return true;
        }

        return false;
    }

    const std::vector<class Actor*> ActorManager::GetActors()
    {
        return this->_actors;
    }

    /// <summary>
    /// 全てのアクターを破棄.
    /// </summary>
    void ActorManager::DeleteAllActorsAndMemFree()
    {
        // 削除リストに登録する
        // 破棄するとリスト項目を外す処理が発生するのでループのリストが壊れるのを防ぐため
        std::vector<Actor*> deletes;
        for (auto actor : this->_actors)
        {
            deletes.emplace_back(actor);
        }

        for (auto actor = deletes.begin(); actor != deletes.end(); ++actor)
            SAFETY_MEM_RELEASE(*actor);

        this->_actors.clear();
    }

    void ActorManager::_Clear()
    {
        this->_actors.clear();
    }
}
