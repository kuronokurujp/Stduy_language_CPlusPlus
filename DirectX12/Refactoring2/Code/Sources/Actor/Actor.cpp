#include "Actor/ActorMiniHeader.h"

#include "Component/Component.h"
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

    void Actor::Tick(float in_deltaTimeSecond)
    {
        if (this->_state != eState::eState_Active)
        {
            return;
        }

        // くっつているコンポーネント更新
        for (auto component : this->_components)
        {
            component->Update(in_deltaTimeSecond);
        }

        // コンポーネントがすべて更新してから実行
        // コンポーネントの結果が同フレーム取れる
        this->TickImplement(in_deltaTimeSecond);
    }

    void Actor::AddComponentMemData(std::shared_ptr<Component::Component> in_component)
    {
        auto iter = this->_components.begin();
        this->_components.insert(iter, in_component);

        // TODO: ここでコンポーネントのBeginがActorの処理順序によってはバグになるのでステートを変えてActorのUpdateでBeginを呼ぶべき
        // 暫定対応
        in_component->Begin();
    }

    bool Actor::RemoveComponentAndMemFree(std::shared_ptr<Component::Component> in_component)
    {
        auto iter = std::find(
            this->_components.begin(),
            this->_components.end(),
            in_component);
        if (iter == this->_components.end())
        {
            return false;
        }

        this->_components.erase(iter);
        in_component->End();
        in_component.reset();

        return true;
    }

    /// <summary>
    /// くっつている全てのコンポーネント削除.
    /// </summary>
    /// <returns></returns>
    void Actor::RemoveComponentAndMemFree()
    {
        for (auto component : this->_components)
            component->End();

        this->_components.clear();
    }

    void Actor::_Clear()
    {
        this->_components.clear();
    }
}
