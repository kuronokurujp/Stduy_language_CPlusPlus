#pragma once

#include "Engine/Math/Math.h"
#include "Engine/MiniEngine.h"
#include "Engine/Module/Module.h"

// プラットフォームモジュールを使う
// 主にインプット入力の取得など
#include "ActorModule.h"
#include "LocalizationModule.h"
#include "RenderModule.h"

// モジュールのヘッダーファイルは全てインクルードする
#include "UI/Builder/UILayoutBuilder.h"
#include "UI/Builder/UILayoutData.h"
#include "UI/Component/Input/UIInputRouter.h"
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
        HE_MODULE_GENRATE_DECLARATION(UIModule);

    public:
        UIModule();

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
        Core::Common::Handle NewLayoutByLayotuAsset(const Core::Common::Handle& in_rHandle,
                                                    const Uint32 in_sort,
                                                    const Core::Common::Handle& in_rViewHandle);

        // UIのLayerを作成する
        // 描画ソート機能は未実装
        Core::Common::Handle NewLayer(const Core::Common::StringBase& in_szrName,
                                      const Uint32 in_sort);

        /// <summary>
        /// テキストラベルWidgetを作成
        /// </summary>
        /// <param name="in_bLoc">ローカライズテキスト対象フラグ</param>
        /// <returns></returns>
        Core::Common::Handle NewLabelWidget(const Core::Common::Handle& in_rViewHandle,
                                            const Core::Common::StringBase& in_szrName,
                                            const Uint32 in_sort, const Char* in_pLocGroupName,
                                            const Char* in_szText,
                                            const Core::Math::Rect2& in_rTextRect,
                                            const Uint32 in_uTextColor);

        /// <summary>
        /// ボタンWidget作成
        /// </summary>
        Core::Common::Handle NewButtonWidget(const Core::Common::Handle& in_rViewHandle,
                                             const Core::Common::StringBase& in_szrName,
                                             const Uint32 in_sort,
                                             const Core::Math::Rect2& in_rBtnRect,
                                             const Uint32 in_uBtnColor);

        // UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        Core::Common::Handle NewWidget(const Core::Common::StringBase& in_szName,
                                       const Uint32 in_uSort);

        // 親Widgetに子Widgetを追加
        const Bool AddChildWidget(Core::Common::Handle& in_rParent,
                                  Core::Common::Handle& in_rWidget);

        // Widgetにコンポーネント追加
        template <class T>
        Core::Common::Handle AddComponent(Core::Common::Handle& in_rWidget,
                                          const Sint32 in_iUpdateOrder)
        {
            auto pActor = this->GetWidget(in_rWidget);
            // アクターにコンポーネント追加
            return pActor->AddComponent<T>(in_iUpdateOrder);
        }

        Widget* GetWidget(const Core::Common::Handle&);

    protected:
        /// <summary>
        /// UIのモジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        virtual const Bool _VRelease() override final;
    };

}  // namespace UI
