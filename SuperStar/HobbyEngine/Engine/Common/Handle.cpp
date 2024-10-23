#include "Handle.h"

namespace Core::Common
{
    /// <summary>
    /// 初期化
    /// 管理するindexを設定
    /// </summary>
    void Handle::Init(const Uint32 in_uIndex)
    {
        HE_ASSERT(this->Null());
        HE_ASSERT(in_uIndex <= E_MAX_INDEX);

        // Uint32の最大数は42億になるので,
        // インデックスとマジックナンバーの組み合わせでかぶる事はないと思う
        static Uint32 uMagicNumber = 0;
        ++uMagicNumber;

        // インデックスとマジックナンバーを割り当て
        this->_handleField._index = in_uIndex;
        this->_handleField._magic = uMagicNumber;
    }
}  // namespace Core::Common

const Core::Common::Handle NullHandle;
