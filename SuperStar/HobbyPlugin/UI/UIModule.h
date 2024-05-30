#pragma once

#include "MiniEngine.h"
#ifdef _HOBBY_ENGINE
#include "Engine.h"
#endif

#include "Core/Common/Handle.h"
#include "Core/Common/FixString.h"
#include "Core/Math/Math.h"

#include "Module/Module.h"

// プラットフォームモジュールを使う
// 主にインプット入力の取得など
#include "HobbyEngine/Platform/PlatformModule.h"

// 外部モジュール
#include "HobbyPlugin/Actor/Actor.h"

namespace UI
{
    /// <summary>
    /// UI用の追加モジュール
    /// </summary>
    class UIModule final : public Module::ModuleBase<UIModule>
    {
    public:
        /// <summary>
        /// TODO: UIのモジュール初期化
        /// </summary>
        /// <returns></returns>
        const Bool Init() final override;

        /// <summary>
        /// TODO: UIのモジュール終了
        /// </summary>
        /// <returns></returns>
        const Bool End() final override;

        // TODO: UIのLayerを作成する
        // TODO: 描画ソート機能は未実装
        Core::Common::Handle NewLayer(const Core::Common::FixString128 in_name, const Uint32 in_sort);

        // TODO: UIのテキスト付のButtonを作成する
        Core::Common::Handle NewButtonAndText(
            const Core::Common::FixString128 in_name,
            const Uint32 in_sort,
            const Char* in_pText,
            const Core::Math::Rect2& in_rBtnRect, const Uint32 in_btnColor,
            const Core::Math::Vector2& in_textPos, const Uint32 in_textColor);

        // TODO: UIのWidgetを作成する
        // TODO: 描画ソート機能は未実装
        Core::Common::Handle NewWidget(const Core::Common::FixString128 in_name, const Uint32 in_sort);

        // TODO: 親Widgetに子Widgetを追加
        const Bool AddChildWidget(Core::Common::Handle in_hParent, Core::Common::Handle in_hWidget);

        // TODO: Widgetにコンポーネント追加
        template<class T>
        Core::Common::Handle AddComponent(Core::Common::Handle in_hWidget, const Sint32 in_updateOrder)
        {
            Actor::Object* pActor = NULL;
#ifdef _HOBBY_ENGINE
            pActor = HOBBY_CURRENT_LEVEL.GetActor(in_hWidget);
#endif
            E_ASSERT(pActor);

            // TODO: アクターにコンポーネント追加
            return pActor->AddComponent<T>(in_updateOrder);
        }
    };
}
