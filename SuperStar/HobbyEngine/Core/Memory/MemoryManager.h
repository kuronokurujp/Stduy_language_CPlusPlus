#pragma once

#include "Core/Core.h"
#include "Core/Common/Singleton.h"

namespace Core
{
    namespace Memory
    {
        /// <summary>
        /// メモリ管理
        /// アプリで使用するメモリをリークを検知するためにアプリ独自のメモリ管理を使う
        /// メモリ管理のインスタンスは複数作るケースはメモリ管理が分散して複雑になるので絶対にだめ
        /// </summary>
        class Manager : public Common::Singleton<Manager>
        {
            E_CLASS_COPY_CONSTRUCT_NG(Manager);

        public:
            Manager();
            ~Manager();

        public:
            // 最低アラインサイズ（ヘッダもこのアラインである必要あり）
            // X64であれば8byteなのでアライメントは8にする
#ifdef _X64
            static const Uint8	MINIMUM_ALIGN_SIZE = 8;
#else
            static const Uint8	MINIMUM_ALIGN_SIZE = 4;
#endif

            /// <summary>
            /// 確保の位置タイプ
            /// 確保位置を決めれる
            /// </summary>
            enum ALLOCATE_LOCATE_TYPE
            {
                // 前から取る
                ALLOCATE_LOCATE_TOP = 0,
                // 後ろから取る
                ALLOCATE_LOCATE_LAST,
                // 位置タイプの数．
                ALLOCATE_MODE_NUM,
            };

#ifdef _HOBBY_ENGINE_DEBUG
            // メモリ確保
            #define	ALLOCATE_MEMORY(allocateSize, page, alignSize) AllocateMemory(allocateSize, page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TOP, __FILE__, __LINE__)
#else
            // メモリ確保
            #define	ALLOCATE_MEMORY(allocateSize, page, alignSize) AllocateMemory( allocateSize, page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_TOP )
#endif

#ifdef _HOBBY_ENGINE_DEBUG
            // メモリ確保
            #define	ALLOCATE_MEMORY_LAST(allocateSize, page, alignSize) AllocateMemory(allocateSize, page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_LAST, __FILE__, __LINE__ )
#else
            // メモリ確保
            #define	ALLOCATE_MEMORY_LAST(allocateSize, page, alignSize) AllocateMemory(allocateSize, page, alignSize, Core::Memory::Manager::ALLOCATE_LOCATE_LAST)
#endif

            // メモリ解放
            #define	FREE_MEMORY( pAllocateMemory ) FreeMemory( pAllocateMemory )

        private:
            // メモリページの最大数。アラインはこのサイズの公倍数である必要あり
            static const Uint8	MEMORY_PAGE_MAX = 64;

            // 念のためアラインを１に
            // アラインが4とかになっていた場合
            // 1バイトとかのメンバの後ろに勝手にパディングいれられてしまうので
#pragma pack(push, 1)
            /// <summary>
            /// メモリブロックの先頭に書き込まれる
            /// メモリブロックを管理データ
            /// </summary>
            struct BlockHeader
            {
                // メモリブロックのサイズ（ヘッダやパディングも含む）
                Uint32 _size = 0;
                // メモリブロックでアロケーションされるサイズ（実際に使用できるサイズ）
                Uint32 _allocateSize = 0;
                // 前のメモリブロックアドレス
                BlockHeader* _pPrev = NULL;
                // 後ろのメモリブロックアドレス．
                BlockHeader* _pNext = NULL;
                // 使用状況
                // もしかしたら通常の場合と配列確保の場合で分けるかも
                Uint8 _useFlag = 0;
                // アラインサイズ
                Uint8 _alignSize = MINIMUM_ALIGN_SIZE;
                // 使用ページ番号
                Uint8 _page = 0;
                // 後ろのパディングサイズ
                Uint8 _paddingSize = 0;

#ifdef _HOBBY_ENGINE_DEBUG
                // ファイル名（デバッグ用）
                Byte _fileName[256] = {};
                // 行番号（デバッグ用）
                Uint32 _line = 0;
                // 門番（デバッグ用）
                Uint32 _magicNumber = 0;
#endif
            };

#ifdef _HOBBY_ENGINE_DEBUG
            /// <summary>
            /// メモリブロックのフッターデータ
            /// メモリブロックの最後に書き込まれる
            /// メモリブロックの管理データ
            /// 現在はデバッグ用のデータしかないのでリリースには含めない
            /// </summary>
            struct BlockFooter
            {
                // 門番（デバッグ用）
                Uint32 _magicNumber = 0;
            };
#endif

            /// <summary>
            /// メモリページデータ
            /// </summary>
            struct PageInfo
            {
                // ページのトップアドレス．
                void* _pTopAddr = NULL;
                // ページに割り当てられたサイズ．
                Uint32 _size = 0;
                // メモリブロック先頭．
                BlockHeader* _pMemoryBlockTop = NULL;
            };
#pragma pack(pop)

        public:
            // 無効なメモリページ（主に終端子用）
            static const Uint8 INVALID_MEMORY_PAGE = 0xff;

#pragma pack(push, 1)
            /// <summary>
            /// メモリページ設定データ
            /// </summary>
            struct PageSetupInfo
            {
                // ページ番号．
                Uint8 _page = INVALID_MEMORY_PAGE;
                // ページに割り当てるサイズ（MINIMUM_ALIGN_SIZEの倍数であること）
                Uint32 _size = 0xffffffff;
            };
#pragma pack(pop)

        public:
            //	初期化
            const Bool Init(const Uint32 in_manageSize);
            //	終了
            const Bool End();

#ifdef _HOBBY_ENGINE_DEBUG
            // メモリ確保
            void* AllocateMemory(const Uint32 in_allocateSize, const Uint8 in_page, const Uint8 in_alignSize, const ALLOCATE_LOCATE_TYPE in_locateType, const Byte* in_pFile, Uint32 in_line);
#else
            // メモリ確保
            void* AllocateMemory(const Uint32 in_allocateSize, const Uint8 in_page, const Uint8 in_alignSize, const ALLOCATE_LOCATE_TYPE in_locateType);
#endif

            // メモリ開放
            void FreeMemory(void* in_pAllocatedMemory);

            // 確保したメモリのサイズを取得
            const Uint32 GetAllocatedMemorySize(void* in_pAllocatedMemory);

            // メモリページのセットアップ
            const Bool SetupMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_num);
            // メモリページのリマップ
            const Bool RemapMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_num);

        private:
            /// <summary>
            /// 利用準備ができているか
            /// </summary>
            /// <returns></returns>
            inline const Bool _IsReady() const
            {
                return (this->_pHeapTop != NULL);
            }

            /// <summary>
            /// 指定ページが初期化しているか
            /// </summary>
            /// <param name="in_page">ページ</param>
            /// <returns>TRUE 初期化 / FALSE 初期化していない</returns>
            inline const Bool _IsInitMemoryPage(const Uint8 in_page) const
            {
                return (this->_memoryPageInfoArray[in_page]._pTopAddr != NULL);
            }

            /// <summary>
            /// 指定ページに利用中のメモリブロックが存在するか
            /// </summary>
            /// <param name="in_page">ページ</param>
            /// <returns>TRUE 存在 / FALSE 存在しない</returns>
            const Bool _IsExistUsedMemoryBlock(const Uint8 in_page) const
            {
                // ページのメモリブロック先頭にデータがなければ存在しない
                if (this->_memoryPageInfoArray[in_page]._pMemoryBlockTop == NULL)
                    return FALSE;

                // 最初のメモリブロックが使用中
                if (this->_memoryPageInfoArray[in_page]._pMemoryBlockTop->_useFlag == 1)
                    return TRUE;

                // メモリブロックが2つ以上あるならなにかしら使っている
                if (this->_memoryPageInfoArray[in_page]._pMemoryBlockTop->_pNext != NULL)
                    return TRUE;

                return FALSE;
            }

            /// <summary>
            /// メモリブロックのヘッダーサイズを取得
            /// </summary>
            /// <returns></returns>
            inline const Uint32 _GetMemoryBlockHeaderSize() const
            {
                return (sizeof(BlockHeader));
            }

            /// <summary>
            /// メモリブロックのフッターサイズを取得
            /// </summary>
            /// <returns></returns>
            inline const Uint32 _GetMemoryBlockFooterSize() const
            {
                // フッターはデバッグ用のしかないから
#ifdef _HOBBY_ENGINE_DEBUG
                return (sizeof(BlockFooter));
#else
                return 0;
#endif
            }

            /// <summary>
            /// メモリブロックのシステム利用サイズを取得
            /// </summary>
            /// <returns></returns>
            inline const Uint32 _GetMemoryBlockSystemDataSize() const
            {
                return (this->_GetMemoryBlockHeaderSize() + this->_GetMemoryBlockFooterSize());
            }

            // メモリページの初期化
            const Bool _InitMemoryPage(const Uint8 in_page, const Uint32 in_offset, const Uint32 in_size);
            // 指定のメモリブロックの前にメモリブロックを連結する
            void _AddListMemoryBlockPrev(BlockHeader* in_pMemoryBlock, BlockHeader* in_pTargetMemoryBlock);
            // 指定のメモリブロックの後ろに、メモリブロックを連結する
            void _AddListMemoryBlockNext(BlockHeader* in_pMemoryBlock, BlockHeader* in_pTargetMemoryBlock);
            // メモリブロックをリストからはずす
            void _DelListMemoryBlock(BlockHeader* in_pMemoryBlock);
            // 指定のメモリブロックを指定サイズで二つに分ける
            BlockHeader* _SplitMemoryBlock(BlockHeader* in_pMemoryBlock, Uint32 in_splitSize);
            // 指定されたメモリブロックをマージする
            BlockHeader* _MergeMemoryBlock(BlockHeader* in_pMemoryBlock1, BlockHeader* in_pMemoryBlock2);

#ifdef _HOBBY_ENGINE_DEBUG
        public:
            // 指定されたメモリページの情報を表示する
            void PrintMemoryPageInfo(const Uint8 in_page);
            // すべてのメモリ情報を表示する
            void PrintAllMemoryInfo();

            // 指定されたメモリページのメモリブロックが正常かチェックする
            const Bool CheckMemoryBlockByPage(const Uint8 in_page);
            // すべてのメモリブロックが正常かチェックする
            const Bool CheckAllMemoryBlock();

        private:
            // TODO: ランタイムでエンディアンを判定する方法あるか?
            // 門番（ビッグエンディアン）
            // static const Uint32 MAGIC_NUMBER = 0xDEADDEAD;
            // 門番(リトルエンディアン)
            static const Uint32	MAGIC_NUMBER = 0xADEDADDE;
            // 空きメモリ領域に埋めるデータ
            static const Uint8	FREEMEMORY_FILL_DATA = 0xDD;
            // 取得直後のメモリに埋めるデータ
            static const Uint8	NEWMEMORY_FILL_DATA = 0xCD;

            /// <summary>
            /// 指定メモリブロックが有効かチェック
            /// </summary>
            /// <param name="in_pMemoryBlock">メモリブロック</param>
            /// <returns>TRUE 有効 / FALSE 無効</returns>
            const Bool _IsValidMemoryBlock(BlockHeader* in_pMemoryBlock)
            {
                // ヘッダーの管理データをチェック
                if (in_pMemoryBlock->_magicNumber != MAGIC_NUMBER)
                    return FALSE;

                //  フッターの管理データをチェック
                Byte* addr = (reinterpret_cast<Byte*>(in_pMemoryBlock)) + (in_pMemoryBlock->_size - this->_GetMemoryBlockFooterSize());
                if (reinterpret_cast<BlockFooter*>(addr)->_magicNumber != MAGIC_NUMBER)
                    return FALSE;

                return TRUE;
            }

            /// <summary>
            /// デバッグ用
            /// メモリブロックに門番を書き込む
            /// </summary>
            /// <param name="in_pMemoryBlock">メモリブロックのポインタ</param>
            inline void	_SetMemoryBlockMagicNo(BlockHeader* in_pMemoryBlock)
            {
                // ヘッダーに書き込む
                in_pMemoryBlock->_magicNumber = MAGIC_NUMBER;

                BlockFooter* pBlockFooter = reinterpret_cast<BlockFooter*>(reinterpret_cast<Byte*>(in_pMemoryBlock) + (in_pMemoryBlock->_size - _GetMemoryBlockFooterSize()));
                pBlockFooter->_magicNumber = MAGIC_NUMBER;
            }

            // 指定されたメモリブロックを空きメモリデータで塗りつぶす
            void _FillFreeMemoryBlock(BlockHeader* in_pMemoryBlock);
            // 指定されたメモリブロックを新規メモリデータで塗りつぶす
            void _FillNewMemoryBlock(BlockHeader* in_pMemoryBlock);
#endif

        private:
            // プラットフォームから取得した際のヒープの先頭アドレス
            void* _pHeapTop = NULL;
            // ヒープサイズ
            Uint32 _heapSize = 0;
            // メモリページ情報
            // あまり柔軟性よくてもしょうがない(速度重視の配列)
            PageInfo _memoryPageInfoArray[MEMORY_PAGE_MAX];

            Bool _bEnable = TRUE;
        };
    }
}

// メモリ管理のアクセスマクロ
#define HOBBY_MEM_MANGER Core::Memory::Manager::I()
