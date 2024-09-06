#include "LevelBaseComponent.h"

namespace Level
{
    void LevelBaseComponent::RemoveActor(Core::Common::Handle* in_pHandle)
    {
        HE_ASSERT(in_pHandle);

        auto pNode = reinterpret_cast<Node*>(this->_pOwner);
        HE_ASSERT(pNode);

        pNode->RemoveActor(in_pHandle);
    }

}  // namespace Level
