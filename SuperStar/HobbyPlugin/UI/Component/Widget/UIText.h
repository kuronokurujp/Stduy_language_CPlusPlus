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
        void Update(const Float32 in_deltaTime) final override;

        /// <summary>
        /// テキスト設定
        /// </summary>
        /// <param name="in_pText"></param>
        void SetText(const Char* in_pText) { this->_str = in_pText; }

        /// <summary>
        /// ローカライズグループ名
        /// </summary>
        /// <param name="in_pName"></param>
        void SetLocGroupName(const Char* in_pName) { this->_locGroup = in_pName; }

        /// <summary>
        /// テキストの描画矩形を設定
        /// </summary>
        /// <param name="in_rect"></param>
        void SetRect(const Core::Math::Rect2& in_rect) { this->_rect = in_rect; }

        /// <summary>
        /// ローカライズグループ名を設定
        /// 文字列がある == ローカライズテキスト
        /// </summary>
        /// <param name="in_pGroupName"></param>
        void SetLogGroup(const Char* in_pGroupName) { this->_locGroup = in_pGroupName; }

    private:
        void _Clear() { this->_str.Clear(); }

    private:
        Core::Common::FixString1024 _str;
        Core::Common::FixString128 _locGroup;
        Core::Math::Rect2 _rect;
    };
}  // namespace UI
