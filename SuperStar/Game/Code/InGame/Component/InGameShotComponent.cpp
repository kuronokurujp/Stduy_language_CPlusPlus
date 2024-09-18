#include "InGameShotComponent.h"

#include "Actor/Actor.h"

namespace InGame
{
    void InGameShotComponent::VUpdate(const Float32 in_fDt)
    {
        Actor::Component::VUpdate(in_fDt);

        // 弾を打つ
        while (0 < this->_sCatchShot.Size())
        {
            this->_spStrategy->VShot(this->_sCatchShot.PopBack());
        }

        this->_sCatchShot.Clear();
    }

    void InGameShotComponent::Shot(const Core::Math::Vector2& in_rPos)
    {
        HE_ASSERT(this->_spStrategy);

        // 利用するデータを確保
        auto pConfig = this->_sCatchShot.PushBack();

        // 確保したデータ設定
        pConfig->pos = in_rPos;
        this->_spStrategy->VConfigure(pConfig);
    }

}  // namespace InGame
