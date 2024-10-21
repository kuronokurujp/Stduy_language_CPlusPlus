#pragma

#include "Engine/Type.h"

// UUIDを生成

namespace Core::Common
{
    /// <summary>
    /// UUID生成したのを32Bitに省略して返す
    /// UUIDは本来128Bitなので32Bitに省略した事で値の衝突率が高くなるので扱いには注意が必要
    /// </summary>
    extern Uint32 GenerateUUIDBy32Bit();

    /// <summary>
    /// UUID生成したのを128Bitに省略して返す
    /// UUIDは本来128Bitなので32Bitに省略した事で値の衝突率が高くなるので扱いには注意が必要
    /// </summary>
    extern Uint64 GenerateUUIDBy64Bit();

}  // namespace Core::Common
