#pragma once

// 連想配列クラス
// 任意の型を添え字にできる
// しかしその型には比較演算子< と >を実装していないといけない
// アルゴリズムはLLRB(赤黒木)を使っている
// メモリ確保を独自のに差し替えることができる
// キーの重複はだめ

#include "Core/Common/PoolManager.h"
#include "Core/Macro.h"
#include "Core/Type.h"

namespace Core
{
    namespace Common
    {
        // TODO: 基本クラスを作成して基本クラス型でわたせるようにする
        class BaseMap
        {
        public:
        };

        /// <summary>
        /// KEY添え字にしてDATAを探索するDATA固定長のMap
        /// 探索速度はO(log n)になる
        /// </summary>
        /// <typeparam name="KEY">連想配列の添え字の型</typeparam>
        /// <typeparam name="DATA">配列要素の型</typeparam>
        /// <typeparam name="SIZE">SIZEで指定した値が最大要素数,
        /// 最大要素数を超えたらエラーとなる</typeparam>
        template <typename KEY, typename DATA, Sint32 SIZE>
        class FixMap
        {
            E_CLASS_MOVE_CONSTRUCT_NG(FixMap);

        public:
            // 前方宣言
            struct NODE;

            // キーとデータのペア構造体
            struct Pair
            {
                KEY _key;
                DATA _data;
            };

            // カスタムマップのイテレーター
            // STL::Mapと同じように扱うために用意
            class Iterator
            {
            public:
                Iterator(NODE* pNode) : _pNode(pNode) {}

                inline const Bool IsValid() const E_NOEXCEPT { return this->_pNode != NULL; }

                // ペアポインタ参照
                Pair* operator->() { return &this->_pNode->_pair; }

                // ペアポインタ参照
                const Pair* operator->() const { return &this->_pNode->_pair; }

                // インクリメント
                Iterator& operator++()
                {
                    this->_pNode = this->_pNode->_pNext;
                    return *this;
                }

                // インクリメント
                Iterator operator++(int)
                {
                    Iterator it = *this;
                    ++(*this);
                    return it;
                }

                // デクリメント
                Iterator& operator--()
                {
                    this->_pNode = this->_pNode->_pPrev;
                    return *this;
                }

                // デクリメント
                Iterator operator--(int)
                {
                    Iterator it = *this;
                    --(*this);
                    return it;
                }

                // 比較
                bool operator==(const Iterator& rhs) const E_NOEXCEPT
                {
                    return this->_pNode == rhs._pNode;
                }

                // 比較
                bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

                // 代入
                const Iterator& operator=(const Iterator& rhs)
                {
                    this->_pNode = rhs._pNode;
                    return *this;
                }

            private:
                friend class FixMap;

                NODE* _pNode = NULL;
            };

        public:
            // コンストラクタ
            FixMap() : _itTail(&this->_tail) { this->_Init(); }

            // コピーのコンストラクタ
            // ディープコピーにする
            FixMap(const FixMap& other) : _itTail(&this->_tail)
            {
                this->_Init();
                this->_DeepCopy(&other);
            }

            // コピー処理
            // ディープコピーにする
            FixMap& operator=(FixMap& other)
            {
                this->_DeepCopy(&other);
                return *this;
            }

            FixMap& operator=(const FixMap& other)
            {
                this->_DeepCopy(&other);
                return *this;
            }

            // デストラクタ
            ~FixMap()
            {
                // ツリーを全て破棄する
                this->Clear();

#ifdef _HOBBY_ENGINE_DEBUG
                // メモリの確保/解放が正しく行われている
                E_ASSERT(this->_nodeNum == 0);

                // 線形リストの解除が正しく行われている
                E_ASSERT(this->_head._pPrev == NULL);
                E_ASSERT(this->_head._pNext == &this->_tail);
                E_ASSERT(this->_tail._pPrev == &this->_head);
                E_ASSERT(this->_tail._pNext == NULL);
#endif
            }

            /// <summary>
            /// キーとデータを追加
            /// </summary>
            /// <param name="rKey">キー指定</param>
            /// <param name="rData">キーと紐づいたデータ</param>
            /// <returns>失敗したら終端イテレータ / 追加成功なら追加データのイテレータ</returns>
            Iterator Add(const KEY& rKey, const DATA& rData) { return this->_Add(rKey, &rData); }

            /// <summary>
            /// キーからデータ検索
            /// </summary>
            /// <param name="rKey"></param>
            /// <returns>見つからない場合は終端イテレータを返す</returns>
            Iterator Find(const KEY& rKey)
            {
                // ツリーが空なら終端を返す
                if (this->Empty()) return this->End();

                NODE* pNode = this->_Find(this->_pRoot, rKey);
                if (pNode == NULL)
                {
                    // 見つからなかった
                    return this->End();
                }
                else
                {
                    // 見つかった
                    return Iterator(pNode);
                }
            }

            /// <summary>
            /// 指定キーの要素があるか
            /// </summary>
            /// <param name="rKey"></param>
            /// <returns></returns>
            const Bool Contains(const KEY& rKey)
            {
                // ツリーが空なのでキーの要素はない
                if (this->Empty()) return FALSE;

                NODE* pNode = this->_Find(this->_pRoot, rKey);
                if (pNode == NULL) return FALSE;

                return TRUE;
            }

            /// <summary>
            /// データ削除(キー版)
            /// </summary>
            /// <param name="in_rKey"></param>
            /// <returns></returns>
            const Bool Erase(const KEY& in_rKey)
            {
                // ツリーを辿って削除 &再構築
                this->_pRoot = this->_Erase(this->_pRoot, in_rKey);

                // まだツリーが存在するなら、ルートノードを黒にしておく
                if (this->_pRoot) this->_pRoot->_color = NODE::BLACK;

                return TRUE;
            }

            /// <summary>
            /// データ削除(イテレータ版)
            /// </summary>
            /// <param name="it"></param>
            /// <returns></returns>
            const Bool Erase(Iterator it)
            {
                // 終端ノードは消させない
                if (it == this->End()) return FALSE;

                // ツリーを辿って削除 &再構築
                this->_pRoot = this->_Erase(this->_pRoot, it._pNode->_pair._key);

                // まだツリーが存在するなら、ルートノードを黒にしておく
                if (this->_pRoot) this->_pRoot->_color = NODE::BLACK;

                return TRUE;
            }

            /// <summary>
            /// データを全て削除する
            /// </summary>
            void Clear()
            {
                // ルートから辿って破棄
                this->_Clear(this->_pRoot);
                this->_pRoot = NULL;
            }

            /// <summary>
            /// マップが空かどうか
            /// </summary>
            /// <returns>空(TRUE)</returns>
            const Bool Empty() const E_NOEXCEPT { return (this->_pRoot == NULL); }

            /// <summary>
            /// 要素数を返す
            /// </summary>
            const Uint32 Size() const E_NOEXCEPT { return this->_nodeNum; }

            /// <summary>
            /// 先頭イテレーターを取得
            /// データが空なら終端イテレーターを取得
            /// </summary>
            /// <returns></returns>
            Iterator Begin() const E_NOEXCEPT { return Iterator(this->_head._pNext); }

            /// <summary>
            /// 終端イテレーター取得
            /// </summary>
            /// <returns></returns>
            Iterator End() const E_NOEXCEPT { return this->_itTail; }

            /// <summary>
            /// KEYを添え字にしてデータアクセス
            /// KEYがなければそのKEYでデータを追加して参照を返す
            /// </summary>
            /// <param name="rKey"></param>
            /// <returns></returns>
            DATA& operator[](const KEY& rKey) { return this->FindorAdd(rKey); }

        protected:
            /// <summary>
            /// ノード作成
            /// </summary>
            /// <returns></returns>
            virtual NODE* _CreateNode()
            {
                Core::Common::Handle handle;
                NODE* pNode = this->_poolObject.Alloc(&handle);
                E_ASSERT(pNode != NULL);
                pNode->handle = handle;

                return pNode;
            }

            /// <summary>
            /// ノード破棄
            /// </summary>
            /// <param name="pNode"></param>
            virtual void _DestoryNode(const Core::Common::Handle& in_handle)
            {
                this->_poolObject.Free(in_handle);
            }

            inline DATA& FindorAdd(const KEY& rKey)
            {
                Iterator it = this->Find(rKey);
                if (it == this->End())
                {
                    // 無ければ追加する
                    // データ未確定なのでNULL
                    it = this->_Add(rKey, NULL);
                }

                // データの参照を返す
                return it->_data;
            }

#ifdef _HOBBY_ENGINE_DEBUG
        public:
            // ツリーの正当性チェック
            const Bool CheckValidByDebug(const Uint32 node_num)
            {
                Uint32 checked_count = 0;
                Bool bResult         = this->CheckNodeByDebug(_pRoot, &checked_count);
                if (bResult && node_num == checked_count) return TRUE;

                return FALSE;
            }

            // ノードの正当性チェック
            const Bool CheckNodeByDebug(NODE* pNode, Uint32* pCheckedCount)
            {
                // 終端なのでOK
                if (pNode == NULL) return TRUE;

                // 左ノードがあればコンペア
                if (pNode->_pLeft &&
                    this->_Compare(pNode->_pair._key, pNode->_pLeft->_pair._key) != -1)
                    return FALSE;

                // 右ノードがあればコンペア
                if (pNode->_pRight &&
                    this->_Compare(pNode->_pair._key, pNode->_pRight->_pair._key) != 1)
                    return FALSE;

                // 自分の左ノードをチェック
                if (pNode->_pLeft && this->CheckNodeByDebug(pNode->_pLeft, pCheckedCount) == FALSE)
                    return FALSE;

                // 自分の右ノードをチェック
                if (pNode->_pRight &&
                    this->CheckNodeByDebug(pNode->_pRight, pCheckedCount) == FALSE)
                    return FALSE;

                // 有効ノード数をカウント
                ++(*pCheckedCount);
                return TRUE;
            }
#endif

        protected:
            // 線形リストのノード
            struct NODE
            {
                // 赤黒木色定義
                enum COLOR
                {
                    RED,
                    BLACK
                };

                // ツリー用
                struct NODE* _pLeft  = NULL;
                struct NODE* _pRight = NULL;

                // 線形アクセス用
                struct NODE* _pPrev = NULL;
                struct NODE* _pNext = NULL;

                // 赤黒木カラー
                Uint8 _color = 0;

                // キーとデータのペア
                Pair _pair;

                Core::Common::Handle handle;
            };

            //! ノードメモリ確保と初期化
            NODE* _NewNode()
            {
                // ノードメモリ確保
                NODE* pNew = this->_CreateNode();
                E_ASSERT(pNew && "メモリが足りなくてノードが作成できない");

                ++this->_nodeNum;

                // 基本の初期化
                pNew->_pLeft  = NULL;
                pNew->_pRight = NULL;
                // 新しいノードは常に赤
                pNew->_color = NODE::RED;

                // 線形アクセス用にPrev/Nextを繋ぐ
                NODE* pTail           = this->_tail._pPrev;
                pTail->_pNext->_pPrev = pNew;
                pNew->_pNext          = pTail->_pNext;
                pTail->_pNext         = pNew;
                pNew->_pPrev          = pTail;

                return pNew;
            }

            // ノードメモリ破棄
            void _DeleteNode(NODE* pNode)
            {
                // 線形リストから解除
                pNode->_pNext->_pPrev = pNode->_pPrev;
                pNode->_pPrev->_pNext = pNode->_pNext;

#ifdef _HOBBY_ENGINE_DEBUG
                pNode->_pLeft  = NULL;
                pNode->_pNext  = NULL;
                pNode->_pPrev  = NULL;
                pNode->_pRight = NULL;
                pNode->_color  = 0;
#endif
                this->_DestoryNode(pNode->handle);
                --this->_nodeNum;
            }

            // ノードを追加する
            Iterator _Add(const KEY& rKey, const DATA* pData)
            {
                // 赤ノードを作る
                NODE* pNode = this->_NewNode();
                if (pNode == NULL) return this->End();

                pNode->_pair._key = rKey;
                if (pData != NULL)
                {
                    // 添え字アクセスで作る場合はデータが無い
                    // コピーして渡す
                    pNode->_pair._data = *pData;
                }

                // ルートを親として追加
                this->_pRoot = this->_Insert(this->_pRoot, pNode);
                // ルートは常に黒維持
                this->_pRoot->_color = NODE::BLACK;

                return Iterator(pNode);
            }

            /// <summary>
            /// ノードを末尾位置へ挿入
            /// 末尾ノードに挿入するので再帰処理で末尾ノードを探索
            /// </summary>
            /// <param name="pNode"></param>
            /// <param name="pAdd"></param>
            /// <returns></returns>
            NODE* _Insert(NODE* pNode, NODE* pAdd)
            {
                // 親がいないので, そこに追加する
                if (pNode == NULL) return pAdd;

                // キーの比較
                Sint32 cmp_result = this->_Compare(pNode->_pair._key, pAdd->_pair._key);

                // ノード挿入
                if (cmp_result == 0)
                {
                    E_ASSERT(FALSE && "重複したキーは登録出来ません");
                    return pNode;
                }
                else if (cmp_result < 0)
                {
                    pNode->_pLeft = this->_Insert(pNode->_pLeft, pAdd);
                }
                else
                {
                    pNode->_pRight = this->_Insert(pNode->_pRight, pAdd);
                }

                // 追加が終わったのでノードの赤と黒のバランスを取る
                // 赤と黒ノードの状態を見てノードを回転させたりノード色を変えることでバランスを取っている
                // 左が黒で右が赤
                if (this->_IsRed(pNode->_pRight) && (this->_IsRed(pNode->_pLeft) == FALSE))
                {
                    // LLRBでは右に赤を置かない
                    pNode = this->_RotateLeft(pNode);
                }

                // 左が赤で左の更に左が赤
                if (this->_IsRed(pNode->_pLeft) && this->_IsRed(pNode->_pLeft->_pLeft))
                {
                    // 左に赤2連続 → 右回転
                    pNode = this->_RotateRight(pNode);
                }

                // 左が赤で右も赤
                // 末尾ノード
                if (this->_IsRed(pNode->_pLeft) && this->_IsRed(pNode->_pRight))
                {
                    // 左右ともに赤
                    this->_FlipColors(pNode);
                }

                return pNode;
            }

            // キーの大小比較
            // クラスをキーにする場合、比較演算子( >, < )を用意してください。
            virtual Sint32 _Compare(KEY a, KEY b) const E_NOEXCEPT
            {
                if (a < b)
                {
                    // 右に付ける
                    return 1;
                }
                else if (a > b)
                {
                    // 左に付ける
                    return -1;
                }

                // 全く同じ内容
                return 0;
            }

            // キーに対応したノードを探す
            NODE* _Find(NODE* pNode, const KEY& rKey) const
            {
                // 見つからないまま終端
                if (pNode == NULL) return NULL;

                // 比較する
                Sint32 cmp_result = this->_Compare(pNode->_pair._key, rKey);
                if (cmp_result == 0)
                {
                    // 探しているノードだった
                    return pNode;
                }
                else if (cmp_result < 0)
                {
                    // 左へ探しに行く
                    return this->_Find(pNode->_pLeft, rKey);
                }
                else
                {
                    // 右へ探しに行く
                    return this->_Find(pNode->_pRight, rKey);
                }
            }

            // キーに対応したノードを探して削除
            NODE* _Erase(NODE* pNode, const KEY& rKey)
            {
                if (this->_Compare(pNode->_pair._key, rKey) < 0)
                {
                    // 削除ノードは左にある
                    if (!this->_IsRed(pNode->_pLeft) && !this->_IsRed(pNode->_pLeft->_pLeft))
                    {
                        pNode = this->_MoveRedLeft(pNode);
                    }
                    pNode->_pLeft = this->_Erase(pNode->_pLeft, rKey);
                }
                else
                {
                    // 削除ノードは自分自身か右にある
                    if (this->_IsRed(pNode->_pLeft))
                    {
                        pNode = this->_RotateRight(pNode);
                    }

                    if (this->_Compare(pNode->_pair._key, rKey) == 0 && (pNode->_pRight == NULL))
                    {
                        // NULLを返すことで、親からこのpNodeを外させる
                        this->_DeleteNode(pNode);
                        return NULL;
                    }

                    if (this->_IsRed(pNode->_pRight) && !this->_IsRed(pNode->_pRight->_pLeft))
                    {
                        pNode = this->_MoveRedRight(pNode);
                    }

                    if (this->_Compare(pNode->_pair._key, rKey) == 0)
                    {
                        // このノードを削除したいが、左右に別のノードがくっついている
                        // 自分の値に一番近いノードを探す
                        NODE* pMinNode    = this->_FindMinNode(pNode->_pRight);
                        pMinNode->_pRight = this->_RemoveMinNode(pNode->_pRight);
                        pMinNode->_pLeft  = pNode->_pLeft;
                        pMinNode->_color  = pNode->_color;
                        pNode             = pMinNode;
                    }
                    else
                    {
                        pNode->_pRight = this->_Erase(pNode->_pRight, rKey);
                    }
                }

                return this->_Fixup(pNode);
            }

            // ノードを削除
            void _Clear(NODE* pNode)
            {
                if (pNode == NULL) return;

                // 左右のノードを辿る
                this->_Clear(pNode->_pLeft);
                this->_Clear(pNode->_pRight);

                // 自分自身を削除する
                this->_DeleteNode(pNode);
            }

            /// <summary>
            /// ディープコピー処理
            /// </summary>
            /// <param name="other"></param>
            void _DeepCopy(const FixMap* in_pOther)
            {
                E_ASSERT(in_pOther);
                for (const NODE* p                              = in_pOther->_head._pNext;
                     (p != &in_pOther->_tail) && (p != NULL); p = p->_pNext)
                {
                    this->Add(p->_pair._key, p->_pair._data);
                }
            }

            // ツリーの左回転
            NODE* _RotateLeft(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                NODE* pNewParent   = in_pNode->_pRight;
                in_pNode->_pRight  = pNewParent->_pLeft;
                pNewParent->_pLeft = in_pNode;

                pNewParent->_color = in_pNode->_color;
                in_pNode->_color   = NODE::RED;

                return pNewParent;
            }

            // ツリーの右回転
            NODE* _RotateRight(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                NODE* pNewParent    = in_pNode->_pLeft;
                in_pNode->_pLeft    = pNewParent->_pRight;
                pNewParent->_pRight = in_pNode;

                pNewParent->_color = in_pNode->_color;
                in_pNode->_color   = NODE::RED;

                return pNewParent;
            }

            // 自分とその子供の色の反転
            void _FlipColors(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                // 0と1を切り替え
                in_pNode->_color ^= 1;
                in_pNode->_pLeft->_color ^= 1;
                in_pNode->_pRight->_color ^= 1;
            }

            // 左に赤ノードを移動
            NODE* _MoveRedLeft(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                this->_FlipColors(in_pNode);

                if (this->_IsRed(in_pNode->_pRight->_pLeft))
                {
                    in_pNode->_pRight = this->_RotateRight(in_pNode->_pRight);
                    in_pNode          = this->_RotateLeft(in_pNode);
                    this->_FlipColors(in_pNode);
                }

                return in_pNode;
            }

            // 右に赤ノードを移動
            NODE* _MoveRedRight(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                this->_FlipColors(in_pNode);

                if (this->_IsRed(in_pNode->_pLeft->_pLeft))
                {
                    in_pNode = this->_RotateRight(in_pNode);
                    this->_FlipColors(in_pNode);
                }

                return in_pNode;
            }

            // 最も小さいキーとなるノードを探す
            NODE* _FindMinNode(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                if (in_pNode->_pLeft == NULL)
                {
                    return in_pNode;
                }
                else
                {
                    return this->_FindMinNode(in_pNode->_pLeft);
                }
            }

            // 最も小さいキーとなるノードを外す
            NODE* _RemoveMinNode(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                if (in_pNode->_pLeft == NULL) return NULL;

                // 左ノードが黒なら赤に変える
                if ((this->_IsRed(in_pNode->_pLeft) == FALSE) &&
                    (this->_IsRed(in_pNode->_pLeft->_pLeft) == FALSE))
                {
                    in_pNode = this->_MoveRedLeft(in_pNode);
                }

                in_pNode->_pLeft = this->_RemoveMinNode(in_pNode->_pLeft);
                return this->_Fixup(in_pNode);
            }

            // ノードの平衡化
            NODE* _Fixup(NODE* in_pNode)
            {
                E_ASSERT(in_pNode);

                if (this->_IsRed(in_pNode->_pRight))
                {
                    in_pNode = this->_RotateLeft(in_pNode);
                }

                E_ASSERT(in_pNode);
                if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pLeft->_pLeft))
                {
                    in_pNode = this->_RotateRight(in_pNode);
                }

                E_ASSERT(in_pNode);
                if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pRight))
                {
                    this->_FlipColors(in_pNode);
                }

                return in_pNode;
            }

            // ノードが赤かどうか
            const Bool _IsRed(NODE* in_pNode)
            {
                if (in_pNode) return (in_pNode->_color == NODE::RED);

                // 成立しないのでFALSEを返す
                return FALSE;
            }

            inline void _Init() E_NOEXCEPT
            {
                // 線形アクセス用のリストを初期化
                this->_pRoot       = NULL;
                this->_head._pPrev = NULL;
                this->_head._pNext = &this->_tail;
                this->_tail._pPrev = &this->_head;
                this->_tail._pNext = NULL;
            }

        private:
            NODE* _pRoot = NULL;

            // 線形アクセス用
            NODE _head;
            NODE _tail;

            Iterator _itTail;

            Uint32 _nodeNum = 0;
            Core::Common::FixPoolManager<NODE, SIZE> _poolObject;
        };
    }  // namespace Common
}  // namespace Core
