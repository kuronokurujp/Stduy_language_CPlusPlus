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

        // マジックナンバーは32bitのUUIDを使用
        // 32bitなので衝突確率は1/100000で発生する
        // しかしインデックスと組み合わせているので衝突確率はほぼないと思っている
        Uint32 uMagicNumber = GenerateUUIDBy32Bit();
        // インデックスとマジックナンバーを割り当て
        this->_handleField._index = in_uIndex;
        this->_handleField._magic = uMagicNumber;
    }
}  // namespace Core::Common

const Core::Common::Handle NullHandle;
