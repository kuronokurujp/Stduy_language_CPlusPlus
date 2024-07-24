#include "MemoryManager.h"

#include <memory>

namespace Core
{
    namespace Memory
    {
        Manager::Manager()
        {
        }

        const Bool Manager::Release()
        {
            if (this->_IsReady() == FALSE) return TRUE;

            // TODO: メモリが残っているかチェック

            // 解放する
            // TODO: プラットフォームのヒープ解放に切り替える
            ::_aligned_free(this->_pHeapTop);

            // 痕跡を消す
            this->_pHeapTop  = NULL;
            this->_uHeapSize = 0;

            ::memset(this->_aMemoryPageInfoArray, 0, sizeof(this->_aMemoryPageInfoArray));

            return TRUE;
        }

        /// <summary>
        /// 初期化
        /// 利用するヒープサイズ確保
        /// </summary>
        /// <param name="in_useHeepSize">管理側で利用するヒープサイズ</param>
        /// <returns></returns>
        const Bool Manager::Start(const Uint32 in_uUseHeapSize)
        {
            //	既に初期化されているか、チェックする．
            if (this->_IsReady())
            {
                HE_ASSERT(0 && "すでに初期化されている．");
                return FALSE;
            }

            // ヒープからごっそりとる
            // TODO: プラットフォーム用のヒープ取得に切り替え
            // プラットフォームのアライメントに合わせたい
            // とり方は各プラットフォームによって変えること
            this->_pHeapTop = ::_aligned_malloc(in_uUseHeapSize, MinimumAlignSize);
            if (this->_pHeapTop == NULL)
            {
                //	取れなかった．
                HE_ASSERT(0 && "取得に失敗．");
                return FALSE;
            }

            this->_uHeapSize = in_uUseHeapSize;

            //	ページ情報初期化．
            ::memset(this->_aMemoryPageInfoArray, 0, sizeof(this->_aMemoryPageInfoArray));

            return TRUE;
        }

        /// <summary>
        /// メモリページを構築する
        /// 一度構築したら変える事はできない
        /// </summary>
        /// <param name="in_pSetupInfoArray"></param>
        /// <param name="in_num">配列の個数</param>
        /// <returns>TRUE 成功 / FALSE 失敗</returns>
        const Bool Manager::SetupMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_uNum)
        {
            HE_ASSERT(in_pSetupInfoArray && "ページ設定情報がない");

            if (this->_IsReady() == FALSE)
            {
                HE_ASSERT(0 && "Managertが初期化されていない．");
                return FALSE;
            }

            // メモリページが一つでも初期化状態であればエラーにする
            for (Uint8 i = 0; i < MemoryPageMax; i++)
            {
                if (this->_ValidMemoryPage(i))
                {
                    HE_ASSERT(0 && "すでにページが有効になっているのでメモリページは構築済み．");
                    return FALSE;
                }
            }

            Uint32 uHeapOffset = 0;

            // 終端子まで繰り返してページを作成する
            for (Uint32 i = 0; i < in_uNum; ++i)
            {
                const PageSetupInfo* pTempSetupInfo = &in_pSetupInfoArray[i];

                // サイズが大きすぎる場合
                if (uHeapOffset + pTempSetupInfo->_uSize > _uHeapSize)
                {
                    HE_ASSERT(0 && "サイズが大きすぎる．");
                    return FALSE;
                }

                if (this->_InitMemoryPage(pTempSetupInfo->_chPage, uHeapOffset,
                                          pTempSetupInfo->_uSize) == FALSE)
                {
                    HE_ASSERT(0 && "メモリページの初期化に失敗．");
                    return FALSE;
                }

                // 作成ページのサイズ分はヒープを作ったので空いているヒープアドレスにする
                uHeapOffset += pTempSetupInfo->_uSize;
            }

            return TRUE;
        }

        /// <summary>
        /// メモリページ全体のリマップ
        /// </summary>
        /// <param name="in_pSetupInfoArray">メモリページ設定情報の配列</param>
        /// <param name="in_num">配列個数</param>
        /// <returns>TRUE 成功 / FALSE 失敗</returns>
        const Bool Manager::RemapMemoryPage(PageSetupInfo* in_pSetupInfoArray, const Uint32 in_uNum)
        {
            // 初期化されていない
            if (this->_IsReady() == FALSE)
            {
                HE_ASSERT(0 && "Managertが初期化されていない．");
                return FALSE;
            }

#ifdef HE_ENGINE_DEBUG
            // メモリブロックが壊れていないかチェック
            this->CheckAllMemoryBlock();
#endif
            Uint32 uaOffset[MemoryPageMax]     = {0};
            Uint32 uaSize[MemoryPageMax]       = {0};
            Bool baNeedInitFlag[MemoryPageMax] = {FALSE};

            //	サイズをクリアしておく(サイズが0ならいらないページ)
            ::memset(uaSize, 0, HE_ARRAY_SIZE(uaSize));
            ::memset(baNeedInitFlag, 0, HE_ARRAY_SIZE(baNeedInitFlag));

            // 最初にチェックとオフセット等の計算を別枠でする
            // そうしないとDEBUG時で一個前のサイズが大きくなったときメモリブロックのところがデバッグ情報で埋められてしまう
            Uint32 uOffset = 0;
            for (Uint32 i = 0; i < in_uNum; ++i)
            {
                PageSetupInfo* pTempSetupInfo = &in_pSetupInfoArray[i];

                uaSize[pTempSetupInfo->_chPage] = pTempSetupInfo->_uSize;

                // サイズ0なら消すだけなので特にチェックいらず
                if (uaSize[pTempSetupInfo->_chPage] != 0)
                {
                    // サイズが確保したヒープサイズを超えているのでエラー
                    if ((uOffset + pTempSetupInfo->_uSize) > this->_uHeapSize)
                    {
                        HE_ASSERT(0 && "サイズが大きすぎる．");
                        return FALSE;
                    }

                    // 全ページの形にしておく
                    uaOffset[pTempSetupInfo->_chPage]       = uOffset;
                    baNeedInitFlag[pTempSetupInfo->_chPage] = TRUE;

                    // すでに初期化されている場合チェック必要
                    if (this->_ValidMemoryPage(pTempSetupInfo->_chPage))
                    {
                        // オフセットが違う
                        const Bool bDiffOffsetAddr =
                            reinterpret_cast<Sint8*>(
                                this->_aMemoryPageInfoArray[pTempSetupInfo->_chPage]._pTopAddr) !=
                            reinterpret_cast<Sint8*>(this->_pHeapTop) + uOffset;

                        // サイズが違う
                        const Bool bDiffSize =
                            this->_aMemoryPageInfoArray[pTempSetupInfo->_chPage]._uSize !=
                            pTempSetupInfo->_uSize;

                        // ページ初期化ができるかチェック
                        if (bDiffOffsetAddr || bDiffSize)
                        {
                            // 使用中のメモリブロックがあるならだめ
                            // サイズが違う場合は大きくなる分には問題ないし
                            // 小さくなる場合も使用していない部分なら問題ないけど
                            // さらにリマップして問題が発生した際に原因が見つけにくくなりそうなのでとりあえずこうしておく
                            if (this->_ExistUsedMemoryBlock(pTempSetupInfo->_chPage))
                            {
                                HE_ASSERT(0 &&
                                          "オフセットもしくはサイズの変更が指定されているが、使用中"
                                          "のメモリブロックが存在している．");
                                return FALSE;
                            }
                        }
                        else
                        {
                            // オフセットも変わらず,
                            // サイズも同じなのでそのままページが使えるので初期化は不要
                            baNeedInitFlag[pTempSetupInfo->_chPage] = FALSE;
                        }
                    }
                }

                uOffset += pTempSetupInfo->_uSize;
            }

            // 指定されていないページは消す
            // これも先にやっておかないと
            // DEBUG時で一個前のサイズが大きくなったときメモリブロックのところがデバッグ情報で埋められてしまう．
            for (Uint8 i = 0; i < MemoryPageMax; ++i)
            {
                // サイズが0になっているページは指定していないページなので消す
                if (uaSize[i] == 0)
                {
                    // 消すのにごみが残っていてはだめ
                    if (this->_ExistUsedMemoryBlock(i))
                    {
                        HE_ASSERT(0 &&
                                  "ページを消去するのに、使用中のメモリブロックが存在している．");
                        return FALSE;
                    }

                    this->_aMemoryPageInfoArray[i]._pTopAddr        = 0;
                    this->_aMemoryPageInfoArray[i]._uSize           = 0;
                    this->_aMemoryPageInfoArray[i]._pMemoryBlockTop = NULL;
                }
            }

            // 最後に初期化が必要な場所を初期化
            for (Uint8 i = 0; i < MemoryPageMax; ++i)
            {
                //	初期化が必要ならしろ．
                if (baNeedInitFlag[i])
                {
                    if (this->_InitMemoryPage(i, uaOffset[i], uaSize[i]) == FALSE)
                    {
                        HE_ASSERT(0 && "メモリページの初期化に失敗．");
                        return FALSE;
                    }
                }
            }

            return TRUE;
        }

        const Bool Manager::IsValidMemory(void* in_pAllocatedMemory)
        {
            // 受け取ったのは使用できるメモリの先頭
            BlockHeader* pMemoryBlock = reinterpret_cast<BlockHeader*>(
                reinterpret_cast<Sint8*>(in_pAllocatedMemory) - this->_GetMemoryBlockHeaderSize());

            return this->_IsValidMemoryBlock(pMemoryBlock);
        }

        /// <summary>
        /// メモリページの初期化
        /// </summary>
        /// <param name="in_page">メモリページ指定</param>
        /// <param
        /// name="in_heepOffset">メモリページをどこから取るか(メモリマネージャー管理領域先頭からのオフセット。MINIMUM_ALIGN_SIZEの倍数であること)</param>
        /// <param
        /// name="in_useHeepSize">メモリページのサイズ（MINIMUM_ALIGN_SIZEの倍数であること）</param>
        const Bool Manager::_InitMemoryPage(Uint8 in_page, Uint32 in_uHeepOffset,
                                            Uint32 in_uUseHeepSize)
        {
            HE_ASSERT((in_page < HE_ARRAY_NUM(this->_aMemoryPageInfoArray)) &&
                      "指定ページが存在しない");
            HE_ASSERT((this->_GetMemoryBlockSystemDataSize() <= in_uUseHeepSize) &&
                      "ページで確保するサイズがブロックサイズより小さい");

            PageInfo* pPageInfo = &this->_aMemoryPageInfoArray[in_page];

            // ページ情報の初期化
            // 確保したヒープからページで利用するヒープ分を割り当てる
            pPageInfo->_pTopAddr =
                reinterpret_cast<void*>(reinterpret_cast<Sint8*>(this->_pHeapTop) + in_uHeepOffset);
            pPageInfo->_uSize = in_uUseHeepSize;

            // 最初のメモリブロックを設定する
            pPageInfo->_pMemoryBlockTop = reinterpret_cast<BlockHeader*>(pPageInfo->_pTopAddr);

            // 頭にヘッダ情報を書き込む
            pPageInfo->_pMemoryBlockTop->_uSize = in_uUseHeepSize;
            pPageInfo->_pMemoryBlockTop->_uAllocateSize =
                in_uUseHeepSize - this->_GetMemoryBlockSystemDataSize();
            pPageInfo->_pMemoryBlockTop->_pPrev       = NULL;
            pPageInfo->_pMemoryBlockTop->_pNext       = NULL;
            pPageInfo->_pMemoryBlockTop->_useFlag     = 0;
            pPageInfo->_pMemoryBlockTop->_alignSize   = MinimumAlignSize;
            pPageInfo->_pMemoryBlockTop->_page        = in_page;
            pPageInfo->_pMemoryBlockTop->_paddingSize = 0;

#ifdef HE_ENGINE_DEBUG
            ::memset(pPageInfo->_pMemoryBlockTop->_szFileName, 0,
                     HE_ARRAY_SIZE(pPageInfo->_pMemoryBlockTop->_szFileName));
            pPageInfo->_pMemoryBlockTop->_uLine = 0;

            // マジックNoセット
            this->_SetMemoryBlockMagicNo(pPageInfo->_pMemoryBlockTop);

            // 空きメモリな感じ
            this->_FillFreeMemoryBlock(pPageInfo->_pMemoryBlockTop);
#endif
            return TRUE;
        }

#ifdef HE_ENGINE_DEBUG
        /// <summary>
        /// メモリ確保
        /// アライメントサイズMinimumAlignSizeの倍数である必要がある
        /// </summary>
        /// <param name="in_allocateSize">確保したいサイズ</param>
        /// <param name="in_page">メモリページ</param>
        /// <param name="in_alignSize">メモリのアライメントサイズ</param>
        /// <param name="in_locateType">確保位置(前からなのか後ろからなのか)</param>
        /// <param name="in_pFile">呼び出したファイル名(デバッグ限定)</param>
        /// <param name="in_line">呼び出したファイルの行数(デバッグ限定)</param>
        /// <returns>NULL 確保失敗 / 非NULL 確保したメモリアドレス</returns>
        void* Manager::AllocateMemory(Uint32 in_uAllocateSize, Uint8 in_page, Uint8 in_alignSize,
                                      EAllocateLocateType in_eLocateType, const UTF8* in_szFile,
                                      Uint32 in_uLine)
#else
        /// <summary>
        /// メモリ確保
        /// アライメントサイズはMinimumAlignSizeの倍数である必要がある
        /// </summary>
        /// <param name="in_allocateSize">確保したいサイズ</param>
        /// <param name="in_page">メモリページ</param>
        /// <param name="in_alignSize">メモリのアライメントサイズ</param>
        /// <param name="in_locateType">確保位置(前からなのか後ろからなのか)</param>
        /// <returns>NULL 確保失敗 / 非NULL 確保したメモリアドレス</returns>
        void* Manager::AllocateMemory(Uint32 in_uAllocateSize, Uint8 in_page, Uint8 in_alignSize,
                                      EAllocateLocateType in_eLocateType)
#endif
        {
            // 確保サイズが0以下では確保できない
            HE_ASSERT((0 < in_uAllocateSize) && "サイズが0．");
            // 指定したページが存在するか
            HE_ASSERT((in_page < HE_ARRAY_NUM(this->_aMemoryPageInfoArray)) &&
                      "指定したページが存在しない");

            // 初期化されていないとだめ
            if (this->_IsReady() == FALSE)
            {
                HE_ASSERT(0 && "メモリマネージャーが初期化されていない．");
                return NULL;
            }

            // ページも初期化されていないとだめ
            if (this->_ValidMemoryPage(in_page) == FALSE)
            {
                HE_ASSERT(0 && "ページが初期化されていない．");
                return NULL;
            }

            // メモリブロックヘッダの都合上最低アラインサイズを守る
            if ((in_alignSize % MinimumAlignSize) != 0)
            {
                HE_ASSERT(0 && "alignSizeがMINIMUM_ALIGN_SIZEの倍数ではない．");
                return NULL;
            }

            // 0の時はMinimumAlignSizeに
            in_alignSize = in_alignSize == 0 ? MinimumAlignSize : in_alignSize;

            // 空きメモリブロックをたどって, 確保したい容量より大きい奴を探す
            BlockHeader* pFreeMemoryBlock = this->_aMemoryPageInfoArray[in_page]._pMemoryBlockTop;
            if (pFreeMemoryBlock == NULL)
            {
                // 空きが見つからなかった
                return NULL;
            }

            // 後ろからとる場合降順にする
            if (in_eLocateType == EAllocateLocteType_Last)
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
            Uint32 uOffsetSize = 0;
            // 後ろのパディング
            Uint32 uPaddingSize     = 0;
            Uint32 uMemoryBlockSize = 0;

            while (pFreeMemoryBlock != NULL)
            {
                // 空いていて, かつ最低限要求量が確保できるか？
                if (pFreeMemoryBlock->_useFlag == 0 &&
                    pFreeMemoryBlock->_uAllocateSize >= in_uAllocateSize)
                {
                    // 後ろのパディングサイズを割り出す
                    // 後ろはMinimumAlignSizeの倍数であればいい
                    // また頭の方は最低MinimumAlignSizeの倍数のはずなので
                    // 単純に要求サイズから割り出せる
                    uPaddingSize = in_uAllocateSize % MinimumAlignSize;
                    if (uPaddingSize != 0)
                    {
                        // 0じゃないならパディング必要
                        uPaddingSize = MinimumAlignSize - uPaddingSize;
                    }

                    // 後ろからとる場合オフセットの割り出しが違う
                    if (in_eLocateType == EAllocateLocateType_Top)
                    {
                        // 前からとるとき
                        // アラインを考えて実際に置けるかどうか
                        Sint8* pStartAddr = reinterpret_cast<Sint8*>(pFreeMemoryBlock) +
                                            this->_GetMemoryBlockHeaderSize();

                        // 開始位置のオフセットを割り出す
                        uOffsetSize = reinterpret_cast<Ptr>(pStartAddr) % in_alignSize;
                        if (uOffsetSize != 0)
                        {
                            // ぴったりじゃないならスタート位置にオフセットが必要
                            uOffsetSize = in_alignSize - uOffsetSize;

                            // 一個余分に増えることになるのでシステム使用分を考慮に入れる
                            // オフセットが小さい場合
                            // オフセットにシステム使用分（ただしアラインサイズとの倍数になるように）足しこむ
                            if (uOffsetSize < this->_GetMemoryBlockSystemDataSize())
                            {
                                // システム使用分に足りないサイズをアラインサイズで割って切り上げて
                                // その数分アラインサイズを足しこむ
                                uOffsetSize +=
                                    (((this->_GetMemoryBlockSystemDataSize() - uOffsetSize) +
                                      in_alignSize - 1) /
                                     in_alignSize) *
                                    in_alignSize;
                            }
                        }
                    }
                    else
                    {
                        // 後ろからとるとき
                        // アライン考慮せず一番後ろにおいた場合の開始アドレス
                        Ptr pLastStartAddr = reinterpret_cast<Ptr>(
                            reinterpret_cast<Sint8*>(pFreeMemoryBlock) + pFreeMemoryBlock->_uSize -
                            _GetMemoryBlockFooterSize() - (in_uAllocateSize + uPaddingSize));

                        // 開始位置のオフセットを割り出す
                        uOffsetSize = pLastStartAddr % in_alignSize;
                        if (uOffsetSize != 0)
                        {
                            // ぴったりじゃない
                            // なので、ぴったりのところまで引く

                            // その場合分割が必要なので,
                            // 後ろの方のシステム使用分を最低限とれるように
                            if (uOffsetSize < this->_GetMemoryBlockSystemDataSize())
                            {
                                // システム使用分に足りないサイズをアラインサイズで割って切り上げて
                                // その数分アラインサイズを足しこむ
                                uOffsetSize +=
                                    (((this->_GetMemoryBlockSystemDataSize() - uOffsetSize) +
                                      in_alignSize - 1) /
                                     in_alignSize) *
                                    in_alignSize;
                            }
                        }

                        // 後ろからのオフセットなので前からに変換
                        // 0になった場合はオフセットはいらないということになる
                        uOffsetSize = pFreeMemoryBlock->_uSize - uOffsetSize -
                                      (in_uAllocateSize + uPaddingSize) -
                                      this->_GetMemoryBlockSystemDataSize();

                        // オフセットが0じゃない場合,
                        // マイナス（Uintなので最上位ビットが1で判断）になったり
                        // システム使用サイズより小さくなったら絶対とれないので,
                        // 極端に大きくしてとれないようにする．
                        if (uOffsetSize != 0 && (uOffsetSize & 0x80000000 ||
                                                 uOffsetSize < _GetMemoryBlockSystemDataSize()))
                        {
                            // 適当に
                            // これで2G弱までなら問題ないはず
                            uOffsetSize = 0x80000000;
                        }
                    }

                    // システム使用分＋オフセット＋確保したい容量＋パディングを足した分とれるか？
                    // とりあえず とりたいメモリブロックのサイズ
                    uMemoryBlockSize =
                        this->_GetMemoryBlockSystemDataSize() + in_uAllocateSize + uPaddingSize;

                    // それにオフセットを足して 必要なサイズがとれるか
                    // あるいはオフセット＋要求量＋パディングがまったく同じな場合もOK（置き換えられるから）

                    // メモリ確保可能
                    if (pFreeMemoryBlock->_uAllocateSize >= (uMemoryBlockSize + uOffsetSize)) break;
                    if (pFreeMemoryBlock->_uAllocateSize ==
                        (uOffsetSize + in_uAllocateSize + uPaddingSize))
                        break;
                }

                // 次へ
                // 前からとる場合は昇順
                // 後ろからとる場合降順にする
                if (in_eLocateType == EAllocateLocateType_Top)
                {
                    pFreeMemoryBlock = pFreeMemoryBlock->_pNext;
                }
                else
                {
                    pFreeMemoryBlock = pFreeMemoryBlock->_pPrev;
                }
            }

            // 空きが見つからなかった
            if (pFreeMemoryBlock == NULL) return NULL;

            // アラインぴったりの位置なら置き換えられる
            // そうじゃないなら二つに分ける
            if (uOffsetSize != 0)
            {
                // アラインぴったりじゃないので前を二つに分ける
                pFreeMemoryBlock = this->_SplitMemoryBlock(pFreeMemoryBlock, uOffsetSize);
            }

            // 必要な部分以外わける
            BlockHeader* pAllocateMemoryBlock = pFreeMemoryBlock;

            // 要求量 + パディングが空き容量と同じなら分割しないでそのまま置き換えでOK
            if (in_uAllocateSize + uPaddingSize != pAllocateMemoryBlock->_uAllocateSize)
            {
                // 二つに分けて前を使用
                // 後ろは空きのまま
                pFreeMemoryBlock = this->_SplitMemoryBlock(pAllocateMemoryBlock,
                                                           this->_GetMemoryBlockSystemDataSize() +
                                                               in_uAllocateSize + uPaddingSize);
            }

            // 利用ブロックに利用するメモリ情報を設定
            pAllocateMemoryBlock->_uSize         = uMemoryBlockSize;
            pAllocateMemoryBlock->_uAllocateSize = in_uAllocateSize;
            pAllocateMemoryBlock->_alignSize     = in_alignSize;
            pAllocateMemoryBlock->_paddingSize   = static_cast<Uint8>(uPaddingSize);
            pAllocateMemoryBlock->_page          = in_page;
            pAllocateMemoryBlock->_useFlag       = 1;

#ifdef HE_ENGINE_DEBUG
            // ファイル名をコピーする
            {
                Sint32 uMaxCount = HE_ARRAY_NUM(pAllocateMemoryBlock->_szFileName);
                Sint32 uCount    = HE_MIN(static_cast<Sint32>(::strlen(in_szFile)), uMaxCount);
                ::strncpy_s(pAllocateMemoryBlock->_szFileName, in_szFile,
                            uCount * sizeof(pAllocateMemoryBlock->_szFileName[0]));
            }
            pAllocateMemoryBlock->_uLine = in_uLine;

            // 新規メモリな感じ
            this->_FillNewMemoryBlock(pAllocateMemoryBlock);
#endif

            // 返すのは使用できる領域の先頭
            const Uint32 headerSize = this->_GetMemoryBlockHeaderSize();
            Sint8* pAddr = reinterpret_cast<Sint8*>(pAllocateMemoryBlock) + headerSize;
            return (reinterpret_cast<void*>(pAddr));
        }

        /// <summary>
        /// メモリ解放
        /// </summary>
        /// <param name="in_pAllocatedMemory">管理側で確保したメモリのポインタ</param>
        void Manager::FreeMemory(void* in_pAllocatedMemory)
        {
            // 受け取ったのは使用できるメモリの先頭
            const Uint32 headerSize = this->_GetMemoryBlockHeaderSize();
            Sint8* pAddr = reinterpret_cast<Sint8*>(in_pAllocatedMemory) - headerSize;
            BlockHeader* pMemoryBlock = reinterpret_cast<BlockHeader*>(pAddr);

            HE_ASSERT(this->_IsValidMemoryBlock(pMemoryBlock) &&
                      "メモリマネージャーの管理領域ではないか, メモリブロックが破損している");
            HE_ASSERT(pMemoryBlock->_useFlag != 0 && "すでに解放されている．");

            // 未使用にする
            pMemoryBlock->_useFlag = 0;

            // パディングの後始末
            pMemoryBlock->_uAllocateSize += pMemoryBlock->_paddingSize;
            pMemoryBlock->_paddingSize = 0;

#ifdef HE_ENGINE_DEBUG
            ::memset(pMemoryBlock->_szFileName, 0, HE_ARRAY_SIZE(pMemoryBlock->_szFileName));
            pMemoryBlock->_uLine = 0;
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

#ifdef HE_ENGINE_DEBUG
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
            BlockHeader* pMemoryBlock = reinterpret_cast<BlockHeader*>(
                reinterpret_cast<Sint8*>(in_pAllocatedMemory) - this->_GetMemoryBlockHeaderSize());

            HE_ASSERT(_IsValidMemoryBlock(pMemoryBlock) &&
                      "メモリマネージャーの管理領域ではないか、メモリブロックが破損している．");
            HE_ASSERT(pMemoryBlock->_useFlag == 1 && "開き領域のサイズを取得しようとした．");

            return pMemoryBlock->_uAllocateSize;
        }

        /// <summary>
        /// 指定メモリブロックの前に, メモリブロックを連結
        /// </summary>
        /// <param name="in_pMemoryBlock">連結させたいメモリブロック</param>
        /// <param name="in_pTargetMemoryBlock">連結するメモリブロック</param>
        void Manager::_AddListMemoryBlockPrev(BlockHeader* in_pMemoryBlock,
                                              BlockHeader* in_pTargetMemoryBlock)
        {
            HE_ASSERT(in_pMemoryBlock && "連結させたいメモリブロックがない");
            HE_ASSERT(in_pTargetMemoryBlock && "連結するメモリブロックがない");
            HE_ASSERT(in_pMemoryBlock->_page == in_pTargetMemoryBlock->_page && "ページが違う．");
            HE_ASSERT(in_pMemoryBlock->_useFlag == in_pTargetMemoryBlock->_useFlag &&
                      "使用状況が違う．");

            // 前の処理
            if (in_pTargetMemoryBlock->_pPrev == NULL)
            {
                // 先頭
                this->_aMemoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop =
                    in_pMemoryBlock;
            }
            else
            {
                // 先頭じゃない
                in_pTargetMemoryBlock->_pPrev->_pNext = in_pMemoryBlock;
            }

            // 挿入処理
            in_pMemoryBlock->_pPrev       = in_pTargetMemoryBlock->_pPrev;
            in_pMemoryBlock->_pNext       = in_pTargetMemoryBlock;
            in_pTargetMemoryBlock->_pPrev = in_pMemoryBlock;
        }

        /// <summary>
        /// 指定メモリブロックの後ろに別のメモリブロックを連結
        /// </summary>
        /// <param name="in_pMemoryBlock">連結させたいメモリブロック</param>
        /// <param name="in_pTargetMemoryBlock">連結するメモリブロック</param>
        void Manager::_AddListMemoryBlockNext(BlockHeader* in_pMemoryBlock,
                                              BlockHeader* in_pTargetMemoryBlock)
        {
            HE_ASSERT(in_pMemoryBlock && "連結させるメモリブロックがない");
            HE_ASSERT(in_pTargetMemoryBlock && "連結するメモリブロックがない");
            HE_ASSERT(in_pMemoryBlock->_page == in_pTargetMemoryBlock->_page && "ページが違う．");
            HE_ASSERT(in_pMemoryBlock->_useFlag == in_pTargetMemoryBlock->_useFlag &&
                      "使用状況が違う．");

            // 後ろの処理
            if (in_pTargetMemoryBlock->_pNext != NULL)
            {
                // 最後じゃない
                in_pTargetMemoryBlock->_pNext->_pPrev = in_pMemoryBlock;
            }

            // 挿入処理
            in_pMemoryBlock->_pPrev       = in_pTargetMemoryBlock;
            in_pMemoryBlock->_pNext       = in_pTargetMemoryBlock->_pNext;
            in_pTargetMemoryBlock->_pNext = in_pMemoryBlock;
        }

        /// <summary>
        /// メモリブロックをリストから外す
        /// </summary>
        /// <param name="in_pMemoryBlock">外すメモリブロック</param>
        void Manager::_DelListMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            HE_ASSERT(in_pMemoryBlock && "外すメモリブロックが存在しない");

            // 最後か？
            if (in_pMemoryBlock->_pNext == NULL)
            {
                // 最後
                // 先頭か？
                if (in_pMemoryBlock->_pPrev == NULL)
                {
                    // 先頭．
                    this->_aMemoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop = NULL;
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
                    this->_aMemoryPageInfoArray[in_pMemoryBlock->_page]._pMemoryBlockTop =
                        in_pMemoryBlock->_pNext;
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
        Manager::BlockHeader* Manager::_SplitMemoryBlock(BlockHeader* in_pMemoryBlock,
                                                         Uint32 in_uSplitSize)
        {
            if (in_pMemoryBlock->_uAllocateSize < in_uSplitSize)
            {
                HE_ASSERT(0 && "分割するサイズが残りヒープサイズより小さい");
                return NULL;
            }

            if (in_uSplitSize < this->_GetMemoryBlockSystemDataSize())
            {
                HE_ASSERT(0 && "分割するサイズがブロックのシステムサイズより小さい");
                return NULL;
            }

            BlockHeader* pSplitMemoryBlock = reinterpret_cast<BlockHeader*>(
                reinterpret_cast<Sint8*>(in_pMemoryBlock) + in_uSplitSize);

            // パディングは無効になる
            in_pMemoryBlock->_uAllocateSize += in_pMemoryBlock->_paddingSize;
            in_pMemoryBlock->_paddingSize = 0;

            // ヘッダコピー
            ::memcpy(pSplitMemoryBlock, in_pMemoryBlock, this->_GetMemoryBlockHeaderSize());

            // サイズ減らす
            in_pMemoryBlock->_uSize         = in_uSplitSize;
            in_pMemoryBlock->_uAllocateSize = in_uSplitSize - this->_GetMemoryBlockSystemDataSize();

            // 新しく作った方も減らす
            // 減る量は_size、_allocateSizeとも同じであることに注意
            pSplitMemoryBlock->_uSize -= in_uSplitSize;
            pSplitMemoryBlock->_uAllocateSize -= in_uSplitSize;

            // アライン初期化
            pSplitMemoryBlock->_alignSize = MinimumAlignSize;

#ifdef HE_ENGINE_DEBUG
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
        Manager::BlockHeader* Manager::_MergeMemoryBlock(BlockHeader* in_pMemoryBlock1,
                                                         BlockHeader* in_pMemoryBlock2)
        {
            HE_ASSERT(in_pMemoryBlock1->_page == in_pMemoryBlock2->_page && "ページが違う．");
            HE_ASSERT(in_pMemoryBlock1->_useFlag == in_pMemoryBlock2->_useFlag &&
                      "使用状況が違う．");
            HE_ASSERT(reinterpret_cast<BlockHeader*>(reinterpret_cast<Sint8*>(in_pMemoryBlock1) +
                                                     in_pMemoryBlock1->_uSize) ==
                          in_pMemoryBlock2 &&
                      "メモリブロックが連続していない．");

            // まず後ろを外す
            this->_DelListMemoryBlock(in_pMemoryBlock2);

            // マージする場合, パディングはなくなる（空きメモリ系でしか使わないので問題ないはず）
            in_pMemoryBlock1->_uAllocateSize += in_pMemoryBlock1->_paddingSize;
            in_pMemoryBlock1->_paddingSize = 0;

            // マージする分足しこむ
            in_pMemoryBlock1->_uSize += in_pMemoryBlock2->_uSize;
            // 後ろの分丸々入るので, m_sizeであることに注意
            in_pMemoryBlock1->_uAllocateSize += in_pMemoryBlock2->_uSize;

#ifdef HE_ENGINE_DEBUG
            // マジックNoのセット
            this->_SetMemoryBlockMagicNo(in_pMemoryBlock1);
#endif

            return in_pMemoryBlock1;
        }

#ifdef HE_ENGINE_DEBUG

        /// <summary>
        /// 指定メモリページの情報を出力
        /// </summary>
        /// <param name="in_page">ページ</param>
        void Manager::PrintMemoryPageInfo(Uint8 in_page)
        {
            Uint32 uUsedTotal              = 0;
            Uint32 uFreeTotal              = 0;
            Uint32 uUsedCount              = 0;
            Uint32 uFreeCount              = 0;
            BlockHeader* pLargestUsedBlock = NULL;
            BlockHeader* pLargestFreeBlock = NULL;

            // ページ情報の表示
            HE_LOG_LINE(HE_STR_TEXT("============================================"));
            HE_LOG_LINE(HE_STR_TEXT("page[%d]"), in_page);
            HE_LOG_LINE(HE_STR_TEXT("============================================"));
#ifdef HE_X64
            HE_LOG_LINE(HE_STR_TEXT("TopAddr    :0llx%8.8llx"),
                        reinterpret_cast<Ptr>(this->_aMemoryPageInfoArray[in_page]._pTopAddr));
#else
            E_LOG_LINE(E_STR_TEXT("TopAddr    :0lx%8.8lx"),
                       reinterpret_cast<Ptr>(this->_memoryPageInfoArray[in_page]._pTopAddr));
#endif
            HE_LOG_LINE(HE_STR_TEXT("Size       :0lx%8.8lx"),
                        this->_aMemoryPageInfoArray[in_page]._uSize);
            HE_LOG_LINE(HE_STR_TEXT("--------------------------------------------"));

            // チェックする
            this->CheckMemoryBlockByPage(in_page);

            // 使用分の表示
            BlockHeader* pUsedMemoryBlock = this->_aMemoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pUsedMemoryBlock != NULL)
            {
                if (pUsedMemoryBlock->_useFlag == 1)
                {
                    Ptr pBlockAddr = reinterpret_cast<Ptr>(pUsedMemoryBlock);
#ifdef HE_X64
                    HE_LOG_LINE(HE_STR_TEXT("0llx%8.8llx: 0x%8.8x (0x%8.8x) [%d] {%d}: ")
                                    HE_STR_FORMAT_PURE_TEXT HE_STR_TEXT("(%d)"),
                                pBlockAddr + this->_GetMemoryBlockHeaderSize(),
                                pUsedMemoryBlock->_uAllocateSize, pUsedMemoryBlock->_uSize,
                                pUsedMemoryBlock->_paddingSize, pUsedMemoryBlock->_alignSize,
                                pUsedMemoryBlock->_szFileName, pUsedMemoryBlock->_uLine);
#else
                    E_LOG_LINE(E_STR_TEXT("0x%8.8x: 0x%8.8x (0x%8.8x) [%d] {%d}: ")
                                   E_STR_FORMAT_PURE_TEXT E_STR_TEXT("(%d)"),
                               blockAddr + this->_GetMemoryBlockHeaderSize(),
                               pUsedMemoryBlock->_allocateSize, pUsedMemoryBlock->_size,
                               pUsedMemoryBlock->_paddingSize, pUsedMemoryBlock->_alignSize,
                               pUsedMemoryBlock->_pFile, pUsedMemoryBlock->_line);
#endif

                    uUsedTotal += pUsedMemoryBlock->_uAllocateSize;
                    ++uUsedCount;

                    if (pLargestUsedBlock == NULL ||
                        pLargestUsedBlock->_uAllocateSize < pUsedMemoryBlock->_uAllocateSize)
                    {
                        pLargestUsedBlock = pUsedMemoryBlock;
                    }
                }

                pUsedMemoryBlock = pUsedMemoryBlock->_pNext;
            }

            HE_LOG_LINE(HE_STR_TEXT(""));
            HE_LOG_LINE(HE_STR_TEXT("usedCount: %d"), uUsedCount);
            HE_LOG_LINE(HE_STR_TEXT("usedTotal: 0x%8.8x"), uUsedTotal);

            if (pLargestUsedBlock != NULL)
            {
                Ptr pBlockAddr = reinterpret_cast<Ptr>(pLargestFreeBlock);
#ifdef HE_X64
                HE_LOG_LINE(HE_STR_TEXT("maxUsed: 0llx%8.8llx 0x%8.8x (0x%8.8x)"),
                            pBlockAddr + this->_GetMemoryBlockHeaderSize(),
                            pLargestUsedBlock->_uAllocateSize, pLargestUsedBlock->_uSize);
#else
                E_LOG_LINE(E_STR_TEXT("maxUsed: 0x%8.8x 0x%8.8x (0x%8.8x)"),
                           blockAddr + this->_GetMemoryBlockHeaderSize(),
                           pLargestUsedBlock->_allocateSize, pLargestUsedBlock->_size);
#endif
            }

            HE_LOG_LINE(HE_STR_TEXT("-------------------------------------------"));

            // 開き領域の表示
            BlockHeader* pFreeMemoryBlock = this->_aMemoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pFreeMemoryBlock != NULL)
            {
                if (pFreeMemoryBlock->_useFlag == 0)
                {
                    Ptr pBlockAddr = reinterpret_cast<Ptr>(pFreeMemoryBlock);
#ifdef HE_X64
                    HE_LOG_LINE(HE_STR_TEXT("0llx%8.8llx:0x%8.8x (0x%8.8x)"),
                                (pBlockAddr + this->_GetMemoryBlockHeaderSize()),
                                pFreeMemoryBlock->_uAllocateSize, pFreeMemoryBlock->_uSize);
#else
                    E_LOG_LINE(E_STR_TEXT("0x%8.8x:0x%8.8x (0x%8.8x)"),
                               (blockAddr + this->_GetMemoryBlockHeaderSize()),
                               pFreeMemoryBlock->_allocateSize, pFreeMemoryBlock->_size);
#endif
                    uFreeTotal += pFreeMemoryBlock->_uAllocateSize;
                    ++uFreeCount;

                    if (pLargestFreeBlock == NULL ||
                        pLargestFreeBlock->_uAllocateSize < pFreeMemoryBlock->_uAllocateSize)
                    {
                        pLargestFreeBlock = pFreeMemoryBlock;
                    }
                }

                pFreeMemoryBlock = pFreeMemoryBlock->_pNext;
            }

            HE_LOG_LINE(HE_STR_TEXT(""));
            HE_LOG_LINE(HE_STR_TEXT("freeCount: %d"), uFreeCount);
            HE_LOG_LINE(HE_STR_TEXT("freeTotal: 0x%8.8x"), uFreeTotal);

            if (pLargestFreeBlock != NULL)
            {
                HE_LOG_LINE(HE_STR_TEXT("maxFree: 0x%8.8x"), pLargestFreeBlock->_uAllocateSize);
            }

            HE_LOG_LINE(HE_STR_TEXT("============================================"));
        }

        /// <summary>
        /// 全メモリ情報表示
        /// </summary>
        void Manager::PrintAllMemoryInfo()
        {
            for (Uint8 i = 0; i < MemoryPageMax; ++i)
            {
                // 使用していないページは表示しない
                if (this->_ValidMemoryPage(i))
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
            BlockHeader* pMemoryBlock = this->_aMemoryPageInfoArray[in_page]._pMemoryBlockTop;

            while (pMemoryBlock != NULL)
            {
                if (this->_IsValidMemoryBlock(pMemoryBlock) == FALSE)
                {
                    HE_ASSERT(0 && "門番が死んでいる．");
                    return FALSE;
                }

                pMemoryBlock = pMemoryBlock->_pNext;
            }

            return TRUE;
        }

        /// <summary>
        /// 全ページのメモリブロックが正常かチェック
        /// </summary>
        /// <returns>TRUE 全てのページのメモリブロックは正常 / FALSE
        /// 異常なメモリブロックがある</returns>
        const Bool Manager::CheckAllMemoryBlock()
        {
            for (Uint8 i = 0; i < MemoryPageMax; ++i)
            {
                //	使用していないページはチェックしない．
                if (this->CheckMemoryBlockByPage(i))
                {
                    if (this->CheckMemoryBlockByPage(i) == FALSE) return FALSE;
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
                     FreeMemoryFillData,
                     in_pMemoryBlock->_uAllocateSize + in_pMemoryBlock->_paddingSize);
        }

        /// <summary>
        /// 指定メモリブロックを上書き用の固定値で塗りつぶす
        /// </summary>
        /// <param name="in_pMemoryBlock">塗りつぶすメモリブロック</param>
        void Manager::_FillNewMemoryBlock(BlockHeader* in_pMemoryBlock)
        {
            ::memset(reinterpret_cast<Uint8*>(in_pMemoryBlock) + this->_GetMemoryBlockHeaderSize(),
                     NewMemoryFillData,
                     in_pMemoryBlock->_uAllocateSize + in_pMemoryBlock->_paddingSize);
        }
#endif
    }  // namespace Memory
}  // namespace Core
