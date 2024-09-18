#include "Task.h"

const Core::TaskData NullTaskData;

namespace Core
{
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
