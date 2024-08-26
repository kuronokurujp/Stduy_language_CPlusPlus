﻿#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/Math/Rect2.h"
#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIに表示するテキストコンポーネント
    /// </summary>
    class UITextComponent : public UIWidgetComponent
    {
        HE_CLASS_COPY_NG(UITextComponent);
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
        void Update(const Float32 in_fDeltaTime) override;

        /// <summary>
        /// テキスト設定
        /// </summary>
        inline void SetText(const Char* in_szText) { this->_szText = in_szText; }

        /// <summary>
        /// ローカライズグループ名
        /// </summary>
        inline void SetLocGroupName(const Char* in_szName) { this->_szLocGroup = in_szName; }

        /// <summary>
        /// テキストの描画矩形を設定
        /// </summary>
        inline void SetRect(const Core::Math::Rect2& in_rRect) { this->_rect = in_rRect; }

        /// <summary>
        /// ローカライズグループ名を設定
        /// 文字列がある == ローカライズテキスト
        /// </summary>
        inline void SetLogGroup(const Char* in_szGroupName) { this->_szLocGroup = in_szGroupName; }

        inline void SetAnchor(const Core::Math::Rect2::EAnchor in_eAnchor)
        {
            this->_eAnchor = in_eAnchor;
        }

        inline void SetFontSize(const Uint32 in_fontSize) { this->_fontSize = in_fontSize; }

    private:
        void _Clear()
        {
            UIWidgetComponent::_Clear();

            this->_szText.Clear();
            this->_szDrawText.Clear();
            this->_szLocGroup.Clear();
            this->_rect.Clear();
            this->_eAnchor = Core::Math::Rect2::EAnchor_Left;
        }

    private:
        Core::Common::FixString1024 _szText;
        Core::Common::FixString1024 _szDrawText;
        Core::Common::FixString128 _szLocGroup;
        Core::Math::Rect2 _rect;
        Core::Math::Rect2::EAnchor _eAnchor = Core::Math::Rect2::EAnchor_Left;
        Uint32 _fontSize                    = 32;
    };
}  // namespace UI
