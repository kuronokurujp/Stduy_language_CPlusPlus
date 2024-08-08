#pragma once

// UILayoutのデータファイルからUILayoutを構築するデータを生成
#include "Engine/Common/CustomMap.h"
#include "Engine/Common/CustomStack.h"
#include "Engine/Common/CustomString.h"

// 依存モジュール
#include "AssetManagerModule.h"

namespace UI::Builder
{
    enum EAnchor
    {
        EAnchor_Left = 0,
        EPosAnchor_Center,
        EPosAnchor_Max
    };

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
            Char szId[64]       = {0};
            EWidget eWidgetType = EWidget::EWidget_None;

            union ExData
            {
                struct Widget
                {
                    Float32 fX, fY;
                } widget;

                struct Label
                {
                    EAnchor eAnchor;
                    Bool bLoc;
                    Float32 fX, fY;
                    Char szLoc[64];
                    Char szText[64];
                    Style style;
                } label;

                struct Button
                {
                    EAnchor eAnchor;
                    Float32 fX, fY;
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
        const Bool OutputNodeByRootPos(Node* out, const UTF8* in_szName);

        /// <summary>
        /// 指定したノード下にあるノードを名前で取得
        /// </summary>
        const Bool OutputNode(Node* out, const Node& in_rParentNode, const UTF8* in_szName);

        /// <summary>
        /// 指定したノード下にあるノード群を取得
        /// </summary>
        void OutputNodeChildren(Core::Common::StackBase<Node>* out, const Node& in_rParentNode);
    };

}  // namespace UI::Builder

#define UI_BUIILDER_NODE_CHILDREN_TYPE Core::Common::FixMap<Uint64, UI::Builder::Node, 64>
