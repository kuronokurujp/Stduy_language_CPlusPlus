#pragma once

#include <vector>
#include <stdio.h>
#include <memory>

// 前方宣言
namespace Component { class Component; }

namespace Actor
{
    /// <summary>
    /// ゲームアクター
    /// </summary>
    class Actor
    {
    public:
        /// <summary>
        /// Actorの状態
        /// </summary>
        enum class eState
        {
            eState_Begin = 0,
            eState_Active,
            eState_Pause,
            eState_End,
            eState_Dead,
        };

        Actor();
        virtual ~Actor();

        virtual void Begin() {}
        virtual void End() { this->SetState(eState::eState_Dead); }

        void Tick(float in_deltaTimeSecond);
        virtual void TickImplement(float in_deltaTimeSecond) {}

        // レンダリングする
        virtual void Render() {}

        /// <summary>
        /// コンポーネントを追加.
        /// </summary>
        void AddComponentMemData(std::shared_ptr<Component::Component> in_component);

        /// <summary>
        /// くっつている全てのコンポーネント削除.
        /// </summary>
        /// <returns></returns>
        void RemoveComponentAndMemFree();

        /// <summary>
        /// 引数で指定したコンポーネントを外す.
        /// </summary>
        bool RemoveComponentAndMemFree(std::shared_ptr<Component::Component> in_component);

        template<class T>
        void RemoveComponentClassNameAndMemFree()
        {
            for (auto iter : this->_components)
            {
                if (dynamic_cast<T*>(iter) != NULL)
                {
                    SafeRemoveObject(iter, this->_components);
                    SAFETY_MEM_RELEASE(iter);
                    return;
                }
            }
        }

        const eState GetState() const { return this->_state; }
        void SetState(const eState in_state) { this->_state = in_state; }

        // テンプレートで指定したコンポーネント取得.
        // ポインター型限定.
        // TODO: できればポインター型限定のみしか受け付けない制約がつけれないかな
        template <class T> T GetComponent()
        {
            for (unsigned int i = 0; i < this->_components.size(); ++i)
            {
                Component::Component* pComponent = this->_components[i];
                T t = dynamic_cast<T>(pComponent);
                if (t != NULL)
                {
                    return t;
                }
            }

            return NULL;
        }

    private:
        void _Clear();

        // Actorの状態
        eState _state = eState::eState_Begin;

        // Actorにつけるコンポーネントリスト
        // ポインターにしているのはポインターからリストの要素を削除するため
        std::vector<std::shared_ptr<Component::Component>> _components;
    };
}
