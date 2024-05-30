#include "ActorManager.h"
#include "Actor.h"

namespace Actor
{
    const Bool ActorManager::Init()
    {
        return this->_taskManager.Init(256, 32);
    }

    const Bool ActorManager::End()
    {
        this->_taskManager.End();
        return TRUE;
    }

    /// <summary>
    /// Removes the actor.
    /// </summary>
    /// <param name="in_pActor">The in p actor.</param>
    /// <returns></returns>
    // auto ActorManager::RemoveActor(Actor *in_pActor) -> void
    void ActorManager::Remove(const Core::Common::Handle &in_hActor)
    {
        E_ASSERT((in_hActor.Null() == FALSE) && "アクターを破棄するハンドルがない");

        this->_taskManager.Remove(in_hActor);
    }

    Object* ActorManager::Get(const Core::Common::Handle& in_hActor)
    {
        E_ASSERT((in_hActor.Null() == FALSE) && "アクターを破棄するハンドルがない");
        return reinterpret_cast<Object*>(this->_taskManager.GetTask(in_hActor));
    }

    /// <summary>
    /// Determines whether the specified in p actor is actor.
    /// </summary>
    /// <param name="in_pActor">The in p actor.</param>
    /// <returns></returns>
    const Bool ActorManager::IsActor(const Core::Common::Handle &in_hActor)
    {
        E_ASSERT((in_hActor.Null() == FALSE) && "チェックするアクターがない");
        return (this->_taskManager.GetTask(in_hActor) != NULL);

        /*

        int actorSize = static_cast<int>(this->actors.size());
        for (int i = 0; i < actorSize; ++i)
        {
            if (this->actors[i] == in_hActor)
            {
                return true;
            }
        }

        int pendingActorSize = static_cast<int>(this->pendingActors.size());
        for (int i = 0; i < pendingActorSize; ++i)
        {
            if (this->pendingActors[i] == in_hActor)
            {
                return true;
            }
        }

        return false;
        */
    }

    /// <summary>
    /// アクターに入力状態を送信
    /// </summary>
    void ActorManager::ProcessInput(const Float32 in_dt, Platform::InputSystemInterface* in_pInput)
    {
        Core::TaskData taskData
        {
            Actor::Object::ETaskUpdateId_Input,
            reinterpret_cast<void*>(in_pInput)
        };

        {
            const Uint32 max = this->_taskManager.GetMaxGroup();
            for (Uint32 i = 1; i < max; ++i)
            {
                this->_taskManager.UpdateGroup(i, in_dt, &taskData);
            }
        }
    }

    void ActorManager::Update(const Float32 in_dt)
    {
        // アクター処理内で新しいアクターが追加した場合は保留リストにまず登録させる
        this->_bUpdatingActors = TRUE;

        {
            Core::TaskData taskData
            {
                Actor::Object::ETaskUpdateId_Object,
                NULL
            };

            const Uint32 max = this->_taskManager.GetMaxGroup();
            for (Uint32 i = 1; i < max; ++i)
            {
                this->_taskManager.UpdateGroup(i, in_dt, &taskData);
            }
        }
        this->_bUpdatingActors = FALSE;
    }

    void ActorManager::UpdatePending()
    {
        // 保留グループに設定したタスクを更新グループに移動
        this->_taskManager.MoveGroupAll(0, 1);
    }
/*
    /// <summary>
    /// Gets the actors.
    /// </summary>
    /// <returns></returns>
    auto ActorManager::GetActors() -> const std::vector<class Actor *>
    {
        return this->actors;
    }

    /// <summary>
    /// Deletes the actors.
    /// </summary>
    /// <param name="in_rActors">The in r actors.</param>
    /// <param name="pValidate">The p validate.</param>
    /// <returns></returns>
    void ActorManager::DeleteAllActorsAndMemFree(std::vector<Actor *> &in_rActors, bool(*pValidate)(Actor *))
    {
        // 削除リストに登録する
        // 破棄するとリスト項目を外す処理が発生するのでループのリストが壊れるのを防ぐため
        std::vector<Actor *> deletes;
        for (auto actor : in_rActors)
        {
            if (pValidate == nullptr)
            {
                deletes.emplace_back(actor);
            }
            else if (pValidate(actor))
            {
                deletes.emplace_back(actor);
            }
        }

        for (auto actor = deletes.begin(); actor != deletes.end(); ++actor)
        {
            // リストを外す処理はいらない
            // Actor自身が破棄時にリストから外しているから
            SAFETY_MEM_RELEASE(*actor);
        }
    }

    void ActorManager::DeleteAllActorsAndMemFree()
    {
        this->DeleteAllActorsAndMemFree(this->pendingActors, nullptr);
        this->DeleteAllActorsAndMemFree(this->actors, nullptr);
    }
    */
}

