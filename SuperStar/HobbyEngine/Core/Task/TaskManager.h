#pragma once

#include "MiniEngine.h"
#include "Core/Common/PoolManager.h"
#include "Core/Common/FixArray.h"

namespace Core
{
    // 前方宣言
    class Task;
    struct TaskData;

    /// <summary>
    /// タスク管理
    /// プールデータクラスを継承してタスク個数を固定にしている
    /// </summary>
    class TaskManager : public Common::BasePoolManager<Task>
    {
        E_CLASS_COPY_CONSTRUCT_NG(TaskManager)

    public:
        // タスクマネージャが使用するフラグ
        // 処理を停止するフラグ
        static const Uint32 FLAG_PAUSE = 0x00000001;

        // グループの未使用ID値
        static const Sint32 NON_GROPU_ID = -1;

    public:
        TaskManager() : BasePoolManager() {}
        ~TaskManager();
        
        /// <summary>
        /// 初期化
        /// 利用前に必ず呼び出す
        /// </summary>
        /// <param name="in_taskMax">タスクの総数</param>
        /// <param name="in_groupNum">グループ数</param>
        /// <returns></returns>
        const Bool Init(const Uint32 in_taskMax, const Sint32 in_groupNum);
        
        void End();
        
        /// <summary>
        /// 全タスク更新
        /// </summary>
        void UpdateAll(const Float32 in_dt, const TaskData*);
        
        /// <summary>
        /// 指定グループを更新
        /// </summary>
        void UpdateGroup(const Sint32 in_groupId, const Float32 in_dt, const TaskData*);
        
        /// <summary>
        /// タスク作成して追加する
        /// 結果はハンドルで返す
        /// </summary>
        /// <param name="in_groupId">追加するグループID</param>
        /// <returns></returns>
        template<class T>
        Common::Handle CreateAndAdd(const Sint32 in_groupId, const Bool in_bAutoDelete)
        {
            E_ASSERT(in_groupId < this->_groupNum);

            // 利用するタスクを割り当て
            BasePoolManager::AllocData resAlloc = this->Alloc<T>();

            Task* pTask = this->_SetupTask(&resAlloc, in_bAutoDelete);
            this->_Attach(pTask, in_groupId);

            return resAlloc._handle;
        }
        
        /// <summary>
        /// タスクの削除
        /// </summary>
        /// <param name="in_hTask">Addで取得したハンドルを渡す</param>
        void Remove(const Common::Handle& in_hTask);
        
        /// <summary>
        /// グループ丸ごとタスクを削除
        /// </summary>
        /// <param name="in_groupId"></param>
        void RemoveGroup(const Sint32 in_groupId);

        /// <summary>
        /// すべてのタスクを削除
        /// </summary>
        void RemoveAll();

        /// <summary>
        /// 指定グループの全タスクを指定グループへ移動
        /// </summary>
        /// <param name="in_groupId"></param>
        const Bool MoveGroupAll(const Sint32 in_orgGroupId, const Sint32 in_targetGroupId);
        
        /// <summary>
        /// タスクアドレスをハンドルから取得
        /// </summary>
        /// <param name="hTask"></param>
        /// <returns></returns>
        Task* GetTask(const Common::Handle& in_hTask);
 
        template<class T>
        T* GetTask(const Common::Handle& in_hTask)
        {
            Task* pTask = this->GetTask(in_hTask);
            return reinterpret_cast<T*>(pTask);
        }

        /// <summary>
        /// 論理和を使ったフラグの設定
        /// </summary>
        /// <param name="in_groupId"></param>
        /// <param name="in_flags">論理和としてセットするフラグ</param>
        void EnableFlag(const Sint32 in_groupId, const Uint32 in_flags);
        
        /// <summary>
        /// 論理和を使ったフラグの消去
        /// </summary>
        /// <param name="groupId"></param>
        /// <param name="flags">論理和で消去するフラグ</param>
        void DisableFlag(const Sint32 in_groupId, const Uint32 flags);
        
        const Uint32 Flag(const Sint32 in_groupId) const;

        /// <summary>
        /// グループに設定しているタスクの総数
        /// </summary>
        /// <param name="in_groupId"></param>
        /// <returns></returns>
        const Uint32 CountWithGroup(const Sint32 in_groupId) const;

        const Uint32 GetMaxGroup() const { return this->_groupNum; }

    private:
        // タスクグループ管理
        struct TaskGroup
        {
            // グループの先頭タスクでダミーとなる
            Task* _pRootTask = NULL;
            // グループのタスク終端
            Task* _pTailTask = NULL;
            // グループのフラグ
            Uint32 _flags = 0;

            /// <summary>
            /// 設定しているタスク数
            /// </summary>
            Uint32 _count = 0;
            
            TaskGroup();
            ~TaskGroup();
        };

        /// <summary>
        /// 生成したタスクのセットアップ
        /// </summary>
        /// <param name="out_pData"></param>
        Task* _SetupTask(BasePoolManager::AllocData* out_pData, const Bool in_bAutoDelete);

        /// <summary>
        /// タスク追加する
        /// </summary>
        const Bool _Attach(Task* in_pTask, const Sint32 in_groupId);

        /// <summary>
        /// タスクをグループから外す
        /// </summary>
        const Sint32 _Dettach(Task* in_pTask);

    private:
        TaskGroup *_pTasks = NULL;
        Sint32 _groupNum = 0;
    };
};

