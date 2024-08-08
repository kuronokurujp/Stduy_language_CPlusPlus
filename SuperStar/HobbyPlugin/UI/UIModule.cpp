#include "UIModule.h"

#include "Engine/Common/CustomMap.h"
#include "Engine/Memory/Memory.h"

// MODULE_GENRATE_DEFINITION(UI::UIModule, UI);

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

    /// <summary>
    /// UIのモジュール初期化
    /// </summary>
    /// <returns></returns>
    const Bool UIModule::_Start()
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
        if (handle.Null()) return InvalidHandle;

        return handle;
    }

    void UIModule::UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        pAssetManagerModule->Unload(in_rHandle);
    }

    Core::Common::Handle UIModule::NewLayer(const Core::Common::StringBase& in_szrName,
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
        auto pAssetManagerModule = this->GetDependenceModule<AssetManager::AssetManagerModule>();
        HE_ASSERT(pAssetManagerModule);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        auto& asset = pAssetManagerModule->GetAsset<UI::Builder::UILayoutData>(in_rHandle);

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
            this->NewLayer(Core::Common::FixString64(layoutNode.data.szId), in_uSort);

        // レイアウトノード下にあるWidgetを取得
        // MEMO: 要素数が1000を超えるとスタックサイズが足りずにスタックオーバーフローになるので注意
        Core::Common::CustomFixStack<UI::Builder::Node, 16> stack;

        Uint32 sort = 0;
        Core::Common::CustomFixStack<UI::Builder::Node, 64> sNodeChildren;
        asset.OutputNodeChildren(&sNodeChildren, layoutNode);

        Core::Common::CustomFixStack<UI::Builder::Node, 16> sTmpNodeChildren;
        while (sNodeChildren.Empty() == FALSE)
        {
            Core::Common::Handle hParentWidget = hLayout;

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

                        auto h = this->NewLabelWidget(Core::Common::FixString64(pNodeData->szId),
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

                        auto h = this->NewButtonWidget(
                            Core::Common::FixString64(pNodeData->szId), sort,
                            Core::Math::Rect2(pButton->fX, pButton->fY, pStyle->w, pStyle->h,
                                              Local::mPosAnthorToRect2Anchor[pButton->eAnchor]),
                            pStyle->color);

                        // ボタンを押した時のイベントを設定
                        {
                            Actor::Object* pWidget = NULL;
                            pWidget                = pLevelModule->GetCurrneLevel().GetActor(h);
                            HE_ASSERT(pWidget);

                            auto handle =
                                pWidget->GetComponentHandle(&UI::UIButtonComponent::CLASS_RTTI);
                            HE_ASSERT(handle.Null() == FALSE);
                            UI::UIButtonComponent* pBtnComp =
                                pWidget->GetComponent<UI::UIButtonComponent>(handle);

                            auto handler = Core::Memory::MakeCustomUniquePtr<
                                UI::UIButtonMessageHandlerDefault>(
                                pNodeData->szId,
                                [this](Core::Common::StringBase& in_msg)
                                {
                                    auto pLevelModule =
                                        this->GetDependenceModule<Level::LevelModule>();
                                    HE_ASSERT(pLevelModule);

                                    // ボタン入力をレベルのコンポーネントに通知
                                    auto handle = pLevelModule->GetCurrneLevel().GetComponentHandle(
                                        &Level::LevelUserInputReceiveComponent::CLASS_RTTI);
                                    if (handle.Null() == FALSE)
                                    {
                                        Level::LevelUserInputReceiveComponent* pUserInputEventComp =
                                            pLevelModule->GetCurrneLevel()
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

        return hLayout;
    }

    Core::Common::Handle UIModule::NewLabelWidget(const Core::Common::StringBase& in_szrName,
                                                  const Uint32 in_uSort,
                                                  const Char* in_szLocGroupName,
                                                  const Char* in_szText,
                                                  const Core::Math::Rect2& in_rTextRect,
                                                  const Uint32 in_uTextColor)
    {
        Core::Common::Handle handle = this->NewWidget(in_szrName, in_uSort);
        HE_ASSERT(handle.Null() == FALSE);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pWidget = NULL;

        pWidget = pLevelModule->GetCurrneLevel().GetActor(handle);
        if (pWidget == NULL) return handle;

        pWidget->SetPos(Core::Math::Vector3(in_rTextRect.Pos()));

        // ボタンの上に表示するテキストコンポーネント追加と設定
        auto textHandle = this->AddComponent<UI::UITextComponent>(handle, in_uSort + 1);
        {
            UI::UITextComponent* pText = pWidget->GetComponent<UI::UITextComponent>(textHandle);
            pText->SetText(in_szText);
            pText->SetRect(Core::Math::Rect2(0, 0, in_rTextRect.Width(), in_rTextRect.Height(),
                                             in_rTextRect._eAnchor));
            pText->SetColor(in_uTextColor);
            pText->SetLocGroupName(in_szLocGroupName);
            pText->SetAnchor(in_rTextRect._eAnchor);
        }

        return handle;
    }

    Core::Common::Handle UIModule::NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                   const Uint32 in_uSort,
                                                   const Core::Math::Rect2& in_rBtnRect,
                                                   const Uint32 in_uBtnColor)
    {
        Core::Common::Handle handle = this->NewWidget(in_szrName, in_uSort);
        HE_ASSERT(handle.Null() == FALSE);

        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Widget* pWidget = NULL;
        pWidget = reinterpret_cast<Widget*>(pLevelModule->GetCurrneLevel().GetActor(handle));
        if (pWidget == NULL) return InvalidHandle;

        pWidget->SetPos(Core::Math::Vector3(in_rBtnRect.Pos()));

        // ボタンコンポーネント追加と設定
        auto hButton = this->AddComponent<UI::UIButtonComponent>(handle, in_uSort);
        {
            UI::UIButtonComponent* pButton = pWidget->GetComponent<UI::UIButtonComponent>(hButton);
            pButton->SetWidth(in_rBtnRect.Width());
            pButton->SetHeight(in_rBtnRect.Height());
            pButton->SetColor(in_uBtnColor);
            pButton->SetAnchor(in_rBtnRect._eAnchor);
        }

        // 入力ルーターの端末コンポーネントを設定
        auto inputTerminalHandle = this->AddComponent<UI::UIInputTouchComponent>(handle, 0);

        return handle;
    }

    Core::Common::Handle UIModule::NewWidget(const Core::Common::StringBase& in_szrName,
                                             const Uint32 in_uSort)
    {
        Core::Common::Handle handle;
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        Actor::Object* pWidget = NULL;
        handle                 = pLevelModule->GetCurrneLevel().AddActor<Widget>();
        HE_ASSERT(handle.Null() == FALSE);

        pWidget = pLevelModule->GetCurrneLevel().GetActor(handle);
        if (pWidget == NULL) return InvalidHandle;

        // Widgetの初期設定
        pWidget->SetName(in_szrName);

        return handle;
    }

    const Bool UIModule::AddChildWidget(Core::Common::Handle& in_rParent,
                                        Core::Common::Handle& in_rWidget)
    {
        auto pLevelModule = this->GetDependenceModule<Level::LevelModule>();
        HE_ASSERT(pLevelModule);

        return pLevelModule->GetCurrneLevel().ChainActor(in_rWidget, in_rParent);
    }

    const Bool UIModule::_Release()
    {
        return TRUE;
    }
}  // namespace UI
