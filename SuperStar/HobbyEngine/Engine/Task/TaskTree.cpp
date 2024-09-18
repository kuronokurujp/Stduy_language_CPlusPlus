#include "TaskTree.h"

#include "TaskManager.h"

namespace Core
{
    void TaskTree::VEvent(const TaskData& in_rEvent)
    {
        this->_EachbyChildTask([this, in_rEvent](Task* in_pTask) { in_pTask->VEvent(in_rEvent); });
    }

    void TaskTree::_VDestory()
    {
        // 子タスクを全て破棄
        while (this->_lstChildTask.BeginItr() != this->_lstChildTask.EndItr())
        {
            auto it = this->_lstChildTask.BeginItr();

            auto pTask = this->_pTaskManager->GetTask(it->GetHandle());

            // 親から外すのと同時にタスク破棄
            pTask->Kill();
            this->RemoveChildTask(&it->GetHandle());
        }
    }

    Bool TaskTree::AddChildTask(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        // 自分自身を子タスクとして設定できない
        if (in_rHandle == this->_selfHandle) return FALSE;

        auto pTaskTree = this->_GetTaskTree(in_rHandle);
        // 管理側でつながっているのであれば外す
        if (pTaskTree->_bChild == FALSE)
        {
            // 子タスクを持っていないタスクのみ子タスクリストに追加できる
            // 子タスクを持っているタスクが追加できると更新が循環処理になってしまう事があるのでそれを防ぐための処置
            if (pTaskTree->_lstChildTask.Empty() == FALSE)
            {
                HE_ASSERT(FALSE &&
                          "子タスクにしたいタスクの子タスクリストにいくつかのタスクがあるのでだめ");
                return FALSE;
            }

            this->_pTaskManager->_Dettach(pTaskTree);
            // 子のタスクに変わった
            pTaskTree->_bChild = TRUE;
        }
        else
        {
            // すでに子タスクとなっているのであれば親タスクから外す
            auto pParentTask =
                this->_pTaskManager->GetTask(pTaskTree->_chainNode.GetParentHandle());
            HE_ASSERT(pParentTask);
            HE_ASSERT(in_rHandle == pTaskTree->_chainNode.GetHandle());

            HE_ASSERT(HE_GENERATED_CHECK_RTTI(*pParentTask, TaskTree));
            auto pParentTaskTree = reinterpret_cast<TaskTree*>(pParentTask);

            pParentTaskTree->RemoveChildTask(&pTaskTree->_chainNode.GetHandle());
        }

        // 子のタスク情報を作成してリストに追加
        pTaskTree->_chainNode = ChildTaskNode(this->_selfHandle, in_rHandle);

        // つける対象のタスクハンドルとつけるタスクのハンドルをリストに追加
        this->_lstChildTask.PushBack(pTaskTree->_chainNode);

        return TRUE;
    }

    const TaskTree::ChildTaskNodeIterator TaskTree::RemoveChildTask(
        Core::Common::Handle* in_pHandle)
    {
        HE_ASSERT(in_pHandle);
        HE_ASSERT(in_pHandle->Null() == FALSE);

        auto pTaskTree = this->_GetTaskTree(*in_pHandle);
        if (pTaskTree->_chainNode.Empty()) return ChildTaskNodeIterator();

        HE_ASSERT(pTaskTree->_chainNode.GetParentHandle() == this->_selfHandle);

        // 子のタスクではなくなった
        pTaskTree->_bChild = FALSE;
        // 親のリストから子のタスクを外す
        auto nextItr = this->_lstChildTask.Erase(&pTaskTree->_chainNode);

        // 破棄フラグがあればタスク管理で消去
        if (pTaskTree->_bKill)
        {
            this->_pTaskManager->RemoveTask(in_pHandle);
        }
        else
        {
            // タスク管理につけなおす
            this->_pTaskManager->_Attach(pTaskTree, pTaskTree->GetGropuId());
        }

        return nextItr;
    }

    void TaskTree::VSetup(const Bool in_bAutoDelete)
    {
        Task::VSetup(in_bAutoDelete);
        this->_Clear();
    }

    void TaskTree::VUpdate(const Float32 in_fDt)
    {
        // 子タスクの実行
        this->_EachbyChildTask(
            [this, in_fDt](Task* in_pTask)
            {
                // 子タスクがまだ開始していない可能性がある
                if (in_pTask->_bStart)
                {
                    if (in_pTask->VBegin())
                    {
                        in_pTask->_bStart = FALSE;
                    }
                }

                in_pTask->VUpdate(in_fDt);
            });
    }

    TaskTree* TaskTree::_GetTaskTree(const Core::Common::Handle& in_rHandle)
    {
        auto pTask = this->_pTaskManager->GetTask(in_rHandle);
        HE_ASSERT(pTask);
        HE_ASSERT(HE_GENERATED_CHECK_RTTI(*pTask, TaskTree));

        auto pTaskTree = reinterpret_cast<TaskTree*>(pTask);
        HE_ASSERT(pTaskTree);

        return pTaskTree;
    }

    void TaskTree::_EachbyChildTask(std::function<void(Task*)> in_action)
    {
        for (auto it = this->_lstChildTask.BeginItr(); it != this->_lstChildTask.EndItr(); ++it)
        {
            auto pTask = this->_pTaskManager->GetTask(it->GetHandle());
            if (pTask == NULL)
            {
                this->_lstChildTask.Erase(it);
                continue;
            }

            in_action(pTask);
        }
    }

}  // namespace Core
