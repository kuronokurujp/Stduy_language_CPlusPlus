#include "Component.h"

#include "../Actor.h"

namespace Actor
{
    GENERATED_CLASS_BASE_BODY(Component)

    /// <summary>
    /// Finalizes an instance of the <see cref="Component"/> class.
    /// </summary>
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
    void Component::Init(const Bool in_bAutoDelete)
    {
        E_LOG(E_STR_TEXT("初期化したコンポーネントは(%s)"), GetRTTI().GetName().Str());

        Core::Task::Init(in_bAutoDelete);

        this->_Clear();
    }

    void Component::TransformLocalToWorldPos2D(
        Core::Math::Vector2* out_pPos, const Core::Math::Vector2& in_offsetPos)
    {
        E_ASSERT(out_pPos != NULL);

        const Core::Math::Vector3& pos = this->_pOwner->GetWorldPos();
        out_pPos->x = pos.x;
        out_pPos->y = pos.y;

        (*out_pPos) += in_offsetPos;
    }

    void Component::TransformLocalToWorldRect2D(
        Core::Math::Rect2* out_pRect, const Core::Math::Rect2& in_offsetRect)
    {
        E_ASSERT(out_pRect != NULL);

        (*out_pRect) = in_offsetRect;

        const Core::Math::Vector3& pos = this->_pOwner->GetWorldPos();
        (*out_pRect) += Core::Math::Vector2(pos.x, pos.y);
    }
}

