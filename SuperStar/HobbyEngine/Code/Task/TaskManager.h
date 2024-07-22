#pragma once

#include "Common/CustomArray.h"
#include "Common/PoolManager.h"
#include "Core.h"

namespace Core
{
    // 前方宣言
    class Task;
    struct TaskData;

    /// <summary>
    /// タスク管理
    /// プールデータクラスを継承してタスク個数を固定にしている
    /// </summary>
    class TaskManager final : public Common::BasePoolManager<Task>
    {
        HE_CLASS_COPY_CONSTRUCT_NG(TaskManager);

    public:
        // タスクマネージャが使用するフラグ
        // 処理を停止するフラグ
        static const Uint32 FLAG_PAUSE = 0x00000001;

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
        void UpdateAll(const Float32 in_dt, const TaskData&);

        /// <summary>
        /// 指定グループを更新
        /// </summary>
        void UpdateGroup(const Sint32 in_groupId, const Float32 in_dt, const TaskData&);

        /// <summary>
        /// タスク作成して追加する
        /// 結果はハンドルで返す
        /// </summary>
        /// <param name="in_groupId">追加するグループID</param>
        /// <returns></returns>
        template <class T>
        Common::Handle CreateAndAdd(const Sint32 in_groupId, const Bool in_bReleaseMem)
        {
            static_assert(std::is_base_of<Task, T>::value, "TクラスはTaskクラスを継承していない");

            HE_ASSERT(in_groupId < this->_iGroupNum);

            // 利用するタスクを割り当て
            BasePoolManager::AllocData resAlloc = this->_Alloc<T>();
            Task* pTask                         = resAlloc._tpItem;

            pTask->Setup(in_bReleaseMem);
            pTask->_hSelf = resAlloc._handle;

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
        /// TODO: タスクのグループ移動
        /// </summary>
        /// <param name="in_hTask"></param>
        /// <param name="in_gropuId"></param>
        /// <returns></returns>
        const Bool MoveGropuTask(const Common::Handle& in_hTask, const Sint32 in_groupId);

        /// <summary>
        /// タスクアドレスをハンドルから取得
        /// </summary>
        /// <param name="hTask"></param>
        /// <returns></returns>
        Task* GetTask(const Common::Handle& in_hTask);

        template <class T>
        T* GetTask(const Common::Handle& in_hTask)
        {
            static_assert(std::is_base_of<Task, T>::value, "TクラスはTaskクラスを継承していない");

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

        const Uint32 GetMaxGroup() const { return this->_iGroupNum; }

    private:
        // タスクグループ管理
        struct TaskGroup
        {
            // グループの先頭タスクでダミーとなる
            Task* _pRootTask = NULL;
            // グループのタスク終端
            Task* _pTailTask = NULL;
            // グループのフラグ
            Uint32 _uFlags = 0;

            /// <summary>
            /// 設定しているタスク数
            /// </summary>
            Uint32 _uCount = 0;

            TaskGroup();
            ~TaskGroup();
        };

        /// <summary>
        /// タスク追加する
        /// </summary>
        const Bool _Attach(Task* in_pTask, const Sint32 in_groupId);

        /// <summary>
        /// タスクをグループから外す
        /// </summary>
        const Sint32 _Dettach(Task* in_pTask);

    private:
        TaskGroup* _pTasks = NULL;
        Sint32 _iGroupNum  = 0;
    };
};  // namespace Core
