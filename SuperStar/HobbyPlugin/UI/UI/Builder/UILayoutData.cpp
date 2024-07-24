#include "UILayoutData.h"

#include <string.h>

// 依存モジュール
#include "RenderModule.h"

namespace UI
{
    namespace Builder
    {
        /// <summary>
        /// ローカル内のみ使用
        /// </summary>
        namespace Local
        {
            static const Char s_treeNodeDelimita = HE_STR_TEXT('.');
            static const Char s_locGroupDelimita = HE_STR_TEXT('.');

            /// <summary>
            /// UIのスタイルカラー
            /// </summary>
            struct StyleColor
            {
            public:
                StyleColor(const UTF8* in_szName, Render::Color in_rgba)
                {
                    this->szName = in_szName;
                    this->rgba   = in_rgba;
                }

                const UTF8* szName = NULL;
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

            static void _ParseStyle(Style* out, const UTF8* in_szName, const Uint32 in_uSize)
            {
                static UTF8 buff[1024] = {0};

                HE_ASSERT(in_uSize < HE_ARRAY_SIZE(buff));
                ::memset(out, 0, sizeof(Style));

                ::memcpy_s(buff, HE_ARRAY_SIZE(buff), in_szName, in_uSize);
                UTF8* pRestartToken = NULL;
                UTF8* pToken        = ::strtok_s(buff, ";", &pRestartToken);

                while (pToken != NULL)
                {
                    UTF8* szKey  = ::strtok(pToken, ":");
                    UTF8* pValue = ::strtok(NULL, ":");

                    if (szKey && pValue)
                    {
                        while (*szKey == ' ') ++szKey;
                        while (*pValue == ' ') ++pValue;

                        if (::strcmp(szKey, "w") == 0)
                        {
                            out->w = static_cast<Float32>(::atof(pValue));
                        }
                        else if (::strcmp(szKey, "h") == 0)
                        {
                            out->h = static_cast<Float32>(::atoi(pValue));
                        }
                        else if (::strcmp(szKey, "color") == 0)
                        {
                            UTF8 szColorName[128] = {0};
                            ::strncpy_s(szColorName, HE_ARRAY_NUM(szColorName), pValue,
                                        HE_ARRAY_NUM(szColorName));
                            // TODO: キーワードと色名の対応テーブルを作る
                            for (Uint32 i = 0; i < HE_ARRAY_NUM(s_aColorTable); ++i)
                            {
                                if (::strcmp(s_aColorTable[i].szName, szColorName) == 0)
                                {
                                    out->color = s_aColorTable[i].rgba.c;
                                    break;
                                }
                            }
                        }
                    }

                    pToken = ::strtok_s(NULL, ";", &pRestartToken);
                }
            }

            static void ApplyNode(Node* out, const pugi::xml_node& in_rNode)
            {
                HE_ASSERT(out);
                ::memset(out, 0, sizeof(Node));

                // サードパーティライブラリのノードポインタを保存
                out->pNode = in_rNode.internal_object();

                Node::Data* pData = &out->data;

                // TODO: ノードから反映する情報を抜き出す
                pData->eWidgetType = UI::Builder::EWidget_None;

                Core::Common::FixString128 szAttrName = in_rNode.name();
                if (szAttrName == HE_STR_TEXT("ui"))
                {
                    pData->eWidgetType = UI::Builder::EWidget_Root;
                }
                else if (szAttrName == HE_STR_TEXT("w"))
                {
                    pData->eWidgetType = UI::Builder::EWidget_Widget;
                    auto pWidget       = &pData->exData.widget;
                    pWidget->fX        = in_rNode.attribute("x").as_float();
                    pWidget->fY        = in_rNode.attribute("y").as_float();
                }
                else if (szAttrName == HE_STR_TEXT("b"))
                {
                    pData->eWidgetType = UI::Builder::EWidget_Button;
                    auto pBtn          = &pData->exData.button;
                    pBtn->fX           = in_rNode.attribute("x").as_float();
                    pBtn->fY           = in_rNode.attribute("y").as_float();

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pBtn->style, s, static_cast<Uint32>(::strlen(s)));
                }
                else if (szAttrName == HE_STR_TEXT("t"))
                {
                    pData->eWidgetType = UI::Builder::EWidget_Label;
                    auto pLabel        = &pData->exData.label;
                    pLabel->fX         = in_rNode.attribute("x").as_float();
                    pLabel->fY         = in_rNode.attribute("y").as_float();

                    // ローカライズテキストか
                    if (in_rNode.attribute("loc").as_bool())
                    {
                        pLabel->bLoc = TRUE;
                    }

                    auto t = in_rNode.attribute("text").value();
                    if (0 < ::strlen(t))
                    {
                        Core::Common::FixString1024 szTmp(t);
                        if (pLabel->bLoc)
                        {
                            Core::Common::FastFixArray<Core::Common::FixString1024, 3> szaSplitName;

                            Core::Common::OutputSplitString(szaSplitName, szTmp,
                                                            s_locGroupDelimita);
                            HE_STR_CPY_S(pLabel->szLoc, HE_ARRAY_NUM(pLabel->szLoc),
                                         szaSplitName[0].Str(), szaSplitName[0].Length());

                            HE_STR_CPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText),
                                         szaSplitName[1].Str(), szaSplitName[1].Length());
                        }
                        else
                        {
                            HE_STR_CPY_S(pLabel->szText, HE_ARRAY_NUM(pLabel->szText), szTmp.Str(),
                                         szTmp.Length());
                        }
                    }

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pLabel->style, s, static_cast<Uint32>(::strlen(s)));
                }
                else if (szAttrName == HE_STR_TEXT("l"))
                {
                    pData->eWidgetType = UI::Builder::EWidget_Layout;
                    auto pLayout       = &pData->exData.layout;

                    auto s = in_rNode.attribute("style").value();
                    _ParseStyle(&pLayout->style, s, static_cast<Uint32>(::strlen(s)));
                }

                Core::Common::FixString1024 szIdName(in_rNode.attribute("id").value());
                HE_STR_ERRNO e = HE_STR_CPY_S(pData->szId, HE_ARRAY_NUM(pData->szId),
                                              szIdName.Str(), szIdName.Length());
                HE_ASSERT(HE_STR_SUCCESS(e) && "文字列のコピーでエラー");
            }
        }  // namespace Local

        const Bool UILayoutData::OutputNodeByRootPos(Node* out, const UTF8* in_szName)
        {
            HE_ASSERT(out);

            Core::Common::FixString256 nodeName(in_szName);
            Uint64 ulTargetNodeHash = nodeName.Hash();

            auto libNode = this->_doc.child(in_szName);
            HE_ASSERT(libNode.empty() == FALSE && "ノードがない");

            Local::ApplyNode(out, libNode);

            return TRUE;
        }

        const Bool UILayoutData::OutputNode(Node* out, const Node& in_rParentNode,
                                            const UTF8* in_szName)
        {
            HE_ASSERT(out);
            HE_ASSERT(in_rParentNode.pNode);

            pugi::xml_node libNode(in_rParentNode.pNode);
            libNode = libNode.child(in_szName);

            Node node;
            Local::ApplyNode(out, libNode);

            return TRUE;
        }

        void UILayoutData::OutputNodeChildren(Core::Common::FastFixArrayBase<Node>* out,
                                              const Node& in_rParentNode)
        {
            HE_ASSERT(out);

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
