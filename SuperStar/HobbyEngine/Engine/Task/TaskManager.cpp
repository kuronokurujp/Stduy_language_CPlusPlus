#include "TaskManager.h"

#include "Engine/Memory/Memory.h"
#include "Task.h"

namespace Core
{
    TaskManager::~TaskManager()
    {
        this->End();
    }

    const Bool TaskManager::Init(const Uint32 in_taskMax, const Sint32 in_groupNum)
    {
        // 管理グループ数が0以下なら失敗
        if (in_groupNum <= 0) return FALSE;

        // プールのバッファ予約
        this->_Reserve(in_taskMax);

        // タスクグループを確保
        this->_pTasks = HE_NEW_ARRAY(TaskGroup, in_groupNum, 0);
        for (Uint32 i = 0; i < in_groupNum; ++i)
        {
            // 先頭と終端タスクを作成して設定
            this->_pTasks[i]._pRootTask = HE_NEW(Task, 0);
            this->_pTasks[i]._pTailTask = this->_pTasks[i]._pRootTask;
            this->_pTasks[i]._uFlags    = 0;
        }
        HE_ASSERT(this->_pTasks && "タスクグループのメモリ確保に失敗");

        this->_iGroupNum = in_groupNum;

        return TRUE;
    }

    void TaskManager::End()
    {
        if (this->_pTasks == NULL) return;

        // タスクを全て削除
        this->RemoveAll();

        for (Uint32 i = 0; i < this->_iGroupNum; ++i)
        {
            // 先頭と終端タスクを作成して設定
            HE_SAFE_DELETE(this->_pTasks[i]._pRootTask);
        }

        // グループを削除
        HE_SAFE_DELETE_ARRAY(this->_pTasks);
    }

    void TaskManager::UpdateAll(const Float32 in_dt, const TaskData& in_rData)
    {
        for (Sint32 i = 0; i < this->_iGroupNum; ++i) this->UpdateGroup(i, in_dt, in_rData);
    }

    void TaskManager::UpdateGroup(const Sint32 in_groupId, const Float32 in_dt,
                                  const TaskData& in_rData)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);

        // 登録タスクを全更新
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        Task* pTask       = pGroup->_pRootTask;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_uFlags & FLAG_PAUSE) == FALSE)
        {
            while (pTask->_pNext)
            {
                pTask = pTask->_pNext;

                // 死亡フラグが付いてるのは更新しない
                if (pTask->_bKill) continue;

                // タスク実行開始
                // 一度しか呼ばれない
                if (pTask->_bStart)
                {
                    if (pTask->Begin())
                    {
                        pTask->_bStart = FALSE;
                    }
                }

                // タスク更新
                pTask->Update(in_dt, in_rData);
                pTask->_UpdateChild(in_dt, in_rData);
            }
        }

        // 更新が終わったタイミングで死亡フラグが付いたタスクを殺す
        pTask = pGroup->_pRootTask->_pNext;
        while (pTask != NULL)
        {
            // 次のタスクポインタを先に取得しておく
            Task* pNextTask = pTask->_pNext;

            // 死亡フラグが付いてたら消す
            if (pTask->_bKill)
            {
                this->RemoveTask(&pTask->_hSelf);
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::RemoveTask(Common::Handle* in_pTask)
    {
        // タスクのインスタンスを取得
        // 既に削除済みのハンドルだったらNULLが返される
        Task* pTask = this->GetTask(*in_pTask);
        if (pTask == NULL) return;

        // 終了を呼ぶ
        if (pTask->_bStart == FALSE) pTask->End();

        // タスクの連結を解除
        this->_Dettach(pTask);

        pTask->_pManager = NULL;

        // ハンドルを返却する + メモリから削除するかしないか
        const bool bCache = pTask->_bReleaseMem == FALSE;
        this->_Free(*in_pTask, bCache);

        in_pTask->Clear();
    }

    void TaskManager::RemoveGroup(Sint32 in_groupId)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);

        // 登録されているタスクを全て更新
        TaskGroup* pGroup = &_pTasks[in_groupId];
        Task* pTask       = pGroup->_pRootTask;
        pTask             = pGroup->_pRootTask->_pNext;
        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            this->RemoveTask(&pTask->_hSelf);

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::RemoveAll()
    {
        HE_ASSERT(this->_pTasks);

        // タスクを全て削除
        for (Sint32 i = 0; i < this->_iGroupNum; ++i) this->RemoveGroup(i);
    }

    const Bool TaskManager::MoveGroupAll(const Sint32 in_orgGroupId, const Sint32 in_targetGroupId)
    {
        HE_ASSERT(in_orgGroupId < this->_iGroupNum && "グループ元のグループID値が間違っている");
        HE_ASSERT(in_targetGroupId < this->_iGroupNum &&
                  "グループ移動させるグループID値が間違っている");

        // 付け替え元と付け替え先のグループIDが同じなので何もしない
        if (in_orgGroupId == in_targetGroupId) return FALSE;

        // 登録されているグループタスクを全て更新
        TaskGroup* pGroup = &_pTasks[in_orgGroupId];
        Task* pTask       = pGroup->_pRootTask;
        pTask             = pGroup->_pRootTask->_pNext;
        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            {
                // 一旦グループから削除する
                this->_Dettach(pTask);

                // 別のグループに付け替える
                this->_Attach(pTask, in_targetGroupId);
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }

        return TRUE;
    }

    const Bool TaskManager::MoveGropuTask(const Common::Handle& in_hTask, const Sint32 in_groupId)
    {
        HE_ASSERT(in_hTask.Null() == FALSE);

        Task* pTask = this->GetTask(in_hTask);
        HE_ASSERT(pTask);

        if (this->_Dettach(pTask) == Task::iNoneGroupId) return FALSE;

        if (this->_Attach(pTask, in_groupId) == FALSE) return FALSE;

        return TRUE;
    }

    Task* TaskManager::GetTask(const Common::Handle& in_hTask)
    {
        if (in_hTask.Null()) return NULL;

        // ハンドルからタスクインスタンスを引き出す
        Task* pTask = this->_Ref(in_hTask);
        if (pTask == NULL) return NULL;

        return pTask;
    }

    void TaskManager::EnableFlagWithGroup(const Sint32 in_groupId, const Uint32 in_flags)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);

        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_uFlags |= in_flags;
    }

    void TaskManager::DisableFlagWithGroup(const Sint32 in_groupId, const Uint32 in_flags)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_uFlags &= ~in_flags;
    }

    const Uint32 TaskManager::FlagWithGroup(const Sint32 in_groupId) const
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        return pGroup->_uFlags;
    }

    const Uint32 TaskManager::CountWithGroup(const Sint32 in_groupId) const
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        return pGroup->_uCount;
    }

    /// <summary>
    /// タスク追加する
    /// </summary>
    const Bool TaskManager::_Attach(Task* in_pTask, const Sint32 in_groupId)
    {
        HE_ASSERT(Task::iNoneGroupId < in_groupId);
        HE_ASSERT(in_groupId < this->_iGroupNum);

        HE_ASSERT((in_pTask != NULL) && "タスクに割り当ている数がいっぱいで割り当てに失敗");

        // アタッチ
        Task** ppTask        = &in_pTask;
        (*ppTask)->_iGroupId = in_groupId;

        // グループの終端に登録
        Task* pTailTask   = this->_pTasks[in_groupId]._pTailTask;
        pTailTask->_pNext = in_pTask;

        // タスクの初期化
        (*ppTask)->_pPrev    = pTailTask;
        (*ppTask)->_pNext    = NULL;
        (*ppTask)->_iGroupId = in_groupId;
        (*ppTask)->_bStart   = TRUE;
        (*ppTask)->_bKill    = FALSE;
        (*ppTask)->_pManager = this;

        // 新たな終端に据える
        this->_pTasks[in_groupId]._pTailTask = in_pTask;

        ++this->_pTasks[in_groupId]._uCount;

        return TRUE;
    }

    /// <summary>
    /// タスクをグループから外す
    /// </summary>
    const Sint32 TaskManager::_Dettach(Task* in_pTask)
    {
        // タスクの前後付け替え
        Task* pPrevTask = in_pTask->_pPrev;
        Task* pNextTask = in_pTask->_pNext;

        // タスクの前後連結情報がない場合はタスクのつけ外しはできないので終了
        if ((pPrevTask == NULL) && (pNextTask == NULL)) return Task::iNoneGroupId;

        if (pPrevTask != NULL) pPrevTask->_pNext = pNextTask;
        if (pNextTask != NULL) pNextTask->_pPrev = pPrevTask;

        // 今回削除するのが終端タスクか
        Sint32 iGroupId = in_pTask->_iGroupId;
        if (iGroupId != Task::iNoneGroupId)
        {
            if (0 <= iGroupId && iGroupId < this->_iGroupNum)
            {
                TaskGroup* pGroup = &this->_pTasks[iGroupId];
                if (pGroup->_pTailTask == in_pTask)
                    // つなぎなおす
                    pGroup->_pTailTask = pPrevTask;
            }

            --this->_pTasks[iGroupId]._uCount;
        }

        return iGroupId;
    }
}  // namespace Core
