#pragma once

#include "Engine/Common/CustomArray.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Core.h"

namespace Core
{
    // 前方宣言
    class Task;
    struct TaskData;

    /// <summary>
    /// タスク管理
    /// プールデータクラスを継承してタスク個数を固定にしている
    /// </summary>
    class TaskManager final : public Common::RuntimePoolManager<Task>
    {
        HE_CLASS_COPY_NG(TaskManager);
        HE_CLASS_MOVE_NG(TaskManager);

        friend class Task;

    public:
        // タスクマネージャが使用するフラグ
        // 処理を停止するフラグ
        static const Uint32 FLAG_PAUSE = 0x00000001;

    public:
        TaskManager() : RuntimePoolManager() {}
        ~TaskManager();

        /// <summary>
        /// 初期化
        /// 利用前に必ず呼び出す
        /// </summary>
        const Bool Init(const Uint32 in_uTaskMax, const Sint32 in_sGroupNum);

        void End();

        /// <summary>
        /// 全タスク更新
        /// </summary>
        void UpdateAll(const Float32 in_fDt, const TaskData&);

        /// <summary>
        /// 指定グループを更新
        /// </summary>
        void UpdateGroup(const Sint32 in_sGroupId, const Float32 in_fDt, const TaskData&);

        /// <summary>
        /// タスク作成して追加する
        /// 結果はハンドルで返す
        /// </summary>
        template <class T>
        Common::Handle CreateAndAdd(const Sint32 in_sGroupId, const Bool in_bReleaseMem)
        {
            static_assert(std::is_base_of<Task, T>::value, "TクラスはTaskクラスを継承していない");

            HE_ASSERT(in_sGroupId < this->_iGroupNum);

            // 利用するタスクを割り当て
            RuntimePoolManager::AllocData resAlloc = this->_Alloc<T>();
            Task* pTask                            = resAlloc._tpItem;

            pTask->Setup(in_bReleaseMem);
            pTask->_hSelf = resAlloc._handle;

            this->_Attach(pTask, in_sGroupId);

            return resAlloc._handle;
        }

        /// <summary>
        /// タスクの削除
        /// </summary>
        void RemoveTask(Common::Handle*);

        /// <summary>
        /// グループ丸ごとタスクを削除
        /// </summary>
        void RemoveGroup(const Sint32 in_sGroupId);

        /// <summary>
        /// すべてのタスクを削除
        /// </summary>
        void RemoveAll();

        /// <summary>
        /// 指定グループの全タスクを指定グループへ移動
        /// </summary>
        const Bool MoveGroupAll(const Sint32 in_sOrgGroupId, const Sint32 in_sTargetGroupId);

        /// <summary>
        /// TODO: タスクのグループ移動
        /// </summary>
        const Bool MoveGropuTask(const Common::Handle& in_rTask, const Sint32 in_sGroupId);

        /// <summary>
        /// タスクアドレスをハンドルから取得
        /// </summary>
        Task* GetTask(const Common::Handle& in_rTask);

        template <class T>
        T* GetTask(const Common::Handle& in_rTask)
        {
            static_assert(std::is_base_of<Task, T>::value, "TクラスはTaskクラスを継承していない");

            Task* pTask = this->GetTask(in_rTask);
            return reinterpret_cast<T*>(pTask);
        }

        /// <summary>
        /// 論理和を使ったフラグの設定
        /// </summary>
        void EnableFlagWithGroup(const Sint32 in_sGroupId, const Uint32 in_uFlags);

        /// <summary>
        /// 論理和を使ったフラグの消去
        /// </summary>
        void DisableFlagWithGroup(const Sint32 in_sGroupId, const Uint32 in_uFlags);

        const Uint32 FlagWithGroup(const Sint32 in_sGroupId) const;

        /// <summary>
        /// グループに設定しているタスクの総数
        /// </summary>
        /// <param name="in_groupId"></param>
        /// <returns></returns>
        const Uint32 CountWithGroup(const Sint32 in_sGroupId) const;

        /// <summary>
        /// タスクグループの最大数
        /// </summary>
        /// <returns></returns>
        inline const Uint32 GetMaxGroup() const { return this->_iGroupNum; }

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
        };

        /// <summary>
        /// タスク追加する
        /// </summary>
        const Bool _Attach(Task* in_pTask, const Sint32 in_sGroupId);

        /// <summary>
        /// タスクをグループから外す
        /// </summary>
        const Sint32 _Dettach(Task* in_pTask);

    private:
        TaskGroup* _pTasks = NULL;
        Sint32 _iGroupNum  = 0;
    };
};  // namespace Core
