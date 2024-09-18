#pragma once

#include "Engine/Common/CustomList.h"
#include "Engine/Common/Handle.h"
#include "Engine/Core.h"
#include "Engine/Generated.h"

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
        friend class TaskTree;

        HE_CLASS_COPY_NG(Task);
        HE_CLASS_MOVE_NG(Task);
        HE_GENERATED_CLASS_BASE_BODY_HEADER(Task);

    public:
        static const Uint32 uNoneId      = 0;
        static const Sint32 iNoneGroupId = -1;

        // タスクは生成したインスタンスが再利用されるのでコンストラクタで初期化はできない
        Task() { this->_Clear(); }

        virtual ~Task() = default;

        /// <summary>
        /// タスク実行前の初期セットアップ
        /// </summary>
        virtual void VSetup(const Bool in_bAutoDelete);

        /// <summary>
        /// タスク開始
        /// </summary>
        virtual Bool VBegin() { return TRUE; }

        /// <summary>
        /// タスク終了
        /// </summary>
        virtual Bool VEnd() { return TRUE; }

        /// <summary>
        /// 更新
        /// </summary>
        virtual void VUpdate(const Float32) = 0;

        /// <summary>
        /// イベント通知
        /// </summary>
        virtual void VEvent(const TaskData&) {}

        /// <summary>
        /// 破棄指示
        /// 呼び出してもすぐには破棄されない
        /// </summary>
        void Kill();

        /// <summary>
        /// タスクのオブジェクトハンドル
        /// </summary>
        inline const Common::Handle& Handle() const { return this->_selfHandle; }

        /// <summary>
        /// グループID
        /// </summary>
        inline Sint32 GetGropuId() const { return this->_iGroupId; }

    protected:
        /// <summary>
        /// タスク破棄
        /// </summary>
        virtual void _VDestory() {}

    private:
        void _Clear()
        {
            this->_bStart = TRUE;
            this->_bKill  = FALSE;

            this->_iGroupId    = Task::iNoneGroupId;
            this->_pPrev       = NULL;
            this->_pNext       = NULL;
            this->_bReleaseMem = FALSE;
            this->_selfHandle.Clear();
        }

    protected:
        TaskManager* _pTaskManager = NULL;

    private:
        Sint32 _iGroupId = Task::iNoneGroupId;
        Bool _bStart     = TRUE;
        Bool _bKill      = FALSE;
        Common::Handle _selfHandle;

        // タスクを解放した時にメモリからも解放するか
        // 解放しない場合はキャッシュして使いまわすことになる
        Bool _bReleaseMem = FALSE;

        Task* _pPrev = NULL;
        Task* _pNext = NULL;
    };

};  // namespace Core

// タスクデータのデフォルト
// タスクデータの引数でデフォルト設定する場合に利用
extern const Core::TaskData NullTaskData;
