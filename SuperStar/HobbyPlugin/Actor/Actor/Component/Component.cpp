﻿#include "Component.h"

#include "Actor/Actor.h"

namespace Actor
{
    Component::Component() : Task()
    {
        this->_Clear();
    }

    Component::~Component()
    {
    }

    /// <summary>
    /// タスク利用した設定をした最初に実行
    /// 登録に必要な情報を設定
    /// </summary>
    /// <param name="bAutoDelete">TRUEだとタスク破棄と同時に削除
    void Component::Setup(const Bool in_bAutoDelete)
    {
        HE_LOG_LINE(HE_STR_TEXT("初期化したコンポーネントは(") HE_STR_FORMAT_TEXT HE_STR_TEXT(")"),
                    GetRTTI().GetName().Str());

        Core::Task::Setup(in_bAutoDelete);

        this->_Clear();
    }

    const Bool Component::End()
    {
        this->_Clear();

        return TRUE;
    }

    void Component::TransformLocalToWorldPos2D(Core::Math::Vector2* out,
                                               const Core::Math::Vector2& in_rOffsetPos)
    {
        HE_ASSERT(out != NULL);

        const Core::Math::Vector3& pos = this->_pOwner->GetWorldPos();
        out->_fX                       = pos._fX;
        out->_fY                       = pos._fY;

        (*out) += in_rOffsetPos;
    }

    void Component::TransformLocalToWorldRect2D(Core::Math::Rect2* out,
                                                const Core::Math::Rect2& in_rOffsetRect)
    {
        HE_ASSERT(out != NULL);

        (*out) = in_rOffsetRect;

        const Core::Math::Vector3& pos = this->_pOwner->GetWorldPos();
        (*out) += Core::Math::Vector2(pos._fX, pos._fY);
    }
}  // namespace Actor
