#pragma once

#include "MiniEngine.h"
#include "Core/Common/Handle.h"

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
        Uint32 id = 0;
        void* pData = NULL;
    };

    /// <summary>
    /// オブジェクトのタスクを作る時の継承用のクラス
    /// </summary>
    class Task
    {
        // タスクマネージャには非公開メンバを操作させる
        friend class TaskManager;

        E_CLASS_COPY_CONSTRUCT_NG(Task);

    public:
        static const Uint32 NONE_ID = 0;
        static const Sint32 GROUP_NONE_ID = -1;

        // タスクは生成したインスタンスが再利用されるのでコンストラクタで初期化はできない
        Task() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        virtual void Init(const Bool in_bAutoDelete);
        
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
        /// </summary>
        virtual void Update(const Float32 in_dt, const TaskData*) { E_ASSERT(FALSE && "継承クラスは必ず実装"); }

        /// <summary>
        /// タスク破棄
        /// </summary>
        void Kill();
        
        const Common::Handle& GetHandle() const { return _hSelf; }

    private:
        void _Clear()
        {
            this->_bStart = TRUE;
            this->_bKill = FALSE;

            this->_groupId = Task::GROUP_NONE_ID;
            this->_pPrev = NULL;
            this->_pNext = NULL;
            this->_bReleaseMem = FALSE;
        }

    protected:
        TaskManager* _pManager = NULL;

    private:
        Sint32 _groupId = Task::GROUP_NONE_ID;
        Common::Handle _hSelf;
        Bool _bStart = TRUE;
        Bool _bKill = FALSE;
        // タスクを解放した時にメモリからも解放するか
        // 解放しない場合はキャッシュして使いまわすことになる
        Bool _bReleaseMem = FALSE;
        
        Task* _pPrev = NULL;
        Task* _pNext = NULL;
    };

    // タスクデータのデフォルト
    // タスクデータの引数でデフォルト設定する場合に利用
    extern const TaskData DEFAULT_TASK_DATA;
};

