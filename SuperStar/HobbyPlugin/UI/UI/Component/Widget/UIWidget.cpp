#include "UIWidget.h"

namespace UI
{
    void UIWidgetComponent::VSetup(const Bool in_bAutoDelete)
    {
        Actor::Component::VSetup(in_bAutoDelete);
        this->_Clear();
    }

    Bool UIWidgetComponent::VBegin()
    {
        if (Component::VBegin() == FALSE) return FALSE;

        if (this->_viewHandle.Null()) return FALSE;

        this->_transformHandle = this->_pOwner->AddComponent<Actor::TransformComponent>(0);
        if (this->_transformHandle.Null()) return FALSE;

        this->GetTransformComponent()->SetPos(Core::Math::Vector3(this->_pos));

        return TRUE;
    }

    Bool UIWidgetComponent::VEnd()
    {
        if (Component::VEnd() == FALSE) return FALSE;
        if (this->_pOwner->RemoveComponent(&this->_transformHandle) == FALSE)
        {
        }

        this->_Clear();

        return TRUE;
    }

    void UIWidgetComponent::SetPos(const Core::Math::Vector2 in_pos)
    {
        this->_pos = in_pos;

        if (this->_transformHandle.Null()) return;

        auto pTrans = this->GetTransformComponent();
        if (pTrans)
        {
            pTrans->SetPos(Core::Math::Vector3(this->_pos));
        }
    }

    Actor::TransformComponent* UIWidgetComponent::GetTransformComponent()
    {
        return this->_pOwner->GetComponent<Actor::TransformComponent>(this->_transformHandle);
    }

}  // namespace UI
