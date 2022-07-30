#include "Actor/ActorMiniHeader.h"

#if USE_COMPONENT
#include "Component/component.h"
#endif

#include "Common.h"

namespace Actor
{
    Actor::Actor()
    {
        this->_Clear();
    }

    Actor::~Actor()
    {
        // くっついているコンポーネントをすべて破棄する
        this->RemoveComponentAndMemFree();
    }

    void Actor::Update(float in_deltaTimeSecond)
    {
        if (this->_state != eState::eState_Active)
        {
            return;
        }

#if USE_COMPONENT
        // くっつているコンポーネント更新
        for (auto component : this->_components)
        {
            component->Update(in_deltaTimeSecond);
        }
#endif

        // コンポーネントがすべて更新してから実行
        // コンポーネントの結果が同フレーム取れる
        this->UpdateActor(in_deltaTimeSecond);
    }

    void Actor::AddComponentMemData(Component* in_pComponent)
    {
#if USE_COMPONENT
        auto iter = this->_components.begin();
        this->_components.insert(iter, in_pComponent);
#endif
    }

    bool Actor::RemoveComponentAndMemFree(Component* in_pComponent)
    {
#if USE_COMPONENT

        if (in_pComponent == NULL)
        {
            return false;
        }

        auto iter = std::find(
            this->_components.begin(),
            this->_components.end(),
            in_pComponent);
        if (iter == this->_components.end())
        {
            return false;
        }

        this->_components.erase(iter);
        SAFETY_MEM_RELEASE(in_pComponent);

        return true;
#else
        return true;
#endif
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    /// <returns></returns>
    void Actor::RemoveComponentAndMemFree()
    {
#if USE_COMPONENT
        for (auto component : this->_components)
            SAFETY_MEM_RELEASE(component);
#endif
        this->_components.clear();
    }

    void Actor::_Clear()
    {
        this->_state = eState::eState_Active;
        this->_components.clear();
    }
}
