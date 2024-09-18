#include "View.h"

namespace Render
{
    Bool View::Release()
    {
        this->ClearCmd();
        return TRUE;
    }

    Bool View::AddCmd(const Command&& in_rrCmd)
    {
        // コマンドをコピー
        this->_commandBuff.PushBack(in_rrCmd);

        return TRUE;
    }

}  // namespace Render
