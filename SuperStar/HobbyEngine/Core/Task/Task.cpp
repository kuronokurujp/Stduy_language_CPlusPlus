#include "Task.h"

namespace Core
{
    static const TaskData DEFAULT_TASK_DATA;

    /// <summary>
    /// タスク管理に登録する
    /// 登録に必要な情報を設定
    /// </summary>
    /// <param name="bAutoDelete">TRUEだとタスク破棄と同時にタ
    void Task::Init(const Bool in_bAutoDelete)
    {
        this->_Clear();
        // パラメータはタスク管理に登録された時に設定される
        this->_bAutoDelete = in_bAutoDelete;
    }

    void Task::Kill()
    {
        // 死亡フラグを付ける
        _bKill = TRUE;
    }
}

