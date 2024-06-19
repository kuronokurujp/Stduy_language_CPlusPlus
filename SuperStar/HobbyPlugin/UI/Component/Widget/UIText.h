#pragma once

#include "Core/Common/FixString.h"
#include "Core/Math/Vector2.h"

#include "UIWidget.h"

namespace UI
{
    /// <summary>
    /// UIに表示するテキストコンポーネント
    /// </summary>
    class UITextComponent : public UIWidgetComponent
    {
        GENERATED_CLASS_BODY_HEADER();

    public:
        UITextComponent() : UIWidgetComponent() { this->_Clear(); }

        /// <summary>
        /// タスク利用した設定をした最初に実行
        /// 登録に必要な情報を設定
        /// </summary>
        /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
        virtual void Init(const Bool in_bAutoDelete) override
        {
            UIWidgetComponent::Init(in_bAutoDelete);

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

    private:
        void _Clear()
        {
            this->_str.Empty();
        }

    private:
        Core::Common::FixString1024 _str;
    };
}
