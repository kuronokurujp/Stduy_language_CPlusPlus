#include "ActorManager.h"

#include "Actor.h"

namespace Actor
{
    const Bool ActorManager::Start(const Uint32 in_uActorCapacity, const Uint32 in_uActorGroupMax)
    {
        HE_ASSERT(1 < in_uActorGroupMax);
        if (this->_taskManager.Init(in_uActorCapacity, in_uActorGroupMax) == FALSE) return FALSE;

        return TRUE;
    }

    const Bool ActorManager::End()
    {
        this->_taskManager.End();
        return TRUE;
    }

    /// <summary>
    /// Removes the actor.
    /// </summary>
    void ActorManager::Remove(const Core::Common::Handle& in_actorHandle)
    {
        HE_ASSERT((in_actorHandle.Null() == FALSE) && "アクターを破棄するハンドルがない");

        this->_taskManager.Remove(in_actorHandle);
    }

    Object* ActorManager::Get(const Core::Common::Handle& in_rActorHandle)
    {
        HE_ASSERT((in_rActorHandle.Null() == FALSE) && "アクターを破棄するハンドルがない");
        return reinterpret_cast<Object*>(this->_taskManager.GetTask(in_rActorHandle));
    }

    /// <summary>
    /// Determines whether the specified in p actor is actor.
    /// </summary>
    const Bool ActorManager::IsActor(const Core::Common::Handle& in_rActorHandle)
    {
        HE_ASSERT((in_rActorHandle.Null() == FALSE) && "チェックするアクターがない");
        return (this->_taskManager.GetTask(in_rActorHandle) != NULL);
    }

    /// <summary>
    /// アクターに入力状態を送信
    /// </summary>
    void ActorManager::ProcessInput(const Float32 in_uDt, Platform::InputSystemInterface* in_pInput)
    {
        {
            Core::TaskData taskData{Actor::Object::ETaskUpdateId_Input,
                                    reinterpret_cast<void*>(in_pInput)};

            const Uint32 max = this->_GetGroupMax();
            for (Uint32 i = 0; i < max; ++i)
            {
                this->_taskManager.UpdateGroup(i, in_uDt, taskData);
            }
        }
    }

    void ActorManager::Update(const Float32 in_uDt, const Core::TaskData& in_rTaskData)
    {
        // アクター処理内で新しいアクターが追加した場合は保留リストにまず登録させる
        this->_bUpdatingActors = TRUE;
        {
            const Uint32 uMax = this->_GetGroupMax();
            for (Uint32 i = 0; i < uMax; ++i)
            {
                this->_taskManager.UpdateGroup(i, in_uDt, in_rTaskData);
            }
        }
        this->_bUpdatingActors = FALSE;
    }

    void ActorManager::UpdatePending()
    {
        // 保留グループに設定したタスクを更新グループに移動
        for (auto b = this->_pendingDataMap.Begin(); b != this->_pendingDataMap.End(); ++b)
        {
            const auto pData = &b->_tData;
            const Bool bRet  = this->_taskManager.MoveGropuTask(pData->handle, pData->moveGroupId);
            HE_ASSERT(bRet && "保留中のタスクを稼働中に切り替え失敗した");
        }
        this->_pendingDataMap.Clear();
    }

    void ActorManager::MoveDepth(const Core::Common::Handle& in_rActorHandle,
                                 const Uint32 in_uDepth)
    {
        HE_ASSERT(in_uDepth < this->_taskManager.GetMaxGroup());
        const Sint32 iGroupId = static_cast<Sint32>(in_uDepth);

        const auto pTask = this->_taskManager.GetTask(in_rActorHandle);
        if (pTask->GetGropuId() == this->_GetPendingGroupId())
        {
            // 保留中のタスクなので移動させるグループIDのみ切り替える
            auto it = this->_pendingDataMap.Find(in_rActorHandle);
            HE_ASSERT(it.IsValid());
            it->_tData.moveGroupId = iGroupId;
        }
        else
        {
            // タスクのグループIDに変える
            this->_taskManager.MoveGropuTask(in_rActorHandle, iGroupId);
        }
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
        void ActorManager::DeleteAllActorsAndMemFree(std::vector<Actor *> &in_rActors,
       bool(*pValidate)(Actor *))
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
}  // namespace Actor
