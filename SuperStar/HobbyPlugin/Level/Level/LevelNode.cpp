#include "LevelNode.h"

#include "Level/Component/LevelBaseComponent.h"

namespace Level
{
    const Bool Node::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        if (this->_actorManager.Start(256, 2) == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードのアクター管理の初期化が失敗");
            return FALSE;
        }

        return TRUE;
    }

    const Bool Node::VEnd()
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

    const Bool Node::ChainActor(const Core::Common::Handle& in_rChildActor,
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
        this->_actorManagerDecorater.ProcessInput(in_pInputMap);
    }

    /// <summary>
    /// 追加したコンポーネントのセットアップ
    /// </summary>
    const Bool Node::_VSetupComponent(const Core::Common::Handle& in_rHandle)
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

    const Bool Node::ActorMaanagerDecorater::VStart(Actor::ActorManager*)
    {
        this->_lstInputComponent.Clear();

        return TRUE;
    }

    void Node::ActorMaanagerDecorater::ProcessInput(const EnhancedInput::InputMap* in_pInputMap)
    {
        HE_ASSERT(in_pInputMap);

        const void* pInputMap = reinterpret_cast<const void*>(in_pInputMap);
        for (auto it = this->_lstInputComponent.BeginItr(); it != this->_lstInputComponent.EndItr();
             ++it)
        {
            it->VProcessInput(pInputMap);
        }
    }

    void Node::ActorMaanagerDecorater::VOnActorRegistComponent(Actor::Component* in_rComponent)
    {
        HE_ASSERT(in_rComponent);

        if (in_rComponent->VGetRTTI().DerivesFrom(&Actor::InputComponent::CLASS_RTTI) == FALSE)
            return;

        auto pInputComponent = reinterpret_cast<Actor::InputComponent*>(in_rComponent);
        this->_lstInputComponent.PushBack(*pInputComponent);
    }

    void Node::ActorMaanagerDecorater::VOnActorUnRegistComponent(Actor::Component* in_rComponent)
    {
        HE_ASSERT(in_rComponent);
        if (in_rComponent->VGetRTTI().DerivesFrom(&Actor::InputComponent::CLASS_RTTI) == FALSE)
            return;

        auto pInputComponent = reinterpret_cast<Actor::InputComponent*>(in_rComponent);
        this->_lstInputComponent.Erase(pInputComponent);
    }

}  // namespace Level
