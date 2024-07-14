#pragma once

#include "Core/Common/FixString.h"
#include "Core/Math/Rect2.h"
#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIに表示するテキストコンポーネント
    /// </summary>
    class UITextComponent : public UIWidgetComponent
    {
        GENERATED_CLASS_BODY_HEADER(UITextComponent, UIWidgetComponent);

    public:
        UITextComponent() : UIWidgetComponent() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Setup(const Bool in_bAutoDelete) override
        {
            UIWidgetComponent::Setup(in_bAutoDelete);

            this->_Clear();
        }

        /// <summary>
        /// コンポーネントの更新
        /// 必ず処理を記述
        /// </summary>
        /// <param name="in_deltaTime">The in delta time.</param>
        void Update(const Float32 in_fDeltaTime) final override;

        /// <summary>
        /// テキスト設定
        /// </summary>
        /// <param name="in_pText"></param>
        void SetText(const Char* in_szText) { this->_szText = in_szText; }

        /// <summary>
        /// ローカライズグループ名
        /// </summary>
        /// <param name="in_pName"></param>
        void SetLocGroupName(const Char* in_szName) { this->_szLocGroup = in_szName; }

        /// <summary>
        /// テキストの描画矩形を設定
        /// </summary>
        /// <param name="in_rect"></param>
        void SetRect(const Core::Math::Rect2& in_rRect) { this->_rect = in_rRect; }

        /// <summary>
        /// ローカライズグループ名を設定
        /// 文字列がある == ローカライズテキスト
        /// </summary>
        /// <param name="in_pGroupName"></param>
        void SetLogGroup(const Char* in_szGroupName) { this->_szLocGroup = in_szGroupName; }

    private:
        void _Clear() { this->_szText.Clear(); }

    private:
        Core::Common::FixString1024 _szText;
        Core::Common::FixString128 _szLocGroup;
        Core::Math::Rect2 _rect;
    };
}  // namespace UI
