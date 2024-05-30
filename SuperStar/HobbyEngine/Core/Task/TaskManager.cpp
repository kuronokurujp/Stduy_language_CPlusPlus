﻿#include "TaskManager.h"
#include "Task.h"

#include "Core/Memory/Memory.h"

namespace Core
{

    TaskManager::TaskGroup::TaskGroup()
    {
        // 先頭と終端タスクを作成して設定
        this->_pRootTask = new Task();
        this->_pTailTask = this->_pRootTask;
        this->_flags = 0;
    }

    TaskManager::TaskGroup::~TaskGroup()
    {
        E_SAFE_DELETE(this->_pRootTask);
    }

    TaskManager::~TaskManager()
    {
        this->End();
    }

    const Bool TaskManager::Init(const Uint32 in_taskMax, const Sint32 in_groupNum)
    {
        // 管理グループ数が0以下なら失敗
        if (in_groupNum <= 0)
            return FALSE;

        // プールのバッファ予約
        this->Reserve(in_taskMax);
        
        // タスクグループを確保
        this->_pTasks = new TaskGroup[in_groupNum];
        E_ASSERT(this->_pTasks && "タスクグループのメモリ確保に失敗");

        this->_groupNum = in_groupNum;
        
        return TRUE;
    }

    void TaskManager::End()
    {
        if (this->_pTasks == NULL)
            return;

        // タスクを全て削除
        this->RemoveAll();

        // グループを削除
        E_SAFE_DELETE_ARRAY(this->_pTasks);
    }

    void TaskManager::UpdateAll(const Float32 in_dt, const TaskData* in_pData)
    {
        for (Sint32 i = 0; i < this->_groupNum; ++i)
            this->UpdateGroup(i, in_dt, in_pData);
    }

    void TaskManager::UpdateGroup(const Sint32 in_groupId, const Float32 in_dt, const TaskData* in_pData)
    {
        E_ASSERT(in_groupId < this->_groupNum);
        
        // 登録タスクを全更新
        TaskGroup *pGroup = &this->_pTasks[in_groupId];
        Task *pTask = pGroup->_pRootTask;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_flags & FLAG_PAUSE) == FALSE)
        {
            while (pTask->_pNext)
            {
                pTask = pTask->_pNext;
                
                // 死亡フラグが付いてるのは更新しない
                if (pTask->_bKill)
                    continue;

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
                pTask->Update(in_dt, in_pData);
            }
        }
        
        // 更新が終わったタイミングで死亡フラグが付いたタスクを殺す
        pTask = pGroup->_pRootTask->_pNext;
        while (pTask != NULL)
        {
            // 次のタスクポインタを先に取得しておく
            Task *pNextTask = pTask->_pNext;
            
            // 死亡フラグが付いてたら消す
            if (pTask->_bKill)
            {
                this->Remove(pTask->GetHandle());
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::Remove(const Common::Handle& in_hTask)
    {
        // タスクのインスタンスを取得
        // 既に削除済みのハンドルだったらNULLが返される
        Task* pTask = this->GetTask(in_hTask);
        if (pTask == NULL)
            return;

        // 終了を呼ぶ
        pTask->End();

        Sint32 groupId = this->_Dettach(pTask);
        if (groupId == NON_GROPU_ID)
            return;

        // タスクがメモリ削除されると,
        // タスク内にある解放ハンドルも一緒に消えるのでコピーして保存
        Common::Handle freeHandle = in_hTask;

        // ハンドルを返却する + メモリから削除するかしないか
        this->Free(freeHandle, pTask->_bAutoDelete);
    }

    void TaskManager::RemoveGroup(Sint32 in_groupId)
    {
        E_ASSERT(in_groupId < this->_groupNum);
        
        // 登録されているタスクを全て更新
        TaskGroup* pGroup = &_pTasks[in_groupId];
        Task* pTask = pGroup->_pRootTask;
        pTask = pGroup->_pRootTask->_pNext;
        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            this->Remove(pTask->GetHandle());
            
            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::RemoveAll()
    {
        E_ASSERT(this->_pTasks);

        // タスクを全て削除
        for (Sint32 i = 0; i < this->_groupNum; ++i)
            this->RemoveGroup(i);
    }

    const Bool TaskManager::MoveGroupAll(const Sint32 in_orgGroupId, const Sint32 in_targetGroupId)
    {
        E_ASSERT(in_orgGroupId < this->_groupNum && "グループ元のグループID値が間違っている");
        E_ASSERT(in_targetGroupId < this->_groupNum && "グループ移動させるグループID値が間違っている");

        // 付け替え元と付け替え先のグループIDが同じなので何もしない
        if (in_orgGroupId == in_targetGroupId)
            return FALSE;

        // 登録されているグループタスクを全て更新
        TaskGroup* pGroup = &_pTasks[in_orgGroupId];
        Task* pTask = pGroup->_pRootTask;
        pTask = pGroup->_pRootTask->_pNext;
        while (pTask)
        {
            // RemoveでタスクのpPrev/pNextの入れ替えが起きてしまうので,
            // 次に実行するタスクを事前に取得しておく
            Task* pNextTask = pTask->_pNext;
            {
                // 一旦グループから削除する
                // 削除がするがメモリ解放はしてはいけないので自動解放をOFFにする
                Bool bOldAutoDelete = pTask->_bAutoDelete;

                // pTask->_bAutoDelete = FALSE;
                // this->Remove(pTask->GetHandle());
                this->_Dettach(pTask);

                // 別のグループに付け替える
                // pTask->_bAutoDelete = bOldAutoDelete;
                this->_Attach(pTask, in_targetGroupId);
            }
            
            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }

        return TRUE;
    }

    Task* TaskManager::GetTask(const Common::Handle& in_hTask)
    {
        if (in_hTask.Null())
            return NULL;

        // ハンドルからタスクインスタンスを引き出す
        Task* pTask = this->_Ref(in_hTask);
        if (pTask == NULL)
            return NULL;

        return pTask;
    }

    void TaskManager::EnableFlag(const Sint32 in_groupId, const Uint32 in_flags)
    {
        E_ASSERT(in_groupId < this->_groupNum);

        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_flags |= in_flags;
    }

    void TaskManager::DisableFlag(const Sint32 in_groupId, const Uint32 in_flags)
    {
        E_ASSERT(in_groupId < this->_groupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_flags &= ~in_flags;
    }

    const Uint32 TaskManager::Flag(const Sint32 in_groupId) const
    {
        E_ASSERT(in_groupId < this->_groupNum);
        TaskGroup *pGroup = &this->_pTasks[in_groupId];
        return pGroup->_flags;
    }

    const Uint32 TaskManager::CountWithGroup(const Sint32 in_groupId) const
    {
        E_ASSERT(in_groupId < this->_groupNum);
        TaskGroup *pGroup = &this->_pTasks[in_groupId];
        return pGroup->_count;
    }

    Task* TaskManager::_SetupTask(BasePoolManager::AllocData* out_pData, const Bool in_bAutoDelete)
    {
        E_ASSERT(out_pData != NULL);
        Task* pTask = out_pData->_pItem;

        pTask->Init(in_bAutoDelete);
        pTask->_hSelf = out_pData->_handle;

        return pTask;
    }

    /// <summary>
    /// タスク追加する
    /// </summary>
    const Bool TaskManager::_Attach(Task* in_pTask, const Sint32 in_groupId)
    {
        E_ASSERT(NON_GROPU_ID < in_groupId);
        E_ASSERT(in_groupId < this->_groupNum);
        
        E_ASSERT((in_pTask != NULL) && "タスクに割り当ている数がいっぱいで割り当てに失敗");

        // アタッチ
        Task** ppTask = &in_pTask;
        (*ppTask)->_groupId = in_groupId;

        // グループの終端に登録
        Task* pTailTask = this->_pTasks[in_groupId]._pTailTask;
        pTailTask->_pNext = in_pTask;

        // タスクの初期化
        (*ppTask)->_pPrev = pTailTask;
        (*ppTask)->_pNext = NULL;
        (*ppTask)->_groupId = in_groupId;
        (*ppTask)->_bStart = TRUE;
        (*ppTask)->_bKill = FALSE;
        (*ppTask)->_pManager = this;

        // 新たな終端に据える
        this->_pTasks[in_groupId]._pTailTask = in_pTask;

        ++this->_pTasks[in_groupId]._count;

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
        if (pPrevTask != NULL)
            pPrevTask->_pNext = pNextTask;

        if (pNextTask != NULL)
            pNextTask->_pPrev = pPrevTask;
        
        // 今回削除するのが終端タスクか
        Sint32 groupId = in_pTask->_groupId;
        if (0 <= groupId && groupId < this->_groupNum)
        {
            TaskGroup *pGroup = &this->_pTasks[groupId];
            if (pGroup->_pTailTask == in_pTask)
                // つなぎなおす
                pGroup->_pTailTask = pPrevTask;
        }

        in_pTask->_pManager = NULL;
        --this->_pTasks[groupId]._count;

        return groupId;
    }
}
