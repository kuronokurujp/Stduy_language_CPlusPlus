#pragma once

#include "Core/Common/Handle.h"
#include "MiniEngine.h"

namespace Core
{
    // 前方宣言
    class TaskManager;

    /// <summary>
    /// タスクデータ
    /// Updateメソッドを呼び出した側がデータを渡す場合に使う
    /// </summary>
    struct TaskData
    {
    public:
        Uint32 uId  = 0;
        void* pData = NULL;
    };

    /// <summary>
    /// オブジェクトのタスクを作る時の継承用のクラス
    /// </summary>
    class Task
    {
        // タスクマネージャには非公開メンバを操作させる
        friend class TaskManager;

        HE_CLASS_COPY_CONSTRUCT_NG(Task);

    public:
        static const Uint32 uNoneId      = 0;
        static const Sint32 iNoneGroupId = -1;

        // タスクは生成したインスタンスが再利用されるのでコンストラクタで初期化はできない
        Task() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        virtual void Setup(const Bool in_bAutoDelete);

        /// <summary>
        /// タスク破棄
        /// </summary>
        void Kill();

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual const Bool Begin() { return TRUE; }

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual const Bool End() { return TRUE; }

        /// <summary>
        /// 更新用で継承先が実装しないとだめ
        /// TaskData型は更新に必要なデータなのでこのデータを保存してはいけない
        /// </summary>
        virtual void Update(const Float32 in_fDt, const TaskData&)
        {
            HE_ASSERT(FALSE && "継承クラスは必ず実装");
        }

        inline const Common::Handle& GetHandle() const { return this->_hSelf; }

        /// <summary>
        /// タスクのグループID
        /// </summary>
        /// <returns></returns>
        inline const Sint32 GetGropuId() const { return this->_iGroupId; }

    private:
        void _Clear()
        {
            this->_bStart = TRUE;
            this->_bKill  = FALSE;

            this->_iGroupId    = Task::iNoneGroupId;
            this->_pPrev       = NULL;
            this->_pNext       = NULL;
            this->_bReleaseMem = FALSE;
        }

    protected:
        TaskManager* _pManager = NULL;

    private:
        Sint32 _iGroupId = Task::iNoneGroupId;
        Common::Handle _hSelf;
        Bool _bStart = TRUE;
        Bool _bKill  = FALSE;
        // タスクを解放した時にメモリからも解放するか
        // 解放しない場合はキャッシュして使いまわすことになる
        Bool _bReleaseMem = FALSE;

        Task* _pPrev = NULL;
        Task* _pNext = NULL;
    };
};  // namespace Core

// タスクデータのデフォルト
// タスクデータの引数でデフォルト設定する場合に利用
extern const Core::TaskData DEFAULT_TASK_DATA;
