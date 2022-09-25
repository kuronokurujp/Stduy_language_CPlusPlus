#include "Actor/ActorMiniHeader.h"

#include "Common.h"
#include "Component/ActorComponent.h"

namespace Actor
{
    void ActorManager::AddActor(Actor* in_pActor)
    {
        this->_actors.emplace_back(in_pActor);

        // アクター用のコンポーネントを追加
        auto actor_component = std::make_shared<Component::ActorComponent>(this, in_pActor);
        in_pActor->AddComponentMemData(actor_component);
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
        std::vector<Actor*> deletes;
        for (auto actor : this->_actors)
        {
            deletes.emplace_back(actor);
        }

        for (auto actor = deletes.begin(); actor != deletes.end(); ++actor)
        {
            // 破棄前にアクターの終了メソッドが呼ばれていない場合は呼び出す
            // 終了メソッドはメモリ、リソース解放をしているので必ず呼ばないとメモリリークになる可能性がある
            if ((*actor)->GetState() != Actor::eState::eState_Dead)
                (*actor)->End();

            SAFETY_MEM_RELEASE(*actor);
        }

        this->_actors.clear();
    }

    void ActorManager::Tick(float in_deltaTimeSecond)
    {
        for (auto actor : this->_actors)
        {
            if (actor->GetState() == Actor::eState::eState_Begin)
            {
                actor->Begin();
                actor->SetState(Actor::eState::eState_Active);
            }
            actor->Tick(in_deltaTimeSecond);
        }
    }

    void ActorManager::Render()
    {
        for (auto actor : this->_actors)
            actor->Render();
    }

    void ActorManager::_Clear()
    {
        this->_actors.clear();
    }
}
