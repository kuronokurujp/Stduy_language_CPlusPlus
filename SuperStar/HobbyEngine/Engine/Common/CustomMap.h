#pragma once

// 連想配列クラス
// 任意の型を添え字にできる
// しかしその型には比較演算子< と >を実装していないといけない
// アルゴリズムはLLRB(赤黒木)を使っている
// キーの重複はだめ

#include "Engine/Common/CustomStack.h"
#include "Engine/Common/CustomString.h"
#include "Engine/Common/CustomVector.h"
#include "Engine/Common/PoolManager.h"
#include "Engine/Macro.h"
#include "Engine/Type.h"

namespace Core::Common
{
    /// <summary>
    /// KEY添え字にしてDATAを探索するDATA固定長のMap
    /// 探索速度はO(log n)になる
    /// </summary>
    /// <typeparam name="KEY">連想配列の添え字の型</typeparam>
    /// <typeparam name="DATA">配列要素の型</typeparam>
    /// <typeparam name="SIZE">SIZEで指定した値が最大要素数,
    /// 最大要素数を超えたらエラーとなる</typeparam>
    template <typename KEY, typename DATA, Sint32 SIZE>
    class CustomFixMap final
    {
        HE_CLASS_MOVE_NG(CustomFixMap);

    public:
        // 前方宣言
        struct Node;

        // キーとデータのペア構造体
        struct Pair
        {
            KEY key;
            DATA data;
        };

        // カスタムマップのイテレーター
        // STL::Mapと同じように扱うために用意
        class Iterator final
        {
        public:
            Iterator(Node* in_pNode) : _pNode(in_pNode) {}

            inline const Bool IsValid() const HE_NOEXCEPT { return this->_pNode != NULL; }

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
                Iterator iter = *this;
                ++(*this);
                return iter;
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
                Iterator iter = *this;
                --(*this);
                return iter;
            }

            // 比較
            bool operator==(const Iterator& in_rIter) const HE_NOEXCEPT
            {
                return this->_pNode == in_rIter._pNode;
            }

            // 比較
            bool operator!=(const Iterator& in_crIter) const { return !(*this == in_crIter); }

            // 代入
            const Iterator& operator=(const Iterator& in_rIter)
            {
                this->_pNode = in_rIter._pNode;
                return *this;
            }

        private:
            friend class CustomFixMap;

            Node* _pNode = NULL;
        };

    public:
        // コンストラクタ
        CustomFixMap() : _iteratorTail(&this->_tail) { this->_Init(); }

        // コピーのコンストラクタ
        // ディープコピーにする
        CustomFixMap(const CustomFixMap& in_mrOther) : _iteratorTail(&this->_tail)
        {
            this->_Init();
            this->_DeepCopy(&in_mrOther);
        }

        // コンストラクタ (initializer_listを受け取る)
        CustomFixMap(const std::initializer_list<std::pair<KEY, DATA>>& in_rInitList)
            : _iteratorTail(&this->_tail)
        {
            this->_Init();
            for (const auto& item : in_rInitList)
            {
                this->Add(item.first, item.second);
            }
        }

        // コピー処理
        // ディープコピーにする
        CustomFixMap& operator=(CustomFixMap& in_mrOther)
        {
            this->_DeepCopy(&in_mrOther);
            return *this;
        }

        CustomFixMap& operator=(const CustomFixMap& in_mrOther)
        {
            this->_DeepCopy(&in_mrOther);
            return *this;
        }

        // デストラクタ
        ~CustomFixMap()
        {
            // ツリーを全て破棄する
            this->Clear();

#ifdef HE_ENGINE_DEBUG
            // メモリの確保/解放が正しく行われている
            HE_ASSERT(this->_uNodeNum == 0);

            // 線形リストの解除が正しく行われている
            HE_ASSERT(this->_head._pPrev == NULL);
            HE_ASSERT(this->_head._pNext == &this->_tail);
            HE_ASSERT(this->_tail._pPrev == &this->_head);
            HE_ASSERT(this->_tail._pNext == NULL);
#endif
        }

        /// <summary>
        /// キーとデータを追加
        /// </summary>
        Iterator Add(const KEY& in_rKey, const DATA& in_rData)
        {
            return this->_Add(in_rKey, &in_rData);
        }

        /// <summary>
        /// キーからデータ検索
        /// </summary>
        Iterator FindKey(const KEY& in_trKey) const
        {
            // ツリーが空なら終端を返す
            if (this->Empty()) return this->End();

            Node* tpNode = this->_FindKey(this->_pRoot, in_trKey);
            if (tpNode == NULL)
            {
                // 見つからなかった
                return this->End();
            }
            else
            {
                // 見つかった
                return Iterator(tpNode);
            }
        }

        /// <summary>
        /// データからキー検索
        /// </summary>
        Iterator FindData(const DATA& in_rData)
        {
            // ツリーが空なら終端を返す
            if (this->Empty()) return this->End();

            Node* tpNode = this->_FindData(this->_pRoot, in_rData);
            if (tpNode == NULL)
            {
                // 見つからなかった
                return this->End();
            }
            else
            {
                // 見つかった
                return Iterator(tpNode);
            }
        }

        /// <summary>
        /// 指定キーの要素があるか
        /// </summary>
        Bool Contains(const KEY& in_rKey) const
        {
            // ツリーが空なのでキーの要素はない
            if (this->Empty()) return FALSE;

            Node* tpNode = this->_FindKey(this->_pRoot, in_rKey);
            if (tpNode == NULL) return FALSE;

            return TRUE;
        }

        /// <summary>
        /// データ削除(キー版)
        /// </summary>
        Bool Erase(const KEY& in_rKey)
        {
            if (this->Contains(in_rKey) == FALSE) return FALSE;

            // ツリーを辿って削除と再構築
            this->_pRoot = this->_Erase(this->_pRoot, in_rKey);

            // まだツリーが存在するなら、ルートノードを黒にしておく
            if (this->_pRoot) this->_pRoot->_uColor = Node::BLACK;

            return TRUE;
        }

        /// <summary>
        /// データ削除(イテレータ版)
        /// </summary>
        Bool Erase(Iterator in_iter)
        {
            // 終端ノードは消させない
            if (in_iter == this->End()) return FALSE;

            // ツリーを辿って削除 &再構築
            this->_pRoot = this->_Erase(this->_pRoot, in_iter._pNode->_pair.key);

            // まだツリーが存在するなら、ルートノードを黒にしておく
            if (this->_pRoot) this->_pRoot->_uColor = Node::BLACK;

            return TRUE;
        }

        /// <summary>
        /// データを全て削除する
        /// </summary>
        void Clear()
        {
            // ルートから辿って破棄
            this->_Clear(this->_pRoot);
            this->_Init();
        }

        /// <summary>
        /// マップが空かどうか
        /// </summary>
        Bool Empty() const HE_NOEXCEPT { return (this->_pRoot == NULL); }

        /// <summary>
        /// 要素数を返す
        /// </summary>
        Uint32 Size() const HE_NOEXCEPT { return this->_uNodeNum; }

        /// <summary>
        /// 先頭イテレーターを取得
        /// データが空なら終端イテレーターを取得
        /// </summary>
        Iterator Begin() const HE_NOEXCEPT { return Iterator(this->_head._pNext); }

        /// <summary>
        /// 終端イテレーター取得
        /// </summary>
        Iterator End() const HE_NOEXCEPT { return this->_iteratorTail; }

        /// <summary>
        /// KEYを添え字にしてデータアクセス
        /// KEYがなければそのKEYでデータを追加して参照を返す
        /// </summary>
        DATA& operator[](const KEY& in_trKey) { return this->FindOrAddKey(in_trKey); }

    protected:
        /// <summary>
        /// ノード作成
        /// </summary>
        virtual Node* _VCreateNode()
        {
            Core::Common::Handle handle;
            Node* pNode = this->_poolObject.Alloc(&handle);
            HE_ASSERT(pNode != NULL);
            pNode->handle = handle;

            return pNode;
        }

        /// <summary>
        /// ノード破棄
        /// </summary>
        virtual void _VDestoryNode(const Core::Common::Handle& in_rHandle)
        {
            // DATA型で破棄する時にクラスのプロパティをクリアするためにデストラクタを呼ぶ
            // 対象クラスは汎用データ構造を持つクラスに限定
            if constexpr (IsCustomFixVector<DATA>::value || IsCustomFixStack<DATA>::value ||
                          IsCustomFixString<DATA>::value)
            {
                // デストラクタを呼ぶ
                Node* p = this->_poolObject.Ref(in_rHandle);
                p->_pair.data.~DATA();
            }

            this->_poolObject.Free(in_rHandle);
        }

        inline DATA& FindOrAddKey(const KEY& in_trKey)
        {
            Iterator it = this->FindKey(in_trKey);
            if (it == this->End())
            {
                // 無ければ追加する
                // データ未確定なのでNULL
                it = this->_Add(in_trKey, NULL);
            }

            // データの参照を返す
            return it->data;
        }

#ifdef HE_ENGINE_DEBUG
    public:
        // ツリーの正当性チェック
        Bool CheckValidByDebug(const Uint32 in_uNodeNum)
        {
            Uint32 uCheckCount = 0;
            Bool bResult       = this->CheckNodeByDebug(&uCheckCount, _pRoot);
            if (bResult && in_uNodeNum == uCheckCount) return TRUE;

            return FALSE;
        }

        // ノードの正当性チェック
        Bool CheckNodeByDebug(Uint32* out, Node* in_pNode)
        {
            // 終端なのでOK
            if (in_pNode == NULL) return TRUE;

            // 左ノードがあればコンペア
            if (in_pNode->_pLeft &&
                this->_CompareByKey(in_pNode->_pair.key, in_pNode->_pLeft->_pair.key) != -1)
                return FALSE;

            // 右ノードがあればコンペア
            if (in_pNode->_pRight &&
                this->_CompareByKey(in_pNode->_pair.key, in_pNode->_pRight->_pair.key) != 1)
                return FALSE;

            // 自分の左ノードをチェック
            if (in_pNode->_pLeft && this->CheckNodeByDebug(out, in_pNode->_pLeft) == FALSE)
                return FALSE;

            // 自分の右ノードをチェック
            if (in_pNode->_pRight && this->CheckNodeByDebug(out, in_pNode->_pRight) == FALSE)
                return FALSE;

            // 有効ノード数をカウント
            ++(*out);
            return TRUE;
        }
#endif

    protected:
        // 線形リストのノード
        struct Node
        {
            // 赤黒木色定義
            enum COLOR
            {
                RED,
                BLACK
            };

            // ツリー用
            struct Node* _pLeft  = NULL;
            struct Node* _pRight = NULL;

            // 線形アクセス用
            struct Node* _pPrev = NULL;
            struct Node* _pNext = NULL;

            // 赤黒木カラー
            Uint32 _uColor = 0;

            // キーとデータのペア
            Pair _pair;

            Core::Common::Handle handle;
        };

        //! ノードメモリ確保と初期化
        Node* _NewNode()
        {
            // ノードメモリ確保
            Node* pNewNode = this->_VCreateNode();
            HE_ASSERT(pNewNode && "メモリが足りなくてノードが作成できない");

            ++this->_uNodeNum;

            // 基本の初期化
            pNewNode->_pLeft  = NULL;
            pNewNode->_pRight = NULL;
            // 新しいノードは常に赤
            pNewNode->_uColor = Node::RED;

            // 線形アクセス用にPrev/Nextを繋ぐ
            Node* pTailNode           = this->_tail._pPrev;
            pTailNode->_pNext->_pPrev = pNewNode;
            pNewNode->_pNext          = pTailNode->_pNext;
            pTailNode->_pNext         = pNewNode;
            pNewNode->_pPrev          = pTailNode;

            return pNewNode;
        }

        // ノードメモリ破棄
        void _DeleteNode(Node* in_pNode)
        {
            // 線形リストから解除
            in_pNode->_pNext->_pPrev = in_pNode->_pPrev;
            in_pNode->_pPrev->_pNext = in_pNode->_pNext;

#ifdef HE_ENGINE_DEBUG
            in_pNode->_pLeft  = NULL;
            in_pNode->_pNext  = NULL;
            in_pNode->_pPrev  = NULL;
            in_pNode->_pRight = NULL;
            in_pNode->_uColor = 0;
#endif
            this->_VDestoryNode(in_pNode->handle);
            --this->_uNodeNum;
        }

        // ノードを追加する
        Iterator _Add(const KEY& in_rKey, const DATA* in_pData)
        {
            // 赤ノードを作る
            Node* pNode = this->_NewNode();
            if (pNode == NULL) return this->End();

            pNode->_pair.key = in_rKey;
            if (in_pData != NULL)
            {
                // 添え字アクセスで作る場合はデータが無い
                // コピーして渡す
                pNode->_pair.data = *in_pData;
            }

            // ルートを親として追加
            this->_pRoot = this->_Insert(this->_pRoot, pNode);
            // ルートは常に黒維持
            this->_pRoot->_uColor = Node::BLACK;

            return Iterator(pNode);
        }

        /// <summary>
        /// ノードを末尾位置へ挿入
        /// 末尾ノードに挿入するので再帰処理で末尾ノードを探索
        /// </summary>
        Node* _Insert(Node* in_pNode, Node* in_pAdd)
        {
            // 親がいないので, そこに追加する
            if (in_pNode == NULL) return in_pAdd;

            // キーの比較
            Sint32 iCmpResult = this->_CompareByKey(in_pNode->_pair.key, in_pAdd->_pair.key);

            // ノード挿入
            if (iCmpResult == 0)
            {
                HE_ASSERT(FALSE && "重複したキーは登録出来ません");
                return in_pNode;
            }
            else if (iCmpResult < 0)
            {
                in_pNode->_pLeft = this->_Insert(in_pNode->_pLeft, in_pAdd);
            }
            else
            {
                in_pNode->_pRight = this->_Insert(in_pNode->_pRight, in_pAdd);
            }

            // 追加が終わったのでノードの赤と黒のバランスを取る
            // 赤と黒ノードの状態を見てノードを回転させたりノード色を変えることでバランスを取っている
            // 左が黒で右が赤
            if (this->_IsRed(in_pNode->_pRight) && (this->_IsRed(in_pNode->_pLeft) == FALSE))
            {
                // LLRBでは右に赤を置かない
                in_pNode = this->_RotateLeft(in_pNode);
            }

            // 左が赤で左の更に左が赤
            if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pLeft->_pLeft))
            {
                // 左に赤2連続 → 右回転
                in_pNode = this->_RotateRight(in_pNode);
            }

            // 左が赤で右も赤
            // 末尾ノード
            if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pRight))
            {
                // 左右ともに赤
                this->_FlipColors(in_pNode);
            }

            return in_pNode;
        }

        // キーの大小比較
        // クラスをキーにする場合、比較演算子( >, < )を用意してください。
        Sint32 _CompareByKey(const KEY& in_rLeft, const KEY& in_rRight) const HE_NOEXCEPT
        {
            if (in_rLeft < in_rRight)
            {
                // 右に付ける
                return 1;
            }
            else if (in_rLeft > in_rRight)
            {
                // 左に付ける
                return -1;
            }

            // 全く同じ内容
            return 0;
        }

        // データの大小比較
        // クラスをデータにする場合、比較演算子( >, < )を用意してください。
        Sint32 _CompareByData(const DATA& in_rLeft, const DATA& in_rRight) const HE_NOEXCEPT
        {
            // TODO:
            // 比較演算しを対応しているかどうかチェックして対応していない場合はスキップできるか？
            if (in_rLeft < in_rRight)
            {
                // 右に付ける
                return 1;
            }
            else if (in_rLeft > in_rRight)
            {
                // 左に付ける
                return -1;
            }

            // 全く同じ内容
            return 0;
        }

        // キーに対応したノードを探す
        Node* _FindKey(Node* in_pNode, const KEY& in_rKey) const
        {
            // 見つからないまま終端
            if (in_pNode == NULL) return NULL;

            // 比較する
            Sint32 iCmpResult = this->_CompareByKey(in_pNode->_pair.key, in_rKey);
            if (iCmpResult == 0)
            {
                // 探しているノードだった
                return in_pNode;
            }
            else if (iCmpResult < 0)
            {
                // 左へ探しに行く
                return this->_FindKey(in_pNode->_pLeft, in_rKey);
            }
            else
            {
                // 右へ探しに行く
                return this->_FindKey(in_pNode->_pRight, in_rKey);
            }
        }

        // データに対応したノードを探す
        Node* _FindData(Node* in_pNode, const DATA& in_rData) const
        {
            // 見つからないまま終端
            if (in_pNode == NULL) return NULL;

            // 比較する
            Sint32 iCmpResult = this->_CompareByData(in_pNode->_pair.data, in_rData);
            if (iCmpResult == 0)
            {
                // 探しているノードだった
                return in_pNode;
            }
            else if (iCmpResult < 0)
            {
                // 左へ探しに行く
                return this->_FindData(in_pNode->_pLeft, in_rData);
            }
            else
            {
                // 右へ探しに行く
                return this->_FindData(in_pNode->_pRight, in_rData);
            }
        }

        // キーに対応したノードを探して削除
        Node* _Erase(Node* in_pNode, const KEY& in_trKey)
        {
            if (this->_CompareByKey(in_pNode->_pair.key, in_trKey) < 0)
            {
                // 削除ノードは左にある
                if (!this->_IsRed(in_pNode->_pLeft) && !this->_IsRed(in_pNode->_pLeft->_pLeft))
                {
                    in_pNode = this->_MoveRedLeft(in_pNode);
                }
                in_pNode->_pLeft = this->_Erase(in_pNode->_pLeft, in_trKey);
            }
            else
            {
                // 削除ノードは自分自身か右にある
                if (this->_IsRed(in_pNode->_pLeft))
                {
                    in_pNode = this->_RotateRight(in_pNode);
                }

                if (this->_CompareByKey(in_pNode->_pair.key, in_trKey) == 0 &&
                    (in_pNode->_pRight == NULL))
                {
                    // NULLを返すことで、親からこのpNodeを外させる
                    this->_DeleteNode(in_pNode);
                    return NULL;
                }

                if (this->_IsRed(in_pNode->_pRight) && !this->_IsRed(in_pNode->_pRight->_pLeft))
                {
                    in_pNode = this->_MoveRedRight(in_pNode);
                }

                if (this->_CompareByKey(in_pNode->_pair.key, in_trKey) == 0)
                {
                    // このノードを削除したいが、左右に別のノードがくっついている
                    // 自分の値に一番近いノードを探す
                    Node* tpMinNode    = this->_FindMinNode(in_pNode->_pRight);
                    tpMinNode->_pRight = this->_RemoveMinNode(in_pNode->_pRight);
                    tpMinNode->_pLeft  = in_pNode->_pLeft;
                    tpMinNode->_uColor = in_pNode->_uColor;
                    // ノードを削除
                    {
                        this->_DeleteNode(in_pNode);
                    }

                    in_pNode = tpMinNode;
                }
                else
                {
                    in_pNode->_pRight = this->_Erase(in_pNode->_pRight, in_trKey);
                }
            }

            return this->_Fixup(in_pNode);
        }

        // ノードを削除
        void _Clear(Node* in_pNode)
        {
            if (in_pNode == NULL) return;

            // 左右のノードを辿る
            this->_Clear(in_pNode->_pLeft);
            this->_Clear(in_pNode->_pRight);

            // 自分自身を削除する
            this->_DeleteNode(in_pNode);
        }

        /// <summary>
        /// ディープコピー処理
        /// </summary>
        void _DeepCopy(const CustomFixMap* in_mpOther)
        {
            HE_ASSERT(in_mpOther);
            for (const Node* p = in_mpOther->_head._pNext; (p != &in_mpOther->_tail) && (p != NULL);
                 p             = p->_pNext)
            {
                this->Add(p->_pair.key, p->_pair.data);
            }
        }

        // ツリーの左回転
        Node* _RotateLeft(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            Node* tpNewParent   = in_pNode->_pRight;
            in_pNode->_pRight   = tpNewParent->_pLeft;
            tpNewParent->_pLeft = in_pNode;

            tpNewParent->_uColor = in_pNode->_uColor;
            in_pNode->_uColor    = Node::RED;

            return tpNewParent;
        }

        // ツリーの右回転
        Node* _RotateRight(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            Node* pNewParent    = in_pNode->_pLeft;
            in_pNode->_pLeft    = pNewParent->_pRight;
            pNewParent->_pRight = in_pNode;

            pNewParent->_uColor = in_pNode->_uColor;
            in_pNode->_uColor   = Node::RED;

            return pNewParent;
        }

        // 自分とその子供の色の反転
        void _FlipColors(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            // 0と1を切り替え
            in_pNode->_uColor ^= 1;
            if (in_pNode->_pLeft != NULL) in_pNode->_pLeft->_uColor ^= 1;
            if (in_pNode->_pRight != NULL) in_pNode->_pRight->_uColor ^= 1;
        }

        // 左に赤ノードを移動
        Node* _MoveRedLeft(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            // 右のノードを左に移動したいが,
            // 右のノードが存在しない場合もあるので移動できない
            if (in_pNode->_pRight == NULL) return in_pNode;

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
        Node* _MoveRedRight(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            this->_FlipColors(in_pNode);

            if (in_pNode->_pLeft && this->_IsRed(in_pNode->_pLeft->_pLeft))
            {
                in_pNode = this->_RotateRight(in_pNode);
                this->_FlipColors(in_pNode);
            }

            return in_pNode;
        }

        // 最も小さいキーとなるノードを探す
        Node* _FindMinNode(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

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
        Node* _RemoveMinNode(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

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
        Node* _Fixup(Node* in_pNode)
        {
            HE_ASSERT(in_pNode);

            if (this->_IsRed(in_pNode->_pRight))
            {
                in_pNode = this->_RotateLeft(in_pNode);
            }

            HE_ASSERT(in_pNode);
            if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pLeft->_pLeft))
            {
                in_pNode = this->_RotateRight(in_pNode);
            }

            HE_ASSERT(in_pNode);
            if (this->_IsRed(in_pNode->_pLeft) && this->_IsRed(in_pNode->_pRight))
            {
                this->_FlipColors(in_pNode);
            }

            return in_pNode;
        }

        // ノードが赤かどうか
        Bool _IsRed(Node* in_pNode)
        {
            if (in_pNode) return (in_pNode->_uColor == Node::RED);

            // 成立しないのでFALSEを返す
            return FALSE;
        }

        inline void _Init() HE_NOEXCEPT
        {
            // 線形アクセス用のリストを初期化
            this->_pRoot       = NULL;
            this->_head._pPrev = NULL;
            this->_head._pNext = &this->_tail;
            this->_tail._pPrev = &this->_head;
            this->_tail._pNext = NULL;
        }

    private:
        Node* _pRoot = NULL;

        // 線形アクセス用
        Node _head;
        Node _tail;

        Iterator _iteratorTail;

        Uint32 _uNodeNum = 0;
        Core::Common::FixPoolManager<Node, SIZE> _poolObject;
    };
}  // namespace Core::Common
