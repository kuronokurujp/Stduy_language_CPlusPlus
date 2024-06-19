#include "MemoryManager.h"
#include <memory>

namespace Core
{
    namespace Memory
    {
        Manager::Manager()
        {
        }

        Manager::~Manager()
        {
            this->End();
        }

        /// <summary>
        /// 初期化
        /// 利用するヒープサイズ確保
        /// </summary>
        /// <param name="in_useHeepSize">管理側で利用するヒープサイズ</param>
        /// <returns></returns>
        const Bool Manager::Init(const Uint32 in_useHeapSize)
        {
            //	既に初期化されているか、チェックする．
            if (this->_IsReady())
            {
                E_ASSERT(0 && "すでに初期化されている．");
                return FALSE;
            }

            // ヒープからごっそりとる
            // とり方は各プラットフォームによって変えること
            // とりあえず4バイトアラインにしたい．
            // TODO: プラットフォーム用のヒープ取得に切り替え
            this->_pHeapTop = ::_aligned_malloc(in_useHeapSize, MINIMUM_ALIGN_SIZE);
            if (this->_pHeapTop == NULL)
            {
                //	取れなかった．
                E_ASSERT(0 && "取得に失敗．");
                return FALSE;
            }

            this->_heapSize = in_useHeapSize;

            //	ページ情報初期化．
            ::memset(this->_memoryPageInfoArray, 0, sizeof(this->_memoryPageInfoArray));

            return TRUE;
        }

        /// <summary>
        /// 終了
        /// </summary>
        /// <returns></returns>
        const Bool Manager::End()
        {
            if (this->_IsReady() == FALSE)
                return TRUE;

            // TODO: メモリが残っているかチェック

            // 解放する
            // TODO: プラットフォームのヒープ解放に切り替える
            ::_aligned_free(this->_pHeapTop);

            // 痕跡を消す
            this->_pHeapTop = NULL;
            this->_heapSize = 0;

            ::memset(this->_memoryPageInfoArray, 0, sizeof(this->_memoryPageInfoArray));

            return TRUE;
        }

        /// <summary>
        /// メモリページを構築する
        /// 一度構築したら変える事はできない
        /// </summary>
        /// <param name="in_pSetupInfoArray"></param>
        /// <param name="in_num">配列の個数</param>
        /// <returns>TRUE 成功 / FALSE 失敗</returns>
        const Bool Manager::SetupMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_num)
        {
            E_ASSERT(in_pSetupInfoArray && "ページ設定情報がない");

            if (this->_IsReady() == FALSE)
            {
                E_ASSERT(0 && "Managertが初期化されていない．");
                return FALSE;
            }

            // メモリページが一つでも初期化状態であればエラーにする
            for (Uint8 i = 0; i < MEMORY_PAGE_MAX; i++)
            {
                if (this->_IsInitMemoryPage(i))
                {
                    E_ASSERT(0 && "すでに初期化されています．");
                    return FALSE;
                }
            }

            // PageSetupInfo* tempSetupInfo = in_pSetupInfoArray;
            Uint32 heapOffset = 0;

            // 終端子まで繰り返してページを作成する
            // while (tempSetupInfo->_page != INVALID_MEMORY_PAGE)
            for (Uint32 i = 0; i < in_num; ++i)
            {
                const PageSetupInfo* pTempSetupInfo = &in_pSetupInfoArray[i];

                // サイズが大きすぎる場合
                if (heapOffset + pTempSetupInfo->_size > _heapSize)
                {
                    E_ASSERT(0 && "サイズが大きすぎる．");
                    return FALSE;
                }

                if (this->_InitMemoryPage(pTempSetupInfo->_page, heapOffset, pTempSetupInfo->_size) == FALSE)
                {
                    E_ASSERT(0 && "メモリページの初期化に失敗．");
                    return FALSE;
                }

                // 作成ページのサイズ分はヒープを作ったので空いているヒープアドレスにする
                heapOffset += pTempSetupInfo->_size;
                //tempSetupInfo++;
            }

            return TRUE;
        }

        /// <summary>
        /// メモリページ全体のリマップ
        /// </summary>
        /// <param name="in_pSetupInfoArray">メモリページ設定情報の配列</param>
        /// <param name="in_num">配列個数</param>
        /// <returns>TRUE 成功 / FALSE 失敗</returns>
        const Bool Manager::RemapMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_num)
        {
            // 初期化されていない
            if (this->_IsReady() == FALSE)
            {
                E_ASSERT(0 && "Managertが初期化されていない．");
                return FALSE;
            }

#ifdef _HOBBY_ENGINE_DEBUG
            // メモリブロックが壊れていないかチェック
            this->CheckAllMemoryBlock();
#endif

            Uint32 offset = 0;

            Uint32 offsetArray[MEMORY_PAGE_MAX];
            Uint32 sizeArray[MEMORY_PAGE_MAX];
            Bool bNeedInitFlagArray[MEMORY_PAGE_MAX];

            //	サイズをクリアしておく(サイズが0ならいらないページ)
            memset(sizeArray, 0, E_ARRAY_SIZE(sizeArray));
            memset(bNeedInitFlagArray, 0, E_ARRAY_SIZE(bNeedInitFlagArray));

            // 最初にチェックとオフセット等の計算を別枠でする
            // そうしないとDEBUG時で一個前のサイズが大きくなったときメモリブロックのところがデバッグ情報で埋められてしまう
            for (Uint32 i = 0; i < in_num; ++i)
            {
                PageSetupInfo* tempSetupInfo = &in_pSetupInfoArray[i];

                // サイズ0なら消すだけなので特にチェックいらず
                if ((sizeArray[tempSetupInfo->_page] = tempSetupInfo->_size) != 0)
                {
                    // サイズが確保したヒープサイズを超えているのでエラー
                    if (offset + tempSetupInfo->_size > this->_heapSize)
                    {
                        E_ASSERT(0 && "サイズが大きすぎる．");
                        return FALSE;
                    }

                    // 全ページの形にしておく
                    offsetArray[tempSetupInfo->_page] = offset;
                    bNeedInitFlagArray[tempSetupInfo->_page] = TRUE;

                    // すでに初期化されている場合チェック必要
                    if (this->_IsInitMemoryPage(tempSetupInfo->_page))
                    {
                        // オフセットが違うか
                        // サイズが違う場合は初期化しなおし
                        if (
                            reinterpret_cast<Byte*>(this->_memoryPageInfoArray[tempSetupInfo->_page]._pTopAddr) != reinterpret_cast<Byte*>(this->_pHeapTop) + offset
                            || this->_memoryPageInfoArray[tempSetupInfo->_page]._size != tempSetupInfo->_size
                            )
                        {
                            // 使用中のメモリブロックがあるならだめ
                            // サイズが違う場合は大きくなる分には問題ないし
                            // 小さくなる場合も使用していない部分なら問題ないけど
                            // さらにリマップして問題が発生した際に原因が見つけにくくなりそうなのでとりあえずこうしておく
                            if (this->_IsExistUsedMemoryBlock(tempSetupInfo->_page))
                            {
                                E_ASSERT(0 && "オフセットもしくはサイズの変更が指定されているが、使用中のメモリブロックが存在している．");
                                return FALSE;
                            }
                        }
                        else
                        {
                            // オフセットも変わらず,
                            // サイズも同じなのでそのままページが使えるので初期化は不要
                            bNeedInitFlagArray[tempSetupInfo->_page] = FALSE;
                        }
                    }
                }

                offset += tempSetupInfo->_size;
            }

            // 指定されていないページは消す
            // これも先にやっておかないと
            // DEBUG時で一個前のサイズが大きくなったときメモリブロックのところがデバッグ情報で埋められてしまう．
            for (Uint8 i = 0; i < MEMORY_PAGE_MAX; ++i)
            {
                // サイズが0になっているページは指定していないページなので消す
                if (sizeArray[i] == 0)
                {
                    // 消すのにごみが残っていてはだめ
                    if (this->_IsExistUsedMemoryBlock(i))
                    {
                        E_ASSERT(0 && "ページを消去するのに、使用中のメモリブロックが存在している．");
                        return FALSE;
                    }

                    this->_memoryPageInfoArray[i]._pTopAddr = 0;
                    this->_memoryPageInfoArray[i]._size = 0;
                    this->_memoryPageInfoArray[i]._pMemoryBlockTop = NULL;
                }
            }

            // 最後に初期化が必要な場所を初期化
            for (Uint8 i = 0; i < MEMORY_PAGE_MAX; ++i)
            {
                //	初期化が必要ならしろ．
                if (bNeedInitFlagArray[i])
                {
                    if (this->_InitMemoryPage(i, offsetArray[i], sizeArray[i]) == FALSE)
                    {
                        E_ASSERT(0 && "メモリページの初期化に失敗．");
                        return FALSE;
                    }
                }
            }

            return TRUE;
        }

        /// <summary>
        /// メモリページの初期化
        /// </summary>
        /// <param name="in_page">メモリページ指定</param>
        /// <param name="in_heepOffset">メモリページをどこから取るか(メモリマネージャー管理領域先頭からのオフセット。MINIMUM_ALIGN_SIZEの倍数であること)</param>
        /// <param name="in_useHeepSize">メモリページのサイズ（MINIMUM_ALIGN_SIZEの倍数であること）</param>
        /// <returns>TRUE 成功 / FALSE 失敗</returns>
        const Bool Manager::_InitMemoryPage(Uint8 in_page, Uint32 in_heepOffset, Uint32 in_useHeepSize)
        {
            E_ASSERT((in_page < E_ARRAY_NUM(this->_memoryPageInfoArray)) && "指定ページが存在しない");
            E_ASSERT((this->_GetMemoryBlockSystemDataSize() <= in_useHeepSize) && "ページで確保するサイズがブロックサイズより小さい");

            PageInfo* pPageInfo = &this->_memoryPageInfoArray[in_page];

            // ページ情報の初期化
            // 確保したヒープからページで利用するヒープ分を割り当てる
            pPageInfo->_pTopAddr = reinterpret_cast<void*>(reinterpret_cast<Byte*>(this->_pHeapTop) + in_heepOffset);
            pPageInfo->_size = in_useHeepSize;

            // 最初のメモリブロックを設定する
            pPageInfo->_pMemoryBlockTop = reinterpret_cast<BlockHeader*>(pPageInfo->_pTopAddr);

            // 頭にヘッダ情報を書き込む
            pPageInfo->_pMemoryBlockTop->_size = in_useHeepSize;
            pPageInfo->_pMemoryBlockTop->_allocateSize = in_useHeepSize - this->_GetMemoryBlockSystemDataSize();
            pPageInfo->_pMemoryBlockTop->_pPrev = NULL;
            pPageInfo->_pMemoryBlockTop->_pNext = NULL;
            pPageInfo->_pMemoryBlockTop->_useFlag = 0;
            pPageInfo->_pMemoryBlockTop->_alignSize = MINIMUM_ALIGN_SIZE;
            pPageInfo->_pMemoryBlockTop->_page = in_page;
            pPageInfo->_pMemoryBlockTop->_paddingSize = 0;

#ifdef _HOBBY_ENGINE_DEBUG
            memset(pPageInfo->_pMemoryBlockTop->_fileName, 0, E_ARRAY_SIZE(pPageInfo->_pMemoryBlockTop->_fileName));
            pPageInfo->_pMemoryBlockTop->_line = 0;

            // マジックNoセット
            this->_SetMemoryBlockMagicNo(pPageInfo->_pMemoryBlockTop);

            // 空きメモリな感じ
            this->_FillFreeMemoryBlock(pPageInfo->_pMemoryBlockTop);
#endif
            return TRUE;
        }

#ifdef _HOBBY_ENGINE_DEBUG
        /// <summary>
        /// メモリ確保
        /// アライメントサイズはMINIMUM_ALIGN_SIZEの倍数である必要がある
        /// </summary>
        /// <param name="in_allocateSize">確保したいサイズ</param>
        /// <param name="in_page">メモリページ</param>
        /// <param name="in_alignSize">メモリのアライメントサイズ</param>
        /// <param name="in_locateType">確保位置(前からなのか後ろからなのか)</param>
        /// <param name="in_pFile">呼び出したファイル名(デバッグ限定)</param>
        /// <param name="in_line">呼び出したファイルの行数(デバッグ限定)</param>
        /// <returns>NULL 確保失敗 / 非NULL 確保したメモリアドレス</returns>
        void* Manager::AllocateMemory(
            Uint32 in_allocateSize, Uint8 in_page, Uint8 in_alignSize, ALLOCATE_LOCATE_TYPE in_locateType, const Byte* in_pFile, Uint32 in_line)
#else
        /// <summary>
        /// メモリ確保
        /// アライメントサイズはMINIMUM_ALIGN_SIZEの倍数である必要がある
        /// </summary>
        /// <param name="in_allocateSize">確保したいサイズ</param>
        /// <param name="in_page">メモリページ</param>
        /// <param name="in_alignSize">メモリのアライメントサイズ</param>
        /// <param name="in_locateType">確保位置(前からなのか後ろからなのか)</param>
        /// <returns>NULL 確保失敗 / 非NULL 確保したメモリアドレス</returns>
        void* Manager::AllocateMemory(Uint32 in_allocateSize, Uint8 in_page, Uint8 in_alignSize, ALLOCATE_LOCATE_TYPE in_locateType)
#endif
        {
            // 確保サイズが0以下では確保できない
            E_ASSERT((0 < in_allocateSize) && "サイズが0．");
            // 指定したページが存在するか
            E_ASSERT((in_page < E_ARRAY_NUM(this->_memoryPageInfoArray)) && "指定したページが存在しない");

            // 初期化されていないとだめ
            if (this->_IsReady() == FALSE)
            {
                E_ASSERT(0 && "メモリマネージャーが初期化されていない．");
                return NULL;
            }

            // ページも初期化されていないとだめ
            if (this->_IsInitMemoryPage(in_page) == FALSE)
            {
                E_ASSERT(0 && "ページが初期化されていない．");
                return NULL;
            }

            // メモリブロックヘッダの都合上最低アラインサイズを守る
            if ((in_alignSize % MINIMUM_ALIGN_SIZE) != 0)
            {
                E_ASSERT(0 && "alignSizeがMINIMUM_ALIGN_SIZEの倍数ではない．");
                return NULL;
            }

            // 0の時はMINIMUM_ALIGN_SIZEに
            in_alignSize = in_alignSize == 0 ? MINIMUM_ALIGN_SIZE : in_alignSize;

            // 空きメモリブロックをたどって, 確保したい容量より大きい奴を探す
            BlockHeader* pFreeMemoryBlock = this->_memoryPageInfoArray[in_page]._pMemoryBlockTop;
            if (pFreeMemoryBlock == NULL)
            {
                // 空きが見つからなかった
                return NULL;
            }

            // 後ろからとる場合降順にする
            if (in_locateType == ALLOCATE_LOCATE_LAST)
            {
                // 一番後ろまでたどっていく
                while (pFreeMemoryBlock->_pNext != NULL)
                    pFreeMemoryBlock = pFreeMemoryBlock->_pNext;
            }

            if (pFreeMemoryBlock == NULL)
            {
                // 空きが見つからなかった
                return NULL;
            }

            // 空きのどこからとるかというオフセット
            // 0ならいらない
            // それ以外の場合はシステム使用分も含まれる
            Uint32 offsetSize = 0;
            // 後ろのパディング
            Uint32 paddingSize = 0;
            Uint32 memoryBlockSize = 0;

            while (pFreeMemoryBlock != NULL)
            {
                // 空いていて, かつ最低限要求量が確保できるか？
                if (pFreeMemoryBlock->_useFlag == 0 && pFreeMemoryBlock->_allocateSize >= in_allocateSize)
                {
                    // 後ろのパディングサイズを割り出す
                    // 後ろはMINIMUM_ALIGN_SIZEの倍数であればいい
                    // また頭の方は最低MINIMUM_ALIGN_SIZEの倍数のはずなので
                    // 単純に要求サイズから割り出せる
                    paddingSize = in_allocateSize % MINIMUM_ALIGN_SIZE;
                    if (paddingSize != 0)
                    {
                        // 0じゃないならパディング必要
                        paddingSize = MINIMUM_ALIGN_SIZE - paddingSize;
                    }

                    // 後ろからとる場合オフセットの割り出しが違う
                    if (in_locateType == ALLOCATE_LOCATE_TOP)
                    {
                        // 前からとるとき
                        // アラインを考えて実際に置けるかどうか
                        Byte* startAddr = reinterpret_cast<Byte*>(pFreeMemoryBlock) + this->_GetMemoryBlockHeaderSize();

                        // 開始位置のオフセットを割り出す
                        offsetSize = reinterpret_cast<Ptr>(startAddr) % in_alignSize;
                        if (offsetSize != 0)
                        {
                            // ぴったりじゃないならスタート位置にオフセットが必要
                            offsetSize = in_alignSize - offsetSize;

                            // 一個余分に増えることになるのでシステム使用分を考慮に入れる
                            // オフセットが小さい場合
                            // オフセットにシステム使用分（ただしアラインサイズとの倍数になるように）足しこむ
                            if (offsetSize < this->_GetMemoryBlockSystemDataSize())
                            {
                                // システム使用分に足りないサイズをアラインサイズで割って切り上げて
                                // その数分アラインサイズを足しこむ
                                offsetSize += (((this->_GetMemoryBlockSystemDataSize() - offsetSize) + in_alignSize - 1) / in_alignSize) * in_alignSize;
                            }
                        }
                    }
                    else
                    {
                        // 後ろからとるとき
                        // アライン考慮せず一番後ろにおいた場合の開始アドレス
                        Ptr lastStartAddr = reinterpret_cast<Ptr>(
                            reinterpret_cast<Byte*>(pFreeMemoryBlock) + pFreeMemoryBlock->_size - _GetMemoryBlockFooterSize() - (in_allocateSize + paddingSize));

                        // 開始位置のオフセットを割り出す
                        offsetSize = lastStartAddr % in_alignSize;
                        if (offsetSize != 0)
                        {
                            // ぴったりじゃない
                            // なので、ぴったりのところまで引く

                            // その場合分割が必要なので, 後ろの方のシステム使用分を最低限とれるように
                            if (offsetSize < this->_GetMemoryBlockSystemDataSize())
                            {
                                // システム使用分に足りないサイズをアラインサイズで割って切り上げて
                                // その数分アラインサイズを足しこむ
                                offsetSize += (((this->_GetMemoryBlockSystemDataSize() - offsetSize) + in_alignSize - 1) / in_alignSize) * in_alignSize;
                            }
                        }

                        // 後ろからのオフセットなので前からに変換
                        // 0になった場合はオフセットはいらないということになる
                        offsetSize = pFreeMemoryBlock->_size - offsetSize - (in_allocateSize + paddingSize) - this->_GetMemoryBlockSystemDataSize();

                        // オフセットが0じゃない場合,
                        // マイナス（Uintなので最上位ビットが1で判断）になったり
                        // システム使用サイズより小さくなったら絶対とれないので,
                        // 極端に大きくしてとれないようにする．
                        if (
                            offsetSize != 0
                            && (offsetSize & 0x80000000 || offsetSize < _GetMemoryBlockSystemDataSize())
                            )
                        {
                            // 適当に
                            // これで2G弱までなら問題ないはず
                            offsetSize = 0x80000000;
                        }
                    }

                    // システム使用分＋オフセット＋確保したい容量＋パディングを足した分とれるか？
                    // とりあえず とりたいメモリブロックのサイズ
                    memoryBlockSize = this->_GetMemoryBlockSystemDataSize() + in_allocateSize + paddingSize;

                    // それにオフセットを足して 必要なサイズがとれるか
                    // あるいはオフセット＋要求量＋パディングがまったく同じな場合もOK（置き換えられるから）

                    // メモリ確保可能
                    if (pFreeMemoryBlock->_allocateSize >= (memoryBlockSize + offsetSize))
                        break;
                    if (pFreeMemoryBlock->_allocateSize == (offsetSize + in_allocateSize + paddingSize))
                        break;
                }

                // 次へ
                // 前からとる場合は昇順
                // 後ろからとる場合降順にする
                if (in_locateType == ALLOCATE_LOCATE_TOP)
                {
                    pFreeMemoryBlock = pFreeMemoryBlock->_pNext;
                }
                else
                {
                    pFreeMemoryBlock = pFreeMemoryBlock->_pPrev;
                }
            }

            // 空きが見つからなかった
            if (pFreeMemoryBlock == NULL)
                return NULL;

            // アラインぴったりの位置なら置き換えられる
            // そうじゃないなら二つに分ける
            if (offsetSize != 0)
            {
                // アラインぴったりじゃないので前を二つに分ける
                pFreeMemoryBlock = this->_SplitMemoryBlock(pFreeMemoryBlock, offsetSize);
            }

            // 必要な部分以外わける
            BlockHeader* pAllocateMemoryBlock = pFreeMemoryBlock;

            // 要求量 + パディングが空き容量と同じなら分割しないでそのまま置き換えでOK
            if (in_allocateSize + paddingSize != pAllocateMemoryBlock->_allocateSize)
            {
                // 二つに分けて前を使用
                // 後ろは空きのまま
                pFreeMemoryBlock = this->_SplitMemoryBlock(pAllocateMemoryBlock, this->_GetMemoryBlockSystemDataSize() + in_allocateSize + paddingSize);
            }

            // 利用ブロックに利用するメモリ情報を設定
            pAllocateMemoryBlock->_size = memoryBlockSize;
            pAllocateMemoryBlock->_allocateSize = in_allocateSize;
            pAllocateMemoryBlock->_alignSize = in_alignSize;
            pAllocateMemoryBlock->_paddingSize = static_cast<Uint8>(paddingSize);
            pAllocateMemoryBlock->_page = in_page;
            pAllocateMemoryBlock->_useFlag = 1;

#ifdef _HOBBY_ENGINE_DEBUG
            // ファイル名をコピーする
            {
                Sint32 maxCount = E_ARRAY_NUM(pAllocateMemoryBlock->_fileName);
                Sint32 count = E_MIN(static_cast<Sint32>(::strlen(in_pFile)), maxCount);
                ::strncpy_s(pAllocateMemoryBlock->_fileName, in_pFile, count * sizeof(pAllocateMemoryBlock->_fileName[0]));
            }
            pAllocateMemoryBlock->_line = in_line;

            // 新規メモリな感じ
            this->_FillNewMemoryBlock(pAllocateMemoryBlock);
#endif

            // 返すのは使用できる領域の先頭
            return (reinterpret_cast<BlockHeader*>(reinterpret_cast<Byte*>(pAllocateMemoryBlock) + this->_GetMemoryBlockHeaderSize()));
        }

        /// <summary>
        /// メモリ解放
        /// </summary>
        /// <param name="in_pAllocatedMemory">管理側で確保したメモリのポインタ</param>
        void Manager::FreeMemory(void* in_pAllocatedMemory)
        {
            // 受け取ったのは使用できるメモリの先頭
            BlockHeader* pMemoryBlock =
                reinterpret_cast<BlockHeader*>(reinterpret_cast<Byte*>(in_pAllocatedMemory) - this->_GetMemoryBlockHeaderSize());

            E_ASSERT(this->_IsValidMemoryBlock(pMemoryBlock) && "メモリマネージャーの管理領域ではないか, メモリブロックが破損している");
            E_ASSERT(pMemoryBlock->_useFlag != 0 && "すでに解放されている．");

            // 未使用にする
            pMemoryBlock->_useFlag = 0;

            // パディングの後始末
            pMemoryBlock->_allocateSize += pMemoryBlock->_paddingSize;
            pMemoryBlock->_paddingSize = 0;

#ifdef _HOBBY_ENGINE_DEBUG
            memset(pMemoryBlock->_fileName, 0, E_ARRAY_SIZE(pMemoryBlock->_fileName));
            pMemoryBlock->_line = 0;
#endif

            // 前に空きがあって隙間がないならマージ
            if (pMemoryBlock->_pPrev != NULL && pMemoryBlock->_pPrev->_useFlag == 0)
            {
                BlockHeader* pTempMemoryBlock;
                pTempMemoryBlock = this->_MergeMemoryBlock(pMemoryBlock->_pPrev, pMemoryBlock);

                if (pTempMemoryBlock != NULL)
                {
                    pMemoryBlock = pTempMemoryBlock;
                }
            }

            // 後ろに空きがあって隙間がないならマージ
            if (pMemoryBlock->_pNext != NULL && pMemoryBlock->_pNext->_useFlag == 0)
            {
                this->_MergeMemoryBlock(pMemoryBlock, pMemoryBlock->_pNext);
            }

#ifdef _HOBBY_ENGINE_DEBUG
            // 解放したメモリブロックを初期化
            this->_FillFreeMemoryBlock(pMemoryBlock);
#endif
        }

        /// <summary>
        /// 確保したメモリサイズを取得
        /// </summary>
        /// <param name="in_pAllocatedMemory">管理側で確保したメモリのポインタ</param>
        /// <returns></returns>
        const Uint32 Manager::GetAllocatedMemorySize(void* in_pAllocatedMemory)
        {
            // 受け取ったのは使用できるメモリの先頭
            BlockHeader* pMemoryBlock =
                reinterpret_cast<BlockHeader*>(reinterpret_cast<Byte*>(in_pAllocatedMemory) - this->_GetMemoryBlockHeaderSize());

            E_ASSERT(_IsValidMemoryBlock(pMemoryBlock) && "メモリマネージャーの管理領域ではないか、メモリブロックが破損している．");
            E_ASSERT(pMemoryBlock->_useFlag == 1 && "開き領域のサイズを取得しようとした．");

            return pMemoryBlock->_allocateSize;
        }

        /// <summary>
        /// 指定メモリブロックの前に, メモリブロックを連結
        /// </summary>
        /// <param name="in_pMemoryBlock">連結させたいメモリブロック</param>
        /// <param name="in_pTargetMemoryBlock">連結するメモリブロック</param>
        void	Manager::_AddListMemoryBlockPrev(BlockHeader* in_pMemoryBlock, BlockHeader* in_pTargetMemoryBlock)
        {
            E_ASSERT(in_pMemoryBlock && "連結させたいメモリブロックがない");
            E_ASSERT(in_pTargetMemoryBlock && "連結するメモリブロックがない");
            E_ASSERT(in_pMemoryBlock->_page == in_pTargetMemoryBlock->_page && "ページが違う．");
            E_ASSERT(in_pMemoryBlock->_useFlag == in_pTargetMemoryBlock->_useFlag && "使用状況が違う．");

            // 前の処理
            if (in_pTargetMemoryBlock->_pPrev == NULL)
            {
                // 先頭
                this->_memoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop = in_pMemoryBlock;
            }
            else
            {
                // 先頭じゃない
                in_pTargetMemoryBlock->_pPrev->_pNext = in_pMemoryBlock;
            }

            // 挿入処理
            in_pMemoryBlock->_pPrev = in_pTargetMemoryBlock->_pPrev;
            in_pMemoryBlock->_pNext = in_pTargetMemoryBlock;
            in_pTargetMemoryBlock->_pPrev = in_pMemoryBlock;
        }

        /// <summary>
        /// 指定メモリブロックの後ろに別のメモリブロックを連結
        /// </summary>
        /// <param name="in_pMemoryBlock">連結させたいメモリブロック</param>
        /// <param name="in_pTargetMemoryBlock">連結するメモリブロック</param>
        void Manager::_AddListMemoryBlockNext(BlockHeader* in_pMemoryBlock, BlockHeader* in_pTargetMemoryBlock)
        {
            E_ASSERT(in_pMemoryBlock && "連結させるメモリブロックがない");
            E_ASSERT(in_pTargetMemoryBlock && "連結するメモリブロックがない");
            E_ASSERT(in_pMemoryBlock->_page == in_pTargetMemoryBlock->_page && "ページが違う．");
            E_ASSERT(in_pMemoryBlock->_useFlag == in_pTargetMemoryBlock->_useFlag && "使用状況が違う．");

            // 後ろの処理
            if (in_pTargetMemoryBlock->_pNext != NULL)
            {
                // 最後じゃない
                in_pTargetMemoryBlock->_pNext->_pPrev = in_pMemoryBlock;
            }

            // 挿入処理
            in_pMemoryBlock->_pPrev = in_pTargetMemoryBlock;
            in_pMemoryBlock->_pNext = in_pTargetMemoryBlock->_pNext;
            in_pTargetMemoryBlock->_pNext = in_pMemoryBlock;
        }

        /// <summary>
        /// メモリブロックをリストから外す
        /// </summary>
        /// <param name="in_pMemoryBlock">外すメモリブロック</param>
        void Manager::_DelListMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            E_ASSERT(in_pMemoryBlock && "外すメモリブロックが存在しない");

            // 最後か？
            if (in_pMemoryBlock->_pNext == NULL)
            {
                // 最後
                // 先頭か？
                if (in_pMemoryBlock->_pPrev == NULL)
                {
                    // 先頭．
                    this->_memoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop = NULL;
                }
                else
                {
                    // 先頭じゃない
                    // 一個前を最後にする
                    in_pMemoryBlock->_pPrev->_pNext = NULL;
                }
            }
            else
            {
                // 最後じゃない
                // 先頭か？
                if (in_pMemoryBlock->_pPrev == NULL)
                {
                    // 先頭
                    // 自分を飛ばす
                    this->_memoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop = in_pMemoryBlock->_pNext;
                    in_pMemoryBlock->_pNext->_pPrev = NULL;
                }
                else
                {
                    // 先頭じゃない
                    // 一個前と後ろを連結
                    in_pMemoryBlock->_pPrev->_pNext = in_pMemoryBlock->_pNext;
                    in_pMemoryBlock->_pNext->_pPrev = in_pMemoryBlock->_pPrev;
                }
            }

            // 自分の始末
            in_pMemoryBlock->_pPrev = NULL;
            in_pMemoryBlock->_pNext = NULL;
        }

        /// <summary>
        /// メモリブロックを指定サイズで2つに分割する
        /// 分割して新規作成したメモリブロックはアライメントは不明
        /// 空きメモリ関係で使用しているのでそれ以外には使用しないようにする
        /// </summary>
        /// <param name="in_pMemoryBlock">分割したいメモリブロック</param>
        /// <param name="in_splitSize">分割したいサイズ(システム使用分を含める)</param>
        /// <returns>NULL 分割失敗 / 非NULL 分割して新規作成したメモリブロック(後ろの方)</returns>
        Manager::BlockHeader* Manager::_SplitMemoryBlock(BlockHeader* in_pMemoryBlock, Uint32 in_splitSize)
        {
            if (in_pMemoryBlock->_allocateSize < in_splitSize)
            {
                E_ASSERT(0 && "分割するサイズが残りヒープサイズより小さい");
                return NULL;
            }

            if (in_splitSize < this->_GetMemoryBlockSystemDataSize())
            {
                E_ASSERT(0 && "分割するサイズがブロックのシステムサイズより小さい");
                return NULL;
            }

            BlockHeader* pSplitMemoryBlock = reinterpret_cast<BlockHeader*>(reinterpret_cast<Byte*>(in_pMemoryBlock) + in_splitSize);

            // パディングは無効になる
            in_pMemoryBlock->_allocateSize += in_pMemoryBlock->_paddingSize;
            in_pMemoryBlock->_paddingSize = 0;

            // ヘッダコピー
            ::memcpy(pSplitMemoryBlock, in_pMemoryBlock, this->_GetMemoryBlockHeaderSize());

            // サイズ減らす
            in_pMemoryBlock->_size = in_splitSize;
            in_pMemoryBlock->_allocateSize = in_splitSize - this->_GetMemoryBlockSystemDataSize();

            // 新しく作った方も減らす
            // 減る量は_size、_allocateSizeとも同じであることに注意
            pSplitMemoryBlock->_size -= in_splitSize;
            pSplitMemoryBlock->_allocateSize -= in_splitSize;

            // アライン初期化
            pSplitMemoryBlock->_alignSize = MINIMUM_ALIGN_SIZE;

#ifdef _HOBBY_ENGINE_DEBUG
            // マジックNoセット
            this->_SetMemoryBlockMagicNo(in_pMemoryBlock);
            // 念のため
            this->_SetMemoryBlockMagicNo(pSplitMemoryBlock);
#endif

            // 元のやつの後ろに分割した分を追加する
            this->_AddListMemoryBlockNext(pSplitMemoryBlock, in_pMemoryBlock);

            return pSplitMemoryBlock;
        }

        /// <summary>
        /// 指定した2つのメモリブロックを一つのメモリブロックにする
        /// 空きメモリ用のみ使うのでそれ以外では使ってはいけない
        /// 2つのメモリブロックは昇順なのが条件
        /// </summary>
        /// <param name="in_pMemoryBlock1">前にマージしたいメモリブロック</param>
        /// <param name="in_pMemoryBlock2">後ろにマージしたいメモリブロック</param>
        /// <returns>NULL マージ失敗 / 非NULL マージ成功でマージしたメモリブロック</returns>
        Manager::BlockHeader* Manager::_MergeMemoryBlock(BlockHeader* in_pMemoryBlock1, BlockHeader* in_pMemoryBlock2)
        {
            E_ASSERT(in_pMemoryBlock1->_page == in_pMemoryBlock2->_page && "ページが違う．");
            E_ASSERT(in_pMemoryBlock1->_useFlag == in_pMemoryBlock2->_useFlag && "使用状況が違う．");
            E_ASSERT(reinterpret_cast<BlockHeader*>(reinterpret_cast<Byte*>(in_pMemoryBlock1) + in_pMemoryBlock1->_size) == in_pMemoryBlock2 && "メモリブロックが連続していない．");

            // まず後ろを外す
            this->_DelListMemoryBlock(in_pMemoryBlock2);

            // マージする場合, パディングはなくなる（空きメモリ系でしか使わないので問題ないはず）
            in_pMemoryBlock1->_allocateSize += in_pMemoryBlock1->_paddingSize;
            in_pMemoryBlock1->_paddingSize = 0;

            // マージする分足しこむ
            in_pMemoryBlock1->_size += in_pMemoryBlock2->_size;
            // 後ろの分丸々入るので, m_sizeであることに注意
            in_pMemoryBlock1->_allocateSize += in_pMemoryBlock2->_size;

#ifdef _HOBBY_ENGINE_DEBUG
            // マジックNoのセット
            this->_SetMemoryBlockMagicNo(in_pMemoryBlock1);
#endif

            return in_pMemoryBlock1;
        }

#ifdef _HOBBY_ENGINE_DEBUG

        /// <summary>
        /// 指定メモリページの情報を出力
        /// </summary>
        /// <param name="in_page">ページ</param>
        void Manager::PrintMemoryPageInfo(Uint8	in_page)
        {
            Uint32 usedTotal = 0;
            Uint32 freeTotal = 0;
            Uint32 usedCount = 0;
            Uint32 freeCount = 0;
            BlockHeader* pLargestUsedBlock = NULL;
            BlockHeader* pLargestFreeBlock = NULL;

            // ページ情報の表示
            E_LOG_LINE(E_STR_TEXT("============================================"));
            E_LOG_LINE(E_STR_TEXT("page[%d]"), in_page);
            E_LOG_LINE(E_STR_TEXT("============================================"));
#ifdef _X64
            E_LOG_LINE(E_STR_TEXT("TopAddr    :0llx%8.8llx"), reinterpret_cast<Ptr>(this->_memoryPageInfoArray[in_page]._pTopAddr));
#else
            E_LOG_LINE(E_STR_TEXT("TopAddr    :0lx%8.8lx"), reinterpret_cast<Ptr>(this->_memoryPageInfoArray[in_page]._pTopAddr));
#endif
            E_LOG_LINE(E_STR_TEXT("Size       :0lx%8.8lx"), this->_memoryPageInfoArray[in_page]._size);
            E_LOG_LINE(E_STR_TEXT("--------------------------------------------"));

            // チェックする
            this->CheckMemoryBlockByPage(in_page);

            // 使用分の表示
            BlockHeader* pUsedMemoryBlock = this->_memoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pUsedMemoryBlock != NULL)
            {
                if (pUsedMemoryBlock->_useFlag == 1)
                {
                    Ptr blockAddr = reinterpret_cast<Ptr>(pUsedMemoryBlock);
#ifdef _X64
                    E_LOG_LINE(E_STR_TEXT("0llx%8.8llx: 0x%8.8x (0x%8.8x) [%d] {%d}: %hs(%d)"),
                        blockAddr + this->_GetMemoryBlockHeaderSize(),
                        pUsedMemoryBlock->_allocateSize,
                        pUsedMemoryBlock->_size,
                        pUsedMemoryBlock->_paddingSize,
                        pUsedMemoryBlock->_alignSize,
                        pUsedMemoryBlock->_fileName,
                        pUsedMemoryBlock->_line);
#else
                    E_LOG_LINE(E_STR_TEXT("0x%8.8x: 0x%8.8x (0x%8.8x) [%d] {%d}: %s(%d)"),
                        blockAddr + this->_GetMemoryBlockHeaderSize(),
                        pUsedMemoryBlock->_allocateSize,
                        pUsedMemoryBlock->_size,
                        pUsedMemoryBlock->_paddingSize,
                        pUsedMemoryBlock->_alignSize,
                        pUsedMemoryBlock->_pFile,
                        pUsedMemoryBlock->_line);
#endif

                    usedTotal += pUsedMemoryBlock->_allocateSize;
                    ++usedCount;

                    if (pLargestUsedBlock == NULL || pLargestUsedBlock->_allocateSize < pUsedMemoryBlock->_allocateSize)
                    {
                        pLargestUsedBlock = pUsedMemoryBlock;
                    }
                }

                pUsedMemoryBlock = pUsedMemoryBlock->_pNext;
            }

            E_LOG_LINE(E_STR_TEXT(""));
            E_LOG_LINE(E_STR_TEXT("usedCount: %d"), usedCount);
            E_LOG_LINE(E_STR_TEXT("usedTotal: 0x%8.8x"), usedTotal);

            if (pLargestUsedBlock != NULL)
            {
                Ptr blockAddr = reinterpret_cast<Ptr>(pLargestFreeBlock);
#ifdef _X64
                E_LOG_LINE(E_STR_TEXT("maxUsed: 0llx%8.8llx 0x%8.8x (0x%8.8x)"),
                    blockAddr + this->_GetMemoryBlockHeaderSize(),
                    pLargestUsedBlock->_allocateSize,
                    pLargestUsedBlock->_size);
#else
                E_LOG_LINE(E_STR_TEXT("maxUsed: 0x%8.8x 0x%8.8x (0x%8.8x)"),
                    blockAddr + this->_GetMemoryBlockHeaderSize(),
                    pLargestUsedBlock->_allocateSize,
                    pLargestUsedBlock->_size);
#endif
            }

            E_LOG_LINE(E_STR_TEXT("-------------------------------------------"));

            // 開き領域の表示
            BlockHeader* pFreeMemoryBlock = this->_memoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pFreeMemoryBlock != NULL)
            {
                if (pFreeMemoryBlock->_useFlag == 0)
                {
                    Ptr blockAddr = reinterpret_cast<Ptr>(pFreeMemoryBlock);
#ifdef _X64
                    E_LOG_LINE(E_STR_TEXT("0llx%8.8llx:0x%8.8x (0x%8.8x)"),
                        (blockAddr + this->_GetMemoryBlockHeaderSize()),
                        pFreeMemoryBlock->_allocateSize,
                        pFreeMemoryBlock->_size);
#else
                    E_LOG_LINE(E_STR_TEXT("0x%8.8x:0x%8.8x (0x%8.8x)"),
                        (blockAddr + this->_GetMemoryBlockHeaderSize()),
                        pFreeMemoryBlock->_allocateSize,
                        pFreeMemoryBlock->_size);
#endif
                    freeTotal += pFreeMemoryBlock->_allocateSize;
                    ++freeCount;

                    if (pLargestFreeBlock == NULL || pLargestFreeBlock->_allocateSize < pFreeMemoryBlock->_allocateSize)
                    {
                        pLargestFreeBlock = pFreeMemoryBlock;
                    }
                }

                pFreeMemoryBlock = pFreeMemoryBlock->_pNext;
            }

            E_LOG_LINE(E_STR_TEXT(""));
            E_LOG_LINE(E_STR_TEXT("freeCount: %d"), freeCount);
            E_LOG_LINE(E_STR_TEXT("freeTotal: 0x%8.8x"), freeTotal);

            if (pLargestFreeBlock != NULL)
            {
                E_LOG_LINE(E_STR_TEXT("maxFree: 0x%8.8x"), pLargestFreeBlock->_allocateSize);
            }

            E_LOG_LINE(E_STR_TEXT("============================================"));
        }

        /// <summary>
        /// 全メモリ情報表示
        /// </summary>
        void Manager::PrintAllMemoryInfo()
        {
            for (Uint8 i = 0; i < MEMORY_PAGE_MAX; ++i)
            {
                // 使用していないページは表示しない
                if (this->_IsInitMemoryPage(i))
                {
                    this->PrintMemoryPageInfo(i);
                }
            }
        }

        /// <summary>
        /// 指定メモリページのメモリブロックが正常かチェック
        /// </summary>
        /// <param name="in_page">ページ</param>
        /// <returns>TRUE 全メモリブロックが正常 / FALSE 異常なメモリブロックがある</returns>
        const Bool Manager::CheckMemoryBlockByPage(Uint8 in_page)
        {
            BlockHeader* pMemoryBlock = this->_memoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pMemoryBlock != NULL)
            {
                if (this->_IsValidMemoryBlock(pMemoryBlock) == FALSE)
                {
                    E_ASSERT(0 && "門番が死んでいる．");
                    return FALSE;
                }

                pMemoryBlock = pMemoryBlock->_pNext;
            }

            return TRUE;
        }

        /// <summary>
        /// 全ページのメモリブロックが正常かチェック
        /// </summary>
        /// <returns>TRUE 全てのページのメモリブロックは正常 / FALSE 異常なメモリブロックがある</returns>
        const Bool Manager::CheckAllMemoryBlock()
        {
            for (Uint8 i = 0; i < MEMORY_PAGE_MAX; ++i)
            {
                //	使用していないページはチェックしない．
                if (this->CheckMemoryBlockByPage(i))
                {
                    if (this->CheckMemoryBlockByPage(i) == FALSE)
                        return FALSE;
                }
            }

            return TRUE;
        }

        /// <summary>
        /// 指定メモリブロックを空き用の固定値で塗りつぶす
        /// </summary>
        /// <param name="in_pMemoryBlock">塗りつぶすメモリブロック</param>
        void Manager::_FillFreeMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            ::memset(reinterpret_cast<Uint8*>(in_pMemoryBlock) + this->_GetMemoryBlockHeaderSize(),
                FREEMEMORY_FILL_DATA, in_pMemoryBlock->_allocateSize + in_pMemoryBlock->_paddingSize);
        }

        /// <summary>
        /// 指定メモリブロックを上書き用の固定値で塗りつぶす
        /// </summary>
        /// <param name="in_pMemoryBlock">塗りつぶすメモリブロック</param>
        void Manager::_FillNewMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            ::memset(reinterpret_cast<Uint8*>(in_pMemoryBlock) + this->_GetMemoryBlockHeaderSize(),
                NEWMEMORY_FILL_DATA, in_pMemoryBlock->_allocateSize + in_pMemoryBlock->_paddingSize);
        }
#endif
    }
}

