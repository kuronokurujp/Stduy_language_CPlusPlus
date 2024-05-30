#include "Memory.h"

#include "MiniEngine.h"

#ifdef _HOBBY_ENGINE_DEBUG

/// <summary>
/// newのオーバロード
/// アライメントはMINIMUM_ALIGN_SIZEの倍数である必要がある
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル</param>
/// <param name="in_line">呼び出したファイル行</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new(
    size_t in_size, Uint8 in_page, Uint8 in_alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType, const Byte* in_pFile, Uint32 in_line)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 memSize = static_cast<Uint32>(in_size);
    return (Core::Memory::Manager::I().AllocateMemory(memSize, in_page, in_alignSize, in_locateType, in_pFile, in_line));
}

/// <summary>
/// new[]のオーバーロード
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定ｊ</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル名</param>
/// <param name="in_line">呼び出したファイル行数</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* ::operator new[](
    size_t in_size, Uint8 in_page, Uint8 in_alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType, const Byte* in_pFile, Uint32 in_line)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 memSize = static_cast<Uint32>(in_size);
    return (Core::Memory::Manager::I().AllocateMemory(memSize, in_page, in_alignSize, in_locateType, in_pFile, in_line));
}

#else

/// <summary>
/// newのオーバロード
/// アライメントはMINIMUM_ALIGN_SIZEの倍数である必要がある
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル</param>
/// <param name="in_line">呼び出したファイル行</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new(size_t in_size, Uint8 in_page, Uint8 in_alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 memSize = static_cast<Uint32>(in_size);
    return (Core::Memory::Manager::I().AllocateMemory(memSize, in_page, in_alignSize, in_locateType));
}

/// <summary>
/// new[]のオーバーロード
/// </summary>
/// <param name="in_size">確保したいサイズ</param>
/// <param name="in_page">メモリページ指定ｊ</param>
/// <param name="in_alignSize">メモリアライメントのサイズ</param>
/// <param name="in_locateType">確保位置</param>
/// <param name="in_pFile">呼び出したファイル名</param>
/// <param name="in_line">呼び出したファイル行数</param>
/// <returns>NULL 失敗 / 非NULL 確保したメモリのアドレス</returns>
void* operator new[](size_t in_size, Uint8 in_page, Uint8 in_alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TYPE in_locateType)
{
    // VSのx64だとsize_tがunsigned int64になるが, 8byteフルサイズでの確保はないと思うので,
    // Uint32でサイズを調整した
    Uint32 memSize = static_cast<Uint32>(in_size);
    return (Core::Memory::Manager::I().AllocateMemory(memSize, in_page, in_alignSize, in_locateType));
}
#endif

/// <summary>
/// deleteのオーバーロード
/// </summary>
/// <param name="in_pPtr">解放メモリのポインタ</param>
void operator delete(void* in_pPtr)
{
    // 初回の共有ポインターを生成したらここが呼ばれてin_pPtrが0になっている
    // ポインターがないのはおかしいので即終了している
    if (in_pPtr == 0)
        return;

    return (Core::Memory::Manager::I().FreeMemory(in_pPtr));
}

/// <summary>
/// delete配列のオーバーロード
/// </summary>
/// <param name="in_pPtr">解放メモリのポインタ</param>
void operator delete[](void* in_pPtr)
{
    if (in_pPtr == 0)
        return;

    return (Core::Memory::Manager::I().FreeMemory(in_pPtr));
}

#ifdef _MSC_VER

void* operator new(std::size_t in_size)
{
    Uint32 memSize = static_cast<Uint32>(in_size);
    Uint8 page = 0;

#ifdef _HOBBY_ENGINE_DEBUG
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_LAST,
        __FILE__, __LINE__));
#else
    return (HOBBY_ENGINE.GetMemManager().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_TOP);
#endif
}

void* operator new[](std::size_t in_size)
{
    Uint32 memSize = static_cast<Uint32>(in_size);
    Uint8 page = 0;

#ifdef _HOBBY_ENGINE_DEBUG
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_LAST,
        __FILE__, __LINE__));
#else
    return (HOBBY_ENGINE.GetMemManager().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_TOP);
#endif
}

#else

void* operator new(std::size_t in_size) throw(std::bad_alloc)
{
    Uint32 memSize = static_cast<Uint32>(in_size);
    Uint8 page = 0;

#ifdef _HOBBY_ENGINE_DEBUG
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_LAST,
        __FILE__, __LINE__));
#else
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_TOP);
#endif
}

void* operator new[](std::size_t in_size) throw(std::bad_alloc)
{
    Uint32 memSize = static_cast<Uint32>(in_size);
    Uint8 page = 0;

#ifdef _HOBBY_ENGINE_DEBUG
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_LAST,
        __FILE__, __LINE__));
#else
    return (Core::Memory::Manager::I().AllocateMemory(
        memSize,
        0,
        Core::Memory::Manager::MINIMUM_ALIGN_SIZE,
        Core::Memory::Manager::ALLOCATE_LOCATE_TOP);
#endif
}

#endif

