#include "{MODULE_NAME}Module.h"

namespace {MODULE_NAME}
{
    /// <summary>
    /// モジュール初期化
    /// </summary>
    const Bool {MODULE_NAME}Module::_VStart()
    {
        return TRUE;
    }

    /// <summary>
    /// インスタンス破棄時に呼ばれる
    /// </summary>
    const Bool {MODULE_NAME}Module::_VRelease()
    {
        return TRUE;
    }
}  // namespace Actor
