#include "InGameCollisionComponent.h"

#include "Actor/Actor.h"
#include "Actor/Component/TransformComponent.h"

namespace InGame
{
    Core::Common::CustomFixVector<InGameCollisionComponent*, 1024>
        InGameCollisionComponent::s_vCollision;

    InGameCollisionComponent::InGameCollisionComponent() : Actor::Component()
    {
    }

    const Bool InGameCollisionComponent::VBegin()
    {
        if (Actor::Component::VBegin() == FALSE) return FALSE;

        s_vCollision.PushBack(this);
        return TRUE;
    }

    const Bool InGameCollisionComponent::VEnd()
    {
        if (Actor::Component::VEnd() == FALSE) return FALSE;

        s_vCollision.Remove(this);
        return TRUE;
    }

    void InGameCollisionComponent::CollisionAll()
    {
        auto& vrCollisionComponent = s_vCollision;
        for (Uint32 i = 0; i < vrCollisionComponent.Size(); ++i)
        {
            auto pSelfCollision = *(vrCollisionComponent.GetPtr(i));
            Uint32 endIndex     = i;
            Uint32 startIndex   = i + 1;
            while (startIndex != endIndex)
            {
                // TODO: コリジョンチェック
                if (InGameCollisionComponent::CollisionWithComponent(pSelfCollision,
                                                                     *(vrCollisionComponent.GetPtr(
                                                                         startIndex))))
                {
                    // TODO: コリジョン発生
                }

                startIndex = (startIndex + 1) % vrCollisionComponent.Size();
            }
        }
    }

    const Bool InGameCollisionComponent::CollisionWithComponent(
        CollisionInterface* in_pSelf, InGameCollisionComponent* in_pCollisionComponent)
    {
        HE_ASSERT(in_pSelf);
        HE_ASSERT(in_pCollisionComponent);

        // TODO: 異なるコリジョンタイプでもコリジョン処理をするようにしている
        // TODO: コリジョンクラス毎にコリジョン処理を分岐
        if (HE_GENERATED_CHECK_RTTI((*in_pCollisionComponent), InGameCircleCollision2DComponent))
        {
            InGameCircleCollision2DComponent* p =
                reinterpret_cast<InGameCircleCollision2DComponent*>(in_pCollisionComponent);

            return in_pSelf->VCollision(*p);
        }

        return FALSE;
    }

    const Bool InGameCircleCollision2DComponent::VCollision(
        InGameCircleCollision2DComponent& in_rOthor)
    {
        // TODO: 2D円の衝突
        if (this->_fRadius <= 0.0f) return FALSE;

        auto pTrans = this->_pOwner->GetComponent<Actor::TransformComponent>();
        if (pTrans == NULL) return FALSE;
        auto pOthorTrans = in_rOthor._pOwner->GetComponent<Actor::TransformComponent>();
        if (pOthorTrans == NULL) return FALSE;

        auto pos      = pTrans->GetWorldPos();
        auto othorPos = pOthorTrans->GetWorldPos();

        auto len = Core::Math::Vector3::Distance(pos, othorPos);
        return (len <= (this->_fRadius + in_rOthor._fRadius));
    }

}  // namespace InGame
