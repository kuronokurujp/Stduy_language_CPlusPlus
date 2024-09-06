#include "Task.h"

#include "TaskManager.h"

static const Core::TaskData NullTaskData;

namespace Core
{
    /// <summary>
    /// タスク管理に登録する
    /// 登録に必要な情報を設定
    /// </summary>
    void Task::VSetup(const Bool in_bReleaseMem)
    {
        this->_Clear();

        // パラメータはタスク管理に登録された時に設定される
        this->_bReleaseMem = in_bReleaseMem;
    }

    void Task::Kill()
    {
        // 死亡フラグを付ける
        _bKill = TRUE;
    }

}  // namespace Core
