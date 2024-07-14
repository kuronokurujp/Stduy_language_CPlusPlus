#include "UIModule.h"

#include "Component/Input/UIInputRouter.h"
#include "Component/Input/UIInputTerminalTouch.h"
#include "Component/Widget/UIButton.h"
#include "Component/Widget/UILayer.h"
#include "Component/Widget/UIText.h"
#include "Core/Memory/Memory.h"
#include "Widget.h"

MODULE_GENRATE_DEFINITION(UI::UIModule, UI);

namespace UI
{
    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool UIModule::Start()
    {
        return TRUE;
    }

    Core::Common::Handle UIModule::LoadAssetWithLayoutBuild(const Core::File::Path& in_rFilePath)
    {
        Core::Common::Handle handle =
            ModuleAssetManager()->Load<UI::Builder::UILayoutData>(in_rFilePath.Str(),
                                                                  Core::File::Path(
                                                                      in_rFilePath.Str()));
        HE_ASSERT(handle.Null() == FALSE);
        if (handle.Null()) return InvalidHandle;

        return handle;
    }

    void UIModule::UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        ModuleAssetManager()->Unload(in_rHandle);
    }

    Core::Common::Handle UIModule::NewLayer(const Core::Common::FixStringBase& in_szrName,
                                            const Uint32 in_uSort)
    {
        Core::Common::Handle handle = this->NewWidget(in_szrName, in_uSort);

        // 入力ルーターコンポーネントを設定
        auto hInputRouter = this->AddComponent<UI::UIInputRouterComponent>(handle, 0);

        // レイヤーコンポーネントを追加
        auto hLayer = this->AddComponent<UI::UILayerComponent>(handle, 0);

        return handle;
    }

    Core::Common::Handle UIModule::NewLayoutByLayotuAsset(Core::Common::Handle& in_rHandle,
                                                          const Uint32 in_uSort)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        auto& asset = ModuleAssetManager()->GetAsset<UI::Builder::UILayoutData>(in_rHandle);
        // ルートノードを取得
        UI::Builder::Node node;
        auto bRet = asset.OutputNodeByRootPos(&node, "ui");
        HE_ASSERT(bRet);

        // ルートノードからレイアウトノードを取得
        UI::Builder::Node layoutNode;
        bRet = asset.OutputNode(&layoutNode, node, "l");
        HE_ASSERT(bRet);
        HE_ASSERT(layoutNode.data.eWidgetType == UI::Builder::EWidget_Layout &&
                  "レイアウトノードが存在しない");

        // レイアウトを作成
        Core::Common::Handle hLayout =
            this->NewLayer(Core::Common::FixString256(layoutNode.data.szId), in_uSort);

        // レイアウトノード下にあるWidgetを取得
        // MEMO: 要素数が1000を超えるとスタックサイズが足りずにスタックオーバーフローになるので注意
        Core::Common::FastFixArray<UI::Builder::Node, 64> stack;

        Uint32 sort = 0;
        Core::Common::FastFixArray<UI::Builder::Node, 64> nodeChildren;
        asset.OutputNodeChildren(&nodeChildren, layoutNode);

        for (Uint32 i = 0; i < nodeChildren.Size(); ++i)
        {
            Core::Common::Handle hParentWidget = hLayout;

            // 再帰処理をしている
            stack.PushBack(nodeChildren[i]);
            while (stack.Empty() == FALSE)
            {
                auto widgetNode                       = stack.PopBack();
                const auto pNodeData                  = &widgetNode.data;
                const UI::Builder::EWidget widgetType = pNodeData->eWidgetType;
                // TODO: 関数テーブルにしてswitch文を消す方向にするかも
                // 今は種類が少ないからいいが, 数が膨大になるとまずいから
                switch (widgetType)
                {
                    case UI::Builder::EWidget_None:
                        break;
                    case UI::Builder::EWidget_Widget:
                    {
                        // TODO: 未対応
                        break;
                    }
                    case UI::Builder::EWidget_Label:
                    {
                        const UI::Builder::Node::Data::ExData::Label* pLabel =
                            &pNodeData->exData.label;
                        const UI::Builder::Style* pStyle = &pLabel->style;
                        const Core::Math::Rect2 rect =
                            Core::Math::Rect2(pLabel->fX, pLabel->fY, pStyle->w, pStyle->h,
                                              Core::Math::Rect2::EPivot_Left);

                        auto h = this->NewLabelWidget(Core::Common::FixString256(pNodeData->szId),
                                                      sort, pLabel->szLoc, pLabel->szText, rect,
                                                      pStyle->color);

                        this->AddChildWidget(hParentWidget, h);
                        hParentWidget = h;

                        sort += 1;
                        break;
                    }
                    case UI::Builder::EWidget_Button:
                    {
                        const UI::Builder::Node::Data::ExData::Button* pButton =
                            &pNodeData->exData.button;
                        const UI::Builder::Style* pStyle = &pButton->style;

                        auto h =
                            this->NewButtonWidget(Core::Common::FixString256(pNodeData->szId), sort,
                                                  Core::Math::Rect2(pButton->fX, pButton->fY,
                                                                    pStyle->w, pStyle->h,
                                                                    Core::Math::Rect2::EPivot_Left),
                                                  pStyle->color);

                        // TODO: ボタンを押した時のイベントを設定
                        {
                            Actor::Object* pWidget = NULL;
                            pWidget                = LEVEL_MODULE_CURRENT_LEVEL->GetActor(h);
                            HE_ASSERT(pWidget);

                            auto handle =
                                pWidget->GetComponentHandle(&UI::UIButtonComponent::CLASS_RTTI);
                            HE_ASSERT(handle.Null() == FALSE);
                            UI::UIButtonComponent* pBtnComp =
                                pWidget->GetComponent<UI::UIButtonComponent>(handle);
                            //                                reinterpret_cast<UI::UIButtonComponent*>(

                            auto handler = std::make_unique<UI::UIButtonMessageHandlerDefault>(
                                pNodeData->szId,
                                [this](Core::Common::FixString128 in_msg)
                                {
                                    // ボタン入力をレベルのコンポーネントに通知
                                    //                                        reinterpret_cast<Level::LevelUserInputReceiveComponent*>(
                                    auto handle = LEVEL_MODULE_CURRENT_LEVEL->GetComponentHandle(
                                        &Level::LevelUserInputReceiveComponent::CLASS_RTTI);
                                    if (handle.Null() == FALSE)
                                    //                                    if (pUserInputEventComp)
                                    {
                                        Level::LevelUserInputReceiveComponent* pUserInputEventComp =
                                            LEVEL_MODULE_CURRENT_LEVEL->GetComponent<
                                                Level::LevelUserInputReceiveComponent>(handle);
                                        ;
                                        pUserInputEventComp->Message(in_msg.Str());
                                    }
                                });
                            pBtnComp->SetPushHandler(std::move(handler));
                        }

                        this->AddChildWidget(hParentWidget, h);
                        hParentWidget = h;

                        sort += 1;
                        break;
                    }
                }

                Core::Common::FastFixArray<UI::Builder::Node, 64> tmpNodeChildren;
                asset.OutputNodeChildren(&tmpNodeChildren, widgetNode);
                for (Uint32 i2 = 0; i2 < tmpNodeChildren.Size(); ++i2)
                {
                    stack.PushBack(tmpNodeChildren[i2]);
                }
            }
        }

        return hLayout;
    }

    Core::Common::Handle UIModule::NewLabelWidget(const Core::Common::FixStringBase& in_szrName,
                                                  const Uint32 in_uSort,
                                                  const Char* in_szLocGroupName,
                                                  const Char* in_szText,
                                                  const Core::Math::Rect2& in_rTextRect,
                                                  const Uint32 in_uTextColor)
    {
        Core::Common::Handle handle = this->NewWidget(in_szrName, in_uSort);
        HE_ASSERT(handle.Null() == FALSE);

        Actor::Object* pWidget = NULL;
        pWidget                = LEVEL_MODULE_CURRENT_LEVEL->GetActor(handle);
        if (pWidget == NULL) return handle;

        // ボタンの上に表示するテキストコンポーネント追加と設定
        auto textHandle = this->AddComponent<UI::UITextComponent>(handle, in_uSort + 1);
        {
            UI::UITextComponent* pText = pWidget->GetComponent<UI::UITextComponent>(textHandle);
            pText->SetText(in_szText);
            pText->SetRect(in_rTextRect);
            pText->SetColor(in_uTextColor);
            pText->SetLocGroupName(in_szLocGroupName);
        }

        return handle;
    }

    Core::Common::Handle UIModule::NewButtonWidget(const Core::Common::FixStringBase& in_szrName,
                                                   const Uint32 in_uSort,
                                                   const Core::Math::Rect2& in_rBtnRect,
                                                   const Uint32 in_uBtnColor)
    {
        Core::Common::Handle handle = this->NewWidget(in_szrName, in_uSort);
        HE_ASSERT(handle.Null() == FALSE);

        Widget* pWidget = NULL;
        pWidget         = reinterpret_cast<Widget*>(LEVEL_MODULE_CURRENT_LEVEL->GetActor(handle));
        if (pWidget == NULL) return InvalidHandle;

        pWidget->SetPos(Core::Math::Vector3(in_rBtnRect.Pos()));

        // ボタンコンポーネント追加と設定
        auto hButton = this->AddComponent<UI::UIButtonComponent>(handle, in_uSort);
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UI::UIButtonComponent>(hButton);
            pButton->SetWidth(in_rBtnRect.Width());
            pButton->SetHeight(in_rBtnRect.Height());
            pButton->SetColor(in_uBtnColor);
        }

        // 入力ルーターの端末コンポーネントを設定
        auto inputTerminalHandle = this->AddComponent<UI::UIInputTouchComponent>(handle, 0);

        return handle;
    }

    Core::Common::Handle UIModule::NewWidget(const Core::Common::FixStringBase& in_szrName,
                                             const Uint32 in_uSort)
    {
        Core::Common::Handle handle;

        Actor::Object* pWidget = NULL;
        handle                 = LEVEL_MODULE_CURRENT_LEVEL->AddActor<Widget>();
        HE_ASSERT(handle.Null() == FALSE);

        pWidget = LEVEL_MODULE_CURRENT_LEVEL->GetActor(handle);
        if (pWidget == NULL) return InvalidHandle;

        // Widgetの初期設定
        pWidget->SetName(in_szrName);

        return handle;
    }

    const Bool UIModule::AddChildWidget(Core::Common::Handle& in_rParent,
                                        Core::Common::Handle& in_rWidget)
    {
        return LEVEL_MODULE_CURRENT_LEVEL->AddParentActor(in_rWidget, in_rParent);
    }

    const Bool UIModule::Release()
    {
        return TRUE;
    }
}  // namespace UI
