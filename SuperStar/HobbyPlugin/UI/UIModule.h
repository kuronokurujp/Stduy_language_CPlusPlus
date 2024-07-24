﻿#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Common/Handle.h"
#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// プラットフォームモジュールを使う
// 主にインプット入力の取得など
#include "Engine/Platform/PlatformModule.h"
#include "LevelModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "UI/Builder/UILayoutBuilder.h"
#include "UI/Builder/UILayoutData.h"
#include "UI/Component/Input/UIInputRouter.h"
#include "UI/Component/Input/UIInputTerminalTouch.h"
#include "UI/Component/Widget/UIButton.h"
#include "UI/Component/Widget/UILayer.h"
#include "UI/Component/Widget/UIText.h"
#include "UI/Widget.h"

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
        Core::Common::Handle NewLayer(const Core::Common::FixStringBase& in_szrName,
                                      const Uint32 in_sort);

        /// <summary>
        /// テキストラベルWidgetを作成
        /// </summary>
        /// <param name="in_bLoc">ローカライズテキスト対象フラグ</param>
        /// <returns></returns>
        Core::Common::Handle NewLabelWidget(const Core::Common::FixStringBase& in_szrName,
                                            const Uint32 in_sort, const Char* in_pLocGroupName,
                                            const Char* in_szText,
                                            const Core::Math::Rect2& in_rTextRect,
                                            const Uint32 in_uTextColor);

        /// <summary>
        /// ボタンWidget作成
        /// </summary>
        Core::Common::Handle NewButtonWidget(const Core::Common::FixStringBase& in_szrName,
                                             const Uint32 in_sort,
                                             const Core::Math::Rect2& in_rBtnRect,
                                             const Uint32 in_uBtnColor);

        // TODO: UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        Core::Common::Handle NewWidget(const Core::Common::FixStringBase& in_szName,
                                       const Uint32 in_uSort);

        // 親Widgetに子Widgetを追加
        const Bool AddChildWidget(Core::Common::Handle& in_rParent,
                                  Core::Common::Handle& in_rWidget);

        // Widgetにコンポーネント追加
        template <class T>
        Core::Common::Handle AddComponent(Core::Common::Handle& in_rWidget,
                                          const Sint32 in_iUpdateOrder)
        {
            Actor::Object* pActor = NULL;
            pActor                = LEVEL_MODULE_CURRENT_LEVEL->GetActor(in_rWidget);
            HE_ASSERT(pActor);

            // アクターにコンポーネント追加
            return pActor->AddComponent<T>(in_iUpdateOrder);
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
