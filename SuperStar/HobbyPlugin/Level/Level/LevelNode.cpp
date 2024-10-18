#include "LevelNode.h"

#include "Level/Component/LevelBaseComponent.h"

namespace Level
{
    /// <summary>
    /// レベルノードにつけるアクター管理のデコレーター
    /// </summary>
    class ActorManagerDecorater final : public Actor::ActorManagerDecoraterlnterface
    {
    public:
        Bool VStart(Actor::ActorManager*) override final
        {
            this->_lstInputComponent.Clear();

            return TRUE;
        }

        void VLateUpdate(const Float32 in_fDt, Actor::ActorManager*) override final {}

        /// <summary>
        /// 管理下にあるアクターに入力状態を送信
        /// </summary>
        void ProcessInput(const EnhancedInput::InputMap* in_pInputMap)
        {
            HE_ASSERT(in_pInputMap);

            const void* pInputMap = reinterpret_cast<const void*>(in_pInputMap);
            for (auto it = this->_lstInputComponent.BeginItr();
                 it != this->_lstInputComponent.EndItr(); ++it)
            {
                it->ProcessInput(pInputMap);
            }
        }

        /// <summary>
        /// 入力コンポーネントの登録・解除
        /// </summary>
        void VOnActorRegistComponent(Actor::Component* in_pComponent) override final
        {
            HE_ASSERT(in_pComponent);

            if (in_pComponent->VGetRTTI().DerivesFrom(&Actor::InputComponent::CLASS_RTTI) == FALSE)
                return;

            auto pInputComponent = reinterpret_cast<Actor::InputComponent*>(in_pComponent);
            this->_lstInputComponent.PushBack(*pInputComponent);
        }

        void VOnActorUnRegistComponent(Actor::Component* in_pComponent) override final
        {
            HE_ASSERT(in_pComponent);
            if (in_pComponent->VGetRTTI().DerivesFrom(&Actor::InputComponent::CLASS_RTTI) == FALSE)
                return;

            auto pInputComponent = reinterpret_cast<Actor::InputComponent*>(in_pComponent);
            this->_lstInputComponent.Erase(pInputComponent);
        }

    private:
        Core::Common::CustomList<Actor::InputComponent> _lstInputComponent;
    };

    Node::Node() : Actor::Object(), _actorManager(HE_MAKE_CUSTOM_UNIQUE_PTR(ActorManagerDecorater))
    {
    }

    Bool Node::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        if (this->_actorManager.Start(256, 2) == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードのアクター管理の初期化が失敗");
            return FALSE;
        }

        return TRUE;
    }

    Bool Node::VEnd()
    {
        if (this->_actorManager.End() == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードの終了に失敗");
            return FALSE;
        }

        return Actor::Object::VEnd();
    }

    void Node::VUpdate(const Float32 in_fDt)
    {
        Actor::Object::VUpdate(in_fDt);

        this->_actorManager.BeginUpdate(in_fDt);
        {
            this->_actorManager.Update(in_fDt);

            // コリジョン処理(コリジョンしてActor追加が起きてもpendingするように)
            // this->_Colision();
            {
        /*                for (Uint32 i = 0; i < this->_uiCollisionArray.Size(); ++i)
                        {

                        }
         */           }
        }
        this->_actorManager.LateUpdate(in_fDt);
    }

    void Node::VEvent(const Core::TaskData& in_rTaskData)
    {
        Actor::Object::VEvent(in_rTaskData);

        switch (in_rTaskData.uId)
        {
            // 入力送信
            case Node::ETaskUpdateId_Input:
            {
                HE_ASSERT(in_rTaskData.pData);
                const auto pInputMap =
                    reinterpret_cast<EnhancedInput::InputMap*>(in_rTaskData.pData);

                this->_VProcessInput(pInputMap);

                break;
            }
        }
    }

    // レベルに追加されたアクターを削除
    void Node::RemoveActor(Core::Common::Handle* in_pActor)
    {
        HE_ASSERT(in_pActor);
        if (in_pActor->Null()) return;

        this->_actorManager.Remove(in_pActor);
    }

    Bool Node::ChainActor(const Core::Common::Handle& in_rChildActor,
                          const Core::Common::Handle& in_rParentActor)
    {
        HE_ASSERT(in_rChildActor.Null() == FALSE);
        HE_ASSERT(in_rParentActor.Null() == FALSE);

        auto pParentActor = this->GetActor<Actor::Object>(in_rParentActor);
        // 子アクターに親アクターを設定する
        return pParentActor->AddChildTask(in_rChildActor);
    }

    void Node::_VProcessInput(const EnhancedInput::InputMap* in_pInputMap)
    {
        ActorManagerDecorater* pDecotrater =
            reinterpret_cast<ActorManagerDecorater*>(this->_actorManager.GetDecorater());
        pDecotrater->ProcessInput(in_pInputMap);

        // this->_actorManagerDecorater.ProcessInput(in_pInputMap);
    }

    /// <summary>
    /// 追加したコンポーネントのセットアップ
    /// </summary>
    Bool Node::_VSetupComponent(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);

        auto c = this->GetComponent<Actor::Component>(in_rHandle);

        if (HE_GENERATED_CHECK_RTTI((*c), LevelBaseComponent) == FALSE)
        {
            HE_ASSERT(0 && "レベル専用のコンポーネントではない");
            return FALSE;
        }

        return Actor::Object::_VSetupComponent(in_rHandle);
    }

}  // namespace Level
