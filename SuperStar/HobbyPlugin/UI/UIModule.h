﻿#pragma once

#include "MiniEngine.h"
#ifdef _HOBBY_ENGINE
#include "Engine.h"
#endif

#include "Core/Common/FixString.h"
#include "Core/Common/Handle.h"
#include "Core/Math/Math.h"
#include "Module/Module.h"

// プラットフォームモジュールを使う
// 主にインプット入力の取得など
#include "HobbyEngine/Platform/PlatformModule.h"

// 外部モジュール
#include "HobbyPlugin/Actor/ActorModule.h"
#include "HobbyPlugin/AssetManager/AssetManagerModule.h"
#include "HobbyPlugin/Level/LevelModule.h"
#include "HobbyPlugin/Localization/LocalizationModule.h"
#include "HobbyPlugin/Render/RenderModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "Builder/UILayoutBuilder.h"
#include "Builder/UILayoutData.h"
#include "Component/Input/UIInputRouter.h"
#include "Component/Input/UIInputTerminalTouch.h"
#include "Component/Widget/UIButton.h"
#include "Component/Widget/UILayer.h"
#include "Component/Widget/UIText.h"
#include "Widget.h"

namespace UI
{
    /// <summary>
    /// UI用の追加モジュール
    /// </summary>
    class UIModule final : public Module::ModuleBase
    {
    public:
        UIModule(const Char* in_pName) : ModuleBase(in_pName) {}

        /// <summary>
        /// BuildFileをロード
        /// </summary>
        /// <param name="in_rFilePath"></param>
        /// <returns></returns>
        Core::Common::Handle LoadAssetWithLayoutBuild(const Core::File::Path& in_rFilePath);

        /// <summary>
        /// TODO: ロードしたBuildFileのアセットを解放
        /// </summary>
        /// <param name="in_rHandle"></param>
        void UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle);

        /// <summary>
        /// TODO: アセットデータからレイアウトを新規作成
        /// </summary>
        /// <param name="in_rHandle"></param>
        /// <returns></returns>
        Core::Common::Handle NewLayoutByLayotuAsset(Core::Common::Handle& in_rHandle,
                                                    const Uint32 in_sort);

        // UIのLayerを作成する
        // 描画ソート機能は未実装
        Core::Common::Handle NewLayer(const Core::Common::FixString128 in_name,
                                      const Uint32 in_sort);

        /// <summary>
        /// テキストラベルWidgetを作成
        /// </summary>
        /// <param name="in_bLoc">ローカライズテキスト対象フラグ</param>
        /// <returns></returns>
        Core::Common::Handle NewLabelWidget(const Core::Common::FixString128 in_name,
                                            const Uint32 in_sort, const Char* in_pLocGroupName,
                                            const Char* in_pText,
                                            const Core::Math::Rect2& in_textRect,
                                            const Uint32 in_textColor);

        /// <summary>
        /// ボタンWidget作成
        /// </summary>
        /// <param name="in_name"></param>
        /// <param name="in_sort"></param>
        /// <param name="in_rBtnRect"></param>
        /// <param name="in_btnColor"></param>
        /// <returns></returns>
        Core::Common::Handle NewButtonWidget(const Core::Common::FixString128 in_name,
                                             const Uint32 in_sort,
                                             const Core::Math::Rect2& in_rBtnRect,
                                             const Uint32 in_btnColor);

        // TODO: UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        Core::Common::Handle NewWidget(const Core::Common::FixString128 in_name,
                                       const Uint32 in_sort);

        // 親Widgetに子Widgetを追加
        const Bool AddChildWidget(Core::Common::Handle in_hParent, Core::Common::Handle in_hWidget);

        // Widgetにコンポーネント追加
        template <class T>
        Core::Common::Handle AddComponent(Core::Common::Handle in_hWidget,
                                          const Sint32 in_updateOrder)
        {
            Actor::Object* pActor = NULL;
            pActor = LEVEL_MODULE_CURRENT_LEVEL->GetActor(in_hWidget);
            E_ASSERT(pActor);

            // アクターにコンポーネント追加
            return pActor->AddComponent<T>(in_updateOrder);
        }

    protected:
        /// <summary>
        /// UIのモジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Start() final override;

        /// <summary>
        /// UIのモジュール終了
        /// </summary>
        /// <returns></returns>
        // const Bool End() final override;
        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool Release() override final;
    };

}  // namespace UI

MODULE_GENRATE_DECLARATION(UI::UIModule, UI);
