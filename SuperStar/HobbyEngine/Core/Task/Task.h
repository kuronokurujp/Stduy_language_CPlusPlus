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

        E_CLASS_COPY_CONSTRUCT_NG(Task)

    public:
        static const Uint32 s_idNone = 0;

        // タスクは生成したインスタンスが再利用されるのでコンストラクタで初期化はできない
        Task() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
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
        virtual void Update(const Float32 in_dt, const TaskData*) {}

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

            this->_groupId = -1;
            this->_pPrev = NULL;
            this->_pNext = NULL;
            this->_bAutoDelete = FALSE;
        }
    protected:
        TaskManager* _pManager;

    private:
        Sint32 _groupId = -1;
        Common::Handle _hSelf;
        Bool _bStart = TRUE;
        Bool _bKill = FALSE;
        Bool _bAutoDelete = FALSE;
        
        Task* _pPrev = NULL;
        Task* _pNext = NULL;
    };

    // タスクデータのデフォルト
    // タスクデータの引数でデフォルト設定する場合に利用
    extern const TaskData DEFAULT_TASK_DATA;
};

