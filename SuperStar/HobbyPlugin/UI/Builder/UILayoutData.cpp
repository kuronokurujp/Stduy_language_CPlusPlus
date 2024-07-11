#include "UILayoutData.h"

#include <string.h>

// 依存モジュール
#include "HobbyPlugin/Render/Color.h"

namespace UI
{
    namespace Builder
    {
        /// <summary>
        /// ローカル内のみ使用
        /// </summary>
        namespace Local
        {
            static const Char s_treeNodeDelimita = E_STR_TEXT('.');
            static const Char s_locGroupDelimita = E_STR_TEXT('.');
            /// <summary>
            /// UIのスタイルカラー
            /// </summary>
            struct StyleColor
            {
            public:
                StyleColor(const Byte* in_pName, Render::Color in_rgba)
                {
                    this->pName = in_pName;
                    this->rgba  = in_rgba;
                }

                const Byte* pName = NULL;
                Render::Color rgba;
            };

            /// <summary>
            /// UIデータの色名とカラーと対応したテーブル一覧
            /// </summary>
            StyleColor s_aColorTable[] = {StyleColor("white", Render::RGB::White),
                                          StyleColor("black", Render::RGB::Black),
                                          StyleColor("red", Render::RGB::Red),
                                          StyleColor("blue", Render::RGB::Blue),
                                          StyleColor("green", Render::RGB::Green),
                                          StyleColor("yellow", Render::RGB::Yellow),
                                          StyleColor("cyan", Render::RGB::Cyan),
                                          StyleColor("magenta", Render::RGB::Magenta),
                                          StyleColor("ornage", Render::RGB::Orange)};

            static void _ParseStyle(Style* s, const Byte* in_pStyle, const Uint32 in_styleSize)
            {
                static Byte buff[1024] = {0};

                E_ASSERT(in_styleSize < E_ARRAY_SIZE(buff));
                ::memset(s, 0, sizeof(Style));

                ::memcpy_s(buff, E_ARRAY_SIZE(buff), in_pStyle, in_styleSize);
                Byte* pRestartToken = NULL;
                Byte* token         = ::strtok_s(buff, ";", &pRestartToken);

                while (token != NULL)
                {
                    Byte* key   = ::strtok(token, ":");
                    Byte* value = ::strtok(NULL, ":");

                    if (key && value)
                    {
                        while (*key == ' ') ++key;
                        while (*value == ' ') ++value;

                        if (::strcmp(key, "w") == 0)
                        {
                            s->w = static_cast<Float32>(::atof(value));
                        }
                        else if (::strcmp(key, "h") == 0)
                        {
                            s->h = static_cast<Float32>(::atoi(value));
                        }
                        else if (::strcmp(key, "color") == 0)
                        {
                            Byte aColorName[128] = {0};
                            ::strncpy_s(aColorName, E_ARRAY_NUM(aColorName), value,
                                        E_ARRAY_NUM(aColorName));
                            // TODO: キーワードと色名の対応テーブルを作る
                            for (Uint32 i = 0; i < E_ARRAY_NUM(s_aColorTable); ++i)
                            {
                                if (::strcmp(s_aColorTable[i].pName, aColorName) == 0)
                                {
                                    s->color = s_aColorTable[i].rgba.c;
                                    break;
                                }
                            }
                        }
                    }

                    token = ::strtok_s(NULL, ";", &pRestartToken);
                }
            }

            static void ApplyNode(Node* out, const pugi::xml_node& in_rNode)
            {
                E_ASSERT(out);
                ::memset(out, 0, sizeof(Node));

                // サードパーティライブラリのノードポインタを保存
                out->pNode = in_rNode.internal_object();

                Node::Data* pData = &out->data;

                // TODO: ノードから反映する情報を抜き出す
                pData->widgetType = UI::Builder::EWidget_None;

                Core::Common::FixString128 attrName = in_rNode.name();
                if (attrName == E_STR_TEXT("ui"))
                {
                    pData->widgetType = UI::Builder::EWidget_Root;
                }
                else if (attrName == E_STR_TEXT("w"))
                {
                    pData->widgetType = UI::Builder::EWidget_Widget;
                    auto pWidget      = &pData->exData.widget;
                    pWidget->x        = in_rNode.attribute("x").as_float();
                    pWidget->y        = in_rNode.attribute("y").as_float();
                }
                else if (attrName == E_STR_TEXT("b"))
                {
                    pData->widgetType = UI::Builder::EWidget_Button;
                    auto pBtn         = &pData->exData.button;
                    pBtn->x           = in_rNode.attribute("x").as_float();
                    pBtn->y           = in_rNode.attribute("y").as_float();

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pBtn->style, s, static_cast<Uint32>(::strlen(s)));
                }
                else if (attrName == E_STR_TEXT("t"))
                {
                    pData->widgetType = UI::Builder::EWidget_Label;
                    auto pLabel       = &pData->exData.label;
                    pLabel->x         = in_rNode.attribute("x").as_float();
                    pLabel->y         = in_rNode.attribute("y").as_float();

                    // ローカライズテキストか
                    if (in_rNode.attribute("loc").as_bool())
                    {
                        pLabel->bLoc = TRUE;
                    }

                    auto t = in_rNode.attribute("text").value();
                    if (0 < ::strlen(t))
                    {
                        Core::Common::FixString1024 tmp(t);
                        if (pLabel->bLoc)
                        {
                            Core::Common::FastFixArray<Core::Common::FixString1024, 3> split;

                            Core::Common::OutputSplitString(split, tmp, s_locGroupDelimita);
                            E_STR_CPY_S(pLabel->loc, E_ARRAY_NUM(pLabel->loc), split[0].Str(),
                                        split[0].Length());

                            E_STR_CPY_S(pLabel->text, E_ARRAY_NUM(pLabel->text), split[1].Str(),
                                        split[1].Length());
                        }
                        else
                        {
                            E_STR_CPY_S(pLabel->text, E_ARRAY_NUM(pLabel->text), tmp.Str(),
                                        tmp.Length());
                        }
                    }

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pLabel->style, s, static_cast<Uint32>(::strlen(s)));
                }
                else if (attrName == E_STR_TEXT("l"))
                {
                    pData->widgetType = UI::Builder::EWidget_Layout;
                    auto pLayout      = &pData->exData.layout;

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pLayout->style, s, static_cast<Uint32>(::strlen(s)));
                }

                Core::Common::FixString1024 idName(in_rNode.attribute("id").value());
                E_STR_ERRNO e =
                    E_STR_CPY_S(pData->id, E_ARRAY_NUM(pData->id), idName.Str(), idName.Length());
                E_ASSERT(E_STR_SUCCESS(e) && "文字列のコピーでエラー");
            }
        }  // namespace Local

        const Bool UILayoutData::OutputNodeByRootPos(Node* out, const Byte* in_pName)
        {
            E_ASSERT(out);

            Core::Common::FixString256 nodeName(in_pName);
            Uint64 targetNodeHash = nodeName.Hash();

            auto libNode = this->_doc.child(in_pName);
            E_ASSERT(libNode.empty() == FALSE && "ノードがない");

            Local::ApplyNode(out, libNode);

            return TRUE;
        }

        const Bool UILayoutData::OutputNode(Node* out, const Node& in_rParentNode,
                                            const Byte* in_pName)
        {
            E_ASSERT(out);
            E_ASSERT(in_rParentNode.pNode);

            pugi::xml_node libNode(in_rParentNode.pNode);
            libNode = libNode.child(in_pName);

            Node node;
            Local::ApplyNode(out, libNode);

            return TRUE;
        }

        void UILayoutData::OutputNodeChildren(Core::Common::FastFixArrayBase<Node>* out,
                                              const Node& in_rParentNode)
        {
            E_ASSERT(out);

            pugi::xml_node libNode(in_rParentNode.pNode);
            {
                for (auto b = libNode.begin(); b != libNode.end(); ++b)
                {
                    Node node;
                    Local::ApplyNode(&node, *b);
                    out->PushBack(node);
                }
            }
        }

    }  // namespace Builder
}  // namespace UI
