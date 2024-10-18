#include "ActorManager.h"

#include "Actor.h"

namespace Actor
{
    ActorManager::ActorManager(
        Core::Memory::UniquePtr<ActorManagerDecoraterlnterface> in_upDecorator)
        : _upDecorator(std::move(in_upDecorator))
    {
    }

    Bool ActorManager::Start(const Uint32 in_uActorCapacity, const Uint32 in_uActorGroupMax)
    {
        HE_ASSERT(1 < in_uActorGroupMax);
        if (this->_taskManager.Init(in_uActorCapacity, in_uActorGroupMax) == FALSE) return FALSE;

        if (this->_upDecorator)
        {
            if (this->_upDecorator->VStart(this) == FALSE) return TRUE;
        }

        return TRUE;
    }

    Bool ActorManager::End()
    {
        this->_taskManager.End();

        return TRUE;
    }

    /// <summary>
    /// Removes the actor.
    /// </summary>
    void ActorManager::Remove(Core::Common::Handle* in_pActorHandle)
    {
        HE_ASSERT(in_pActorHandle);
        HE_ASSERT((in_pActorHandle->Null() == FALSE) && "アクターを破棄するハンドルがない");

        // 入力コンポーネントがついている場合は登録リストから外す
        auto pObject = this->Get(*in_pActorHandle);

        // タスクの全削除処理ですでにタスクが存在しないケースがある
        if (pObject == NULL)
        {
            in_pActorHandle->Clear();
            return;
        }

        auto components = pObject->GetComponents();
        for (auto itr = components->begin(); itr != components->end(); ++itr)
        {
            if (itr->first.Null()) continue;

            Component* pTarget = reinterpret_cast<Component*>(itr->second);
            HE_ASSERT(pTarget);
            this->VOnActorUnRegistComponent(pTarget);
        }

        this->_taskManager.RemoveTask(in_pActorHandle);
    }

    Object* ActorManager::Get(const Core::Common::Handle& in_rActorHandle)
    {
        HE_ASSERT((in_rActorHandle.Null() == FALSE) && "アクターを破棄するハンドルがない");
        return reinterpret_cast<Object*>(this->_taskManager.GetTask(in_rActorHandle));
    }

    void ActorManager::VOnActorRegistComponent(Component* in_pComponent)
    {
        if (this->_upDecorator == NULL) return;

        this->_upDecorator->VOnActorRegistComponent(in_pComponent);
    }

    void ActorManager::VOnActorUnRegistComponent(Component* in_pComponent)
    {
        if (this->_upDecorator == NULL) return;

        this->_upDecorator->VOnActorUnRegistComponent(in_pComponent);
    }

    void ActorManager::BeginUpdate(const Float32 in_fDt)
    {
    }

    void ActorManager::Update(const Float32 in_fDt)
    {
        // アクター処理内で新しいアクターが追加した場合は保留リストにまず登録させる
        this->_bUpdatingActors = TRUE;
        {
            const Uint32 uMax = this->_GetUpdateGroupMax();
            for (Uint32 i = 0; i < uMax; ++i)
            {
                this->_taskManager.UpdateByGroup(i, in_fDt);
            }
        }
        this->_bUpdatingActors = FALSE;
    }

    void ActorManager::LateUpdate(const Float32 in_fDt)
    {
        this->_UpdatePending();

        if (this->_upDecorator) this->_upDecorator->VLateUpdate(in_fDt, this);
    }

    void ActorManager::Event(const Core::TaskData& in_rTaskData)
    {
        this->_taskManager.EventAll(in_rTaskData);
    }

    void ActorManager::_UpdatePending()
    {
        // 保留グループに設定したタスクを更新グループに移動
        for (auto b = this->_pendingDataMap.Begin(); b != this->_pendingDataMap.End(); ++b)
        {
            const auto pData = &b->data;
            const Bool bRet  = this->_taskManager.MoveGropuTask(pData->handle, pData->sMoveGroupId);
            HE_ASSERT(bRet && "保留中のタスクを稼働中に切り替え失敗した");
        }
        this->_pendingDataMap.Clear();
    }

}  // namespace Actor
