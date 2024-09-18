#include "TaskManager.h"

#include "Engine/Memory/Memory.h"
#include "Task.h"

namespace Core
{
    namespace Local
    {
        /// <summary>
        /// ルート用タスク
        /// </summary>
        class RootTask final : public Task
        {
            HE_CLASS_COPY_NG(RootTask);
            HE_CLASS_MOVE_NG(RootTask);
            HE_GENERATED_CLASS_BODY_HEADER(RootTask, Task);

        public:
            RootTask() : Task() {}

            /// <summary>
            /// タスク開始
            /// </summary>
            Bool VBegin() override final { return TRUE; }

            /// <summary>
            /// タスク終了
            /// </summary>
            Bool VEnd() override final { return TRUE; }

            /// <summary>
            /// 更新
            /// </summary>
            void VUpdate(const Float32) override final {}
        };

    }  // namespace Local

    TaskManager::~TaskManager()
    {
        this->End();
    }

    Bool TaskManager::Init(const Uint32 in_taskMax, const Sint32 in_groupNum)
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
            this->_pTasks[i]._pRootTask = HE_NEW(Local::RootTask, 0);
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

    void TaskManager::UpdateAll(const Float32 in_fDt)
    {
        for (Sint32 i = 0; i < this->_iGroupNum; ++i) this->UpdateByGroup(i, in_fDt);
    }

    void TaskManager::UpdateByGroup(const Sint32 in_sGroupId, const Float32 in_fDt)
    {
        HE_ASSERT(in_sGroupId < this->_iGroupNum);

        // 登録タスクを全更新
        TaskGroup* pGroup = &this->_pTasks[in_sGroupId];
        Task* pTask       = pGroup->_pRootTask;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_uFlags & uFlagPause) == FALSE)
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
                    if (pTask->VBegin())
                    {
                        pTask->_bStart = FALSE;
                    }
                }

                // タスク更新
                pTask->VUpdate(in_fDt);
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
                this->RemoveTask(&pTask->_selfHandle);
            }

            // 逃がしておいた次のポインタを得る
            pTask = pNextTask;
        }
    }

    void TaskManager::EventAll(const TaskData& in_rTaskData)
    {
        for (Sint32 i = 0; i < this->_iGroupNum; ++i) this->EventByGroup(i, in_rTaskData);
    }

    void TaskManager::EventByGroup(const Sint32 in_sGroupId, const TaskData& in_rTaskData)
    {
        HE_ASSERT(in_sGroupId < this->_iGroupNum);

        TaskGroup* pGroup = &this->_pTasks[in_sGroupId];
        Task* pTask       = pGroup->_pRootTask;

        // グループにポーズフラグが付いてるなら何もしない
        if ((pGroup->_uFlags & uFlagPause) == FALSE)
        {
            while (pTask->_pNext)
            {
                pTask = pTask->_pNext;

                if (pTask->_bKill) continue;
                if (pTask->_bStart) continue;

                pTask->VEvent(in_rTaskData);
            }
        }
    }

    void TaskManager::RemoveTask(Common::Handle* in_pTask)
    {
        // タスクのインスタンスを取得
        // 既に削除済みのハンドルだったらNULLが返される
        Task* pTask = this->GetTask(*in_pTask);
        if (pTask == NULL) return;

        // 終了を呼ぶ
        if (pTask->_bStart == FALSE) pTask->VEnd();
        // タスクを破棄
        pTask->_VDestory();

        // タスクの連結を解除
        this->_Dettach(pTask);

        pTask->_pTaskManager = NULL;

        // ハンドルを返却する + メモリから削除するかしないか
        const Bool bCache = pTask->_bReleaseMem == FALSE;
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
            this->RemoveTask(&pTask->_selfHandle);

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

    Bool TaskManager::MoveGroupAll(const Sint32 in_orgGroupId, const Sint32 in_targetGroupId)
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

    Bool TaskManager::MoveGropuTask(const Common::Handle& in_hTask, const Sint32 in_groupId)
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

    void TaskManager::EnableFlag(const Sint32 in_groupId, const Uint32 in_flags)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);

        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_uFlags |= in_flags;
    }

    void TaskManager::DisableFlag(const Sint32 in_groupId, const Uint32 in_flags)
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        pGroup->_uFlags &= ~in_flags;
    }

    Uint32 TaskManager::Flag(const Sint32 in_groupId) const
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        return pGroup->_uFlags;
    }

    Uint32 TaskManager::Count(const Sint32 in_groupId) const
    {
        HE_ASSERT(in_groupId < this->_iGroupNum);
        TaskGroup* pGroup = &this->_pTasks[in_groupId];
        return pGroup->_uCount;
    }

    /// <summary>
    /// タスク追加する
    /// </summary>
    Bool TaskManager::_Attach(Task* in_pTask, const Sint32 in_groupId)
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
        (*ppTask)->_pPrev        = pTailTask;
        (*ppTask)->_pNext        = NULL;
        (*ppTask)->_iGroupId     = in_groupId;
        (*ppTask)->_bStart       = TRUE;
        (*ppTask)->_bKill        = FALSE;
        (*ppTask)->_pTaskManager = this;

        // 新たな終端に据える
        this->_pTasks[in_groupId]._pTailTask = in_pTask;

        ++this->_pTasks[in_groupId]._uCount;

        return TRUE;
    }

    /// <summary>
    /// タスクをグループから外す
    /// </summary>
    Sint32 TaskManager::_Dettach(Task* in_pTask)
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
