#include "View.h"

namespace Render
{
    const Bool View::Release()
    {
        this->ClearCmd();
        return TRUE;
    }

    const Bool View::AddCmd(const Command&& in_rrCmd)
    {
        // コマンドをコピー
        this->_commandBuff.PushBack(in_rrCmd);

        return TRUE;
    }

}  // namespace Render
