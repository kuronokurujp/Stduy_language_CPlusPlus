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
    /// widget用のハンドル
    /// </summary>
    class UIWidgetHandlePack
    {
    public:
        UIWidgetHandlePack()
        {
            this->_levelHandle.Clear();
            this->_widgetHandle.Clear();
        }

        UIWidgetHandlePack(const Core::Common::Handle& in_rWidgetHandle,
                           const Core::Common::Handle& in_rLevelHandle)
        {
            this->_widgetHandle = in_rWidgetHandle;
            this->_levelHandle  = in_rLevelHandle;
        }

        Core::Common::Handle _levelHandle;
        Core::Common::Handle _widgetHandle;
    };

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
        /// ロードしたBuildFileのアセットを解放
        /// </summary>
        void UnloadAssetWithLayoutBuild(Core::Common::Handle& in_rHandle);

        /// <summary>
        /// アセットデータからレイアウトを新規作成
        /// </summary>
        const UIWidgetHandlePack NewLayoutByLayotuAsset(
            const Core::Common::Handle& in_rAssetHandle, const Uint32 in_sort,
            const Core::Common::Handle& in_rViewHandle,
            const Core::Common::Handle& in_rLevelHandle);

        // UIのLayerを作成する
        // 描画ソート機能は未実装
        const UIWidgetHandlePack NewLayer(const Core::Common::StringBase& in_szrName,
                                          const Uint32 in_sort,
                                          const Core::Common::Handle& in_rLevelHandle);

        /// <summary>
        /// テキストラベルWidgetを作成
        /// </summary>
        /// <param name="in_bLoc">ローカライズテキスト対象フラグ</param>
        /// <returns></returns>
        const UIWidgetHandlePack NewLabelWidget(const Core::Common::StringBase& in_szrName,
                                                const Uint32 in_sort, const Char* in_pLocGroupName,
                                                const Char* in_szText,
                                                const Core::Math::Rect2& in_rTextRect,
                                                const Uint32 in_uTextColor,
                                                const Core::Common::Handle& in_rViewHandle,
                                                const Core::Common::Handle& in_rLevelHandle);

        /// <summary>
        /// ボタンWidget作成
        /// </summary>
        const UIWidgetHandlePack NewButtonWidget(const Core::Common::StringBase& in_szrName,
                                                 const Uint32 in_sort,
                                                 const Core::Math::Rect2& in_rBtnRect,
                                                 const Uint32 in_uBtnColor,
                                                 const Core::Common::Handle& in_rViewHandle,
                                                 const Core::Common::Handle& in_rLevelHandle);

        // UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        const UIWidgetHandlePack NewWidget(const Core::Common::StringBase& in_szName,
                                           const Uint32 in_uSort,
                                           const Core::Common::Handle& in_rLevelHandle);

        // 親Widgetに子Widgetを追加
        Bool AddChildWidget(UIWidgetHandlePack& in_rParent, UIWidgetHandlePack& in_rWidget);

        // Widgetにコンポーネント追加
        template <class T>
        Core::Common::Handle AddComponent(const UIWidgetHandlePack& in_rWidgetHandle,
                                          const Sint32 in_iUpdateOrder)
        {
            auto pActor = this->GetWidget(in_rWidgetHandle);
            // アクターにコンポーネント追加
            return pActor->AddComponent<T>(in_iUpdateOrder);
        }

        Widget* GetWidget(const UIWidgetHandlePack&);

    protected:
        /// <summary>
        /// UIのモジュール初期化
        /// </summary>
        Bool _VStart() override final;

        /// <summary>
        /// インスタンス破棄時に呼ばれる
        /// </summary>
        Bool _VRelease() override final;
    };

}  // namespace UI
