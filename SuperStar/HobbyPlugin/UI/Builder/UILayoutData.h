#pragma once

// UILayoutのデータファイルからUILayoutを構築するデータを生成
#include "Core/Common/FixMap.h"
#include "Core/Common/FixString.h"

// 依存モジュール
#include "HobbyPlugin/AssetManager/AssetDataBase.h"

namespace UI
{
    namespace Builder
    {
        enum EWidget
        {
            EWidget_None = 0,
            EWidget_Root,
            EWidget_Widget,
            EWidget_Label,
            EWidget_Button,
            EWidget_Layout,
        };

        struct Style
        {
            Float32 w, h;
            Uint32 color;
        };

        struct Node
        {
            // サードパーティのxmlライブラリのノードポインタ
            pugi::xml_node_struct* pNode = NULL;

            /// <summary>
            /// エンジン用のデータ
            /// </summary>
            struct Data
            {
                Char id[256]       = {0};
                EWidget widgetType = EWidget::EWidget_None;

                union ExData
                {
                    struct Widget
                    {
                        Float32 x, y;
                    } widget;

                    struct Label
                    {
                        Bool bLoc;
                        Float32 x, y;
                        Char loc[256];
                        Char text[256];
                        Style style;
                    } label;

                    struct Button
                    {
                        Float32 x, y;
                        Style style;
                    } button;

                    struct Layout
                    {
                        Style style;
                    } layout;
                } exData;
            } data;
        };

        /// <summary>
        /// UILayoutのデータファイルからLayoutを構築するデータを生成
        /// </summary>
        class UILayoutData final : public AssetManager::AssetDataXml
        {
        public:
            /// <summary>
            /// 親ルート下にあるノードを名前で取得
            /// </summary>
            /// <param name="in_pName"></param>
            /// <returns></returns>
            const Bool OutputNodeByRootPos(Node* out, const Byte* in_pName);

            /// <summary>
            /// 指定したノード下にあるノードを名前で取得
            /// </summary>
            /// <param name="in_rParentNode"></param>
            /// <param name="in_pName"></param>
            /// <returns></returns>
            const Bool OutputNode(Node* out, const Node& in_rParentNode, const Byte* in_pName);

            /// <summary>
            /// 指定したノード下にあるノード群を取得
            /// </summary>
            /// <param name="in_rParentNode"></param>
            /// <returns></returns>
            void OutputNodeChildren(Core::Common::FastFixArrayBase<Node>* out,
                                    const Node& in_rParentNode);
        };

    }  // namespace Builder
}  // namespace UI

#define UI_BUIILDER_NODE_CHILDREN_TYPE Core::Common::FixMap<Uint64, UI::Builder::Node, 64>
