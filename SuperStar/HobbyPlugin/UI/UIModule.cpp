#include "UIModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"
#include "LevelModule.h"

namespace UI
{
    // クラス内のみで利用する
    namespace Local
    {
        static Core::Common::CustomFixMap<Builder::EAnchor, Core::Math::Rect2::EAnchor, 3>
            mPosAnthorToRect2Anchor = {{Builder::EAnchor::EAnchor_Left,
                                        Core::Math::Rect2::EAnchor::EAnchor_Left},
                                       {Builder::EAnchor::EPosAnchor_Center,
                                        Core::Math::Rect2::EAnchor::EAnchor_Center}};
    }

    UIModule::UIModule() : ModuleBase(ModuleName())
    {
        // 依存しているモジュールを設定
        // モジュールの依存設定していないと依存したモジュールが使えない
        this->_AppendDependenceModule<AssetManager::AssetManagerModule>();
        this->_AppendDependenceModule<Level::LevelModule>();
        this->_AppendDependenceModule<Localization::LocalizationModule>();
        this->_AppendDependenceModule<Render::RenderModule>();
        this->_AppendDependenceModule<Actor::ActorModule>();
    }

    Widget* UIModule::GetWidget(const UIWidgetHandlePack& in_rHandlePack)
    {
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pActor = NULL;
        pActor                = pLevelModule->GetLevel(in_rHandlePack._levelHandle)
                     .GetActor<Actor::Object>(in_rHandlePack._widgetHandle);
        HE_ASSERT(pActor);

        return reinterpret_cast<Widget*>(pActor);
    }

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool UIModule::_VStart()
    {
        return TRUE;
    }

    Core::Common::Handle UIModule::LoadAssetWithLayoutBuild(const Core::File::Path& in_rFilePath)
    {
        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);
        Core::Common::Handle handle =

            pAssetManagerModule->Load<UI::Builder::UILayoutData>(in_rFilePath.Str(),
                                                                 Core::File::Path(
                                                                     in_rFilePath.Str()));

        HE_ASSERT(handle.Null() == FALSE);
        if (handle.Null()) return NullHandle;

        return handle;
    }

    void UIModule::UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        pAssetManagerModule->Unload(in_rHandle);
    }

    const UIWidgetHandlePack UIModule::NewLayer(const Core::Common::StringBase& in_szrName,
                                                const Uint32 in_uSort,
                                                const Core::Common::Handle& in_rLevelHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort, in_rLevelHandle);

        // 入力ルーターコンポーネントを設定
        auto hInputRouter = this->AddComponent<UI::UIInputRouterComponent>(handlePack, 0);

        // レイヤーコンポーネントを追加
        auto hLayer = this->AddComponent<UI::UILayerComponent>(handlePack, 0);

        return handlePack;
    }

    const UIWidgetHandlePack UIModule::NewLayoutByLayotuAsset(
        const Core::Common::Handle& in_rAssetHandle, const Uint32 in_uSort,
        const Core::Common::Handle& in_rViewHandle, const Core::Common::Handle& in_rLevelHandle)
    {
        HE_ASSERT(in_rAssetHandle.Null() == FALSE);
        HE_ASSERT(in_rLevelHandle.Null() == FALSE);
        HE_ASSERT(in_rViewHandle.Null() == FALSE);

        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        auto& asset = pAssetManagerModule->GetAsset<UI::Builder::UILayoutData>(in_rAssetHandle);

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
        UIWidgetHandlePack widgetHandlePack =
            this->NewLayer(Core::Common::FixString64(layoutNode.data.szId), in_uSort,
                           in_rLevelHandle);

        // レイアウトノード下にあるWidgetを取得
        // MEMO: 要素数が1000を超えるとスタックサイズが足りずにスタックオーバーフローになるので注意
        Core::Common::CustomFixStack<UI::Builder::Node, 16> stack;

        Uint32 sort = 0;
        Core::Common::CustomFixStack<UI::Builder::Node, 64> sNodeChildren;
        asset.OutputNodeChildren(&sNodeChildren, layoutNode);

        Core::Common::CustomFixStack<UI::Builder::Node, 16> sTmpNodeChildren;
        while (sNodeChildren.Empty() == FALSE)
        {
            auto hParentWidget = widgetHandlePack;

            // 再帰処理をしている
            stack.PushBack(sNodeChildren.PopBack());
            while (stack.Empty() == FALSE)
            {
                auto widgetNode                       = stack.PopBack();
                const auto pNodeData                  = &widgetNode.data;
                const UI::Builder::EWidget widgetType = pNodeData->eWidgetType;
                // TODO: 関数テーブルにしてswitch文を消す方向にするかも
                // 今は種類が少ないからいいが, 数が膨大になるとまずい
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
                                              Local::mPosAnthorToRect2Anchor[pLabel->eAnchor]);

                        auto h =
                            this->NewLabelWidget(Core::Common::FixString64(pNodeData->szId), sort,
                                                 pLabel->szLoc, pLabel->szText, rect, pStyle->color,
                                                 in_rViewHandle, in_rLevelHandle);

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

                        auto h = this->NewButtonWidget(
                            Core::Common::FixString64(pNodeData->szId), sort,
                            Core::Math::Rect2(pButton->fX, pButton->fY, pStyle->w, pStyle->h,
                                              Local::mPosAnthorToRect2Anchor[pButton->eAnchor]),
                            pStyle->color, in_rViewHandle, in_rLevelHandle);

                        // ボタンを押した時のイベントを設定
                        {
                            Actor::Object* pWidget = NULL;
                            pWidget = pLevelModule->GetLevel(widgetHandlePack._levelHandle)
                                          .GetActor<Actor::Object>(h._widgetHandle);
                            HE_ASSERT(pWidget);

                            auto handle =
                                pWidget->GetComponentHandle(&UI::UIButtonComponent::CLASS_RTTI);
                            HE_ASSERT(handle.Null() == FALSE);
                            UI::UIButtonComponent* pBtnComp =
                                pWidget->GetComponent<UI::UIButtonComponent>(handle);

                            auto handler = Core::Memory::MakeCustomUniquePtr<
                                UI::UIButtonMessageHandlerDefault>(
                                pNodeData->szId,
                                [this, &widgetHandlePack](Core::Common::StringBase& in_msg)
                                {
                                    auto pLevelModule =
                                        this->GetDependenceModule<Level::LevelModule>();
                                    HE_ASSERT(pLevelModule);

                                    // ボタン入力をレベルのコンポーネントに通知
                                    auto handle =
                                        pLevelModule->GetLevel(widgetHandlePack._levelHandle)
                                            .GetComponentHandle(
                                                &Level::LevelUserInputReceiveComponent::CLASS_RTTI);
                                    if (handle.Null() == FALSE)
                                    {
                                        Level::LevelUserInputReceiveComponent* pUserInputEventComp =
                                            pLevelModule->GetLevel(widgetHandlePack._levelHandle)
                                                .GetComponent<
                                                    Level::LevelUserInputReceiveComponent>(handle);
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

                sTmpNodeChildren.Clear();
                asset.OutputNodeChildren(&sTmpNodeChildren, widgetNode);
                while (sTmpNodeChildren.Empty() == FALSE)
                {
                    stack.PushBack(sTmpNodeChildren.PopBack());
                }
            }
        }

        return widgetHandlePack;
    }

    const UIWidgetHandlePack UIModule::NewLabelWidget(
        const Core::Common::StringBase& in_szrName, const Uint32 in_uSort,
        const Char* in_szLocGroupName, const Char* in_szText, const Core::Math::Rect2& in_rTextRect,
        const Uint32 in_uTextColor, const Core::Common::Handle& in_rViewHandle,
        const Core::Common::Handle& in_rLevelHandle)
    {
        auto handlePack = this->NewWidget(in_szrName, in_uSort, in_rLevelHandle);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pWidget = NULL;

        pWidget = pLevelModule->GetLevel(handlePack._levelHandle)
                      .GetActor<Actor::Object>(handlePack._widgetHandle);
        if (pWidget == NULL) return handlePack;

        // ボタンの上に表示するテキストコンポーネント追加と設定
        auto textHandle = this->AddComponent<UI::UITextComponent>(handlePack, in_uSort + 1);
        {
            UI::UITextComponent* pText = pWidget->GetComponent<UI::UITextComponent>(textHandle);
            pText->SetPos(in_rTextRect.Pos());
            pText->SetViewHandle(in_rViewHandle);
            pText->SetText(in_szText);
            pText->SetRect(Core::Math::Rect2(0, 0, in_rTextRect.Width(), in_rTextRect.Height(),
                                             in_rTextRect._eAnchor));
            pText->SetColor(in_uTextColor);
            pText->SetLocGroupName(in_szLocGroupName);
            pText->SetAnchor(in_rTextRect._eAnchor);
        }

        return handlePack;
    }

    const UIWidgetHandlePack UIModule::NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                       const Uint32 in_uSort,
                                                       const Core::Math::Rect2& in_rBtnRect,
                                                       const Uint32 in_uBtnColor,
                                                       const Core::Common::Handle& in_rViewHandle,
                                                       const Core::Common::Handle& in_rLevelHandle)

    {
        auto widgetHandle = this->NewWidget(in_szrName, in_uSort, in_rLevelHandle);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Widget* pWidget = pLevelModule->GetLevel(widgetHandle._levelHandle)
                              .GetActor<Widget>(widgetHandle._widgetHandle);
        if (pWidget == NULL) return widgetHandle;

        // ボタンコンポーネント追加と設定
        auto hButton = this->AddComponent<UI::UIButtonComponent>(widgetHandle, in_uSort);
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UI::UIButtonComponent>(hButton);
            pButton->SetPos(in_rBtnRect.Pos());
            pButton->SetViewHandle(in_rViewHandle);
            pButton->SetWidth(in_rBtnRect.Width());
            pButton->SetHeight(in_rBtnRect.Height());
            pButton->SetColor(in_uBtnColor);
            pButton->SetAnchor(in_rBtnRect._eAnchor);
        }

        return widgetHandle;
    }

    const UIWidgetHandlePack UIModule::NewWidget(const Core::Common::StringBase& in_szrName,
                                                 const Uint32 in_uSort,
                                                 const Core::Common::Handle& in_rLevelHandle)
    {
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        auto handle = pLevelModule->GetLevel(in_rLevelHandle).AddActor<Widget>();
        HE_ASSERT(handle.Null() == FALSE);

        return UIWidgetHandlePack(handle, in_rLevelHandle);
    }

    const Bool UIModule::AddChildWidget(UIWidgetHandlePack& in_rParent,
                                        UIWidgetHandlePack& in_rWidget)
    {
        HE_ASSERT(in_rParent._levelHandle == in_rWidget._levelHandle);
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        return pLevelModule->GetLevel(in_rParent._levelHandle)
            .ChainActor(in_rWidget._widgetHandle, in_rParent._widgetHandle);
    }

    const Bool UIModule::_VRelease()
    {
        return TRUE;
    }
}  // namespace UI
