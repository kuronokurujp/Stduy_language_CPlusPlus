#include "Task.h"

#include "TaskManager.h"

static const Core::TaskData DEFAULT_TASK_DATA;

namespace Core
{
    /// <summary>
    /// タスク管理に登録する
    /// 登録に必要な情報を設定
    /// </summary>
    void Task::Setup(const Bool in_bReleaseMem)
    {
        this->_Clear();
        // パラメータはタスク管理に登録された時に設定される
        this->_bReleaseMem = in_bReleaseMem;
    }

    void Task::Kill()
    {
        // 死亡フラグを付ける
        _bKill = TRUE;
    }

    const Bool Task::End()
    {
        // 子タスクを全て破棄
        while (this->_lstChildTask.BeginItr() != this->_lstChildTask.EndItr())
        {
            auto it = this->_lstChildTask.BeginItr();

            auto pTask = this->_pManager->GetTask(it->GetHandle());

            // 親から外すのと同時にタスク破棄
            pTask->Kill();
            this->RemoveChildTask(&it->GetHandle());
        }

        return TRUE;
    }

    const Bool Task::AddChildTask(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        // 自分自身を子タスクとして設定できない
        if (in_rHandle == this->_hSelf) return FALSE;

        auto pTask = this->_pManager->GetTask(in_rHandle);
        HE_ASSERT(pTask);

        // 管理側でつながっているのであれば外す
        if (pTask->_bChild == FALSE)
        {
            // 子タスクを持っていないタスクのみ子タスクリストに追加できる
            // 子タスクを持っているタスクが追加できると更新が循環処理になってしまう事があるのでそれを防ぐための処置
            if (pTask->_lstChildTask.Empty() == FALSE)
            {
                HE_ASSERT(FALSE &&
                          "子タスクにしたいタスクの子タスクリストにいくつかのタスクがあるのでだめ");
                return FALSE;
            }

            this->_pManager->_Dettach(pTask);
            // 子のタスクに変わった
            pTask->_bChild = TRUE;
        }
        else
        {
            // すでに子タスクとなっているのであれば親タスクから外す
            auto pParentTask = this->_pManager->GetTask(pTask->_chainNode.GetParentHandle());
            HE_ASSERT(pParentTask);
            HE_ASSERT(in_rHandle == pTask->_chainNode.GetHandle());

            pParentTask->RemoveChildTask(&pTask->_chainNode.GetHandle());
        }

        // 子のタスク情報を作成してリストに追加
        pTask->_chainNode = ChildTaskNode(this->_hSelf, in_rHandle);

        // つける対象のタスクハンドルとつけるタスクのハンドルをリストに追加
        this->_lstChildTask.PushBack(pTask->_chainNode);

        return TRUE;
    }

    const Core::Common::CustomList<Task::ChildTaskNode>::Iterator Task::RemoveChildTask(
        Core::Common::Handle* in_pHandle)
    {
        HE_ASSERT(in_pHandle);
        HE_ASSERT(in_pHandle->Null() == FALSE);
        auto pTask = this->_pManager->GetTask(*in_pHandle);
        HE_ASSERT(pTask);

        if (pTask->_chainNode.Empty())
            return Core::Common::CustomList<Task::ChildTaskNode>::Iterator();
        HE_ASSERT(pTask->_chainNode.GetParentHandle() == this->_hSelf);

        // 子のタスクではなくなった
        pTask->_bChild = FALSE;
        // 親のリストから子のタスクを外す
        auto itr     = Core::Common::CustomList<ChildTaskNode>::Iterator(&pTask->_chainNode);
        auto nextItr = this->_lstChildTask.Erase(itr);

        // this->_chainNodeはクリアしない
        // なぜならリストのイテレータのノードなのでクリアするとイテレータが壊れるから

        // 破棄フラグがあればタスク管理で消去
        if (pTask->_bKill)
        {
            this->_pManager->RemoveTask(in_pHandle);
        }
        else
        {
            // タスク管理につけなおす
            this->_pManager->_Attach(pTask, pTask->GetGropuId());
        }

        return nextItr;
    }

    void Task::_UpdateChild(const Float32 in_fDt, const TaskData& in_rData)
    {
        for (auto it = this->_lstChildTask.BeginItr(); it != this->_lstChildTask.EndItr(); ++it)
        {
            auto pTask = this->_pManager->GetTask(it->GetHandle());
            if (pTask == NULL)
            {
                this->_lstChildTask.Erase(it);
                continue;
            }

            // 子タスクがまだ開始していない可能性がある
            if (pTask->_bStart)
            {
                if (pTask->Begin())
                {
                    pTask->_bStart = FALSE;
                }
            }

            pTask->Update(in_fDt, in_rData);
            pTask->_UpdateChild(in_fDt, in_rData);
        }
    }

}  // namespace Core
