#include "UUID.h"

#include <random>

namespace Core::Common
{
    Uint32 GenerateUUIDBy32Bit()
    {
        // 乱数エンジンの初期化
        // スレッドセーフ対応をしているのでスレッド内でも使え宇
        static thread_local std::mt19937 generator(std::random_device{}());

        // 0からUINT32_MAXまでの値を生成
        std::uniform_int_distribution<Uint32> distribution(0, UINT32_MAX);
        Uint32 uRandomValue = distribution(generator);

        return static_cast<Uint32>(uRandomValue);
    }

    Uint64 GenerateUUIDBy64Bit()
    {
        // 乱数エンジンの初期化
        static thread_local std::mt19937_64 generator(std::random_device{}());

        // 0からUINT64_MAXまでの値を生成
        std::uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
        Uint64 ulRandomValue = distribution(generator);

        // UUIDバージョン4の規格に従ってバージョンとバリアントを設定
        ulRandomValue &= 0xFFFFFFFFFFFF0FFFULL;  // バージョンビットをクリア
        ulRandomValue |= 0x0000000000004000ULL;  // バージョン4を設定

        ulRandomValue &= 0x3FFFFFFFFFFFFFFFULL;  // バリアントビットをクリア
        ulRandomValue |= 0x8000000000000000ULL;  // バリアント1を設定

        return static_cast<Uint64>(ulRandomValue);
    }

}  // namespace Core::Common
