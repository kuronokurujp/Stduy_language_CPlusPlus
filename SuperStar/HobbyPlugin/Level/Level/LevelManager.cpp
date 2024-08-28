﻿#include "LevelManager.h"

#include "ActorModule.h"
#include "Engine/Memory/Memory.h"

namespace Level
{
    const Bool Manager::Init()
    {
        // タスク管理を初期化
        // 利用するタスク設定は内部で固定する
        if (this->_nodeManager.Start(32, 2) == FALSE) return FALSE;

        return TRUE;
    }

    const Bool Manager::Release()
    {
        if (this->_nodeManager.End() == FALSE) return FALSE;

        return TRUE;
    }

    void Manager::ProcessInput(const Float32 in_fDt, const EnhancedInput::InputMap& in_rInputMap)
    {
        // 入力更新
        {
            EnhancedInput::InputMap& rInputMap = const_cast<EnhancedInput::InputMap&>(in_rInputMap);
            Core::TaskData taskData{Node::ETaskUpdateId_Input, reinterpret_cast<void*>(&rInputMap)};

            this->_nodeManager.Update(in_fDt, taskData);
        }
    }

    void Manager::BeforeUpdate(const Float32 in_fDt)
    {
        // レベルの切り替え先があればカレントと交換
        // 切り替えた後は古いカレントは破棄
        if ((this->_nextLevelHandle.Null() == FALSE))
        {
            auto pNode = reinterpret_cast<Node*>(this->_nodeManager.Get(this->_nextLevelHandle));
            HE_ASSERT(pNode);
            pNode->SetState(Actor::Object::EState::EState_Active);

            if (this->_currentLevelHandle.Null() == FALSE)
                this->_nodeManager.Remove(&this->_currentLevelHandle);

            this->_currentLevelHandle = this->_nextLevelHandle;
            this->_nextLevelHandle.Clear();
        }

        this->_nodeManager.BeginUpdate(in_fDt);
    }

    void Manager::Update(const Float32 in_fDt)
    {
        // アクター更新
        // コンポーネントの更新もやっている
        {
            Core::TaskData taskData{
                Node::ETaskUpdateId_Actor,
                NULL,
            };

            this->_nodeManager.Update(in_fDt, taskData);
        }
    }

    void Manager::LateUpdate(const Float32 in_fDt)
    {
        this->_nodeManager.LateUpdate(in_fDt);
    }

    /// <summary>
    /// カレントレベルを取得
    /// </summary>
    /// <returns></returns>
    Level::Node* Manager::CurrentLevel()
    {
        Level::Node* pNode =
            reinterpret_cast<Level::Node*>(this->_nodeManager.Get(this->_currentLevelHandle));
        return pNode;
    }

    const Bool Manager::_StartLevel(const Core::Common::Handle& in_rHandle)
    {
        this->_nextLevelHandle = in_rHandle;

        auto pNode = reinterpret_cast<Node*>(this->_nodeManager.Get(this->_nextLevelHandle));
        HE_ASSERT(pNode);
        if (pNode == NULL) return FALSE;

        // 切り替えた後にアクティブにする
        pNode->SetState(Actor::Object::EState::EState_Paused);

        return TRUE;
    }

    // ここから先は
    // レベルノードの実装

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
        if (Actor::Object::VEnd() == FALSE) return FALSE;

        if (this->_actorManager.End() == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードの終了に失敗");
        }

        return TRUE;
    }

    void Node::VUpdate(const Float32 in_fDt, const Core::TaskData& in_rData)
    {
        Actor::Object::VUpdate(in_fDt, in_rData);

        switch (in_rData.uId)
        {
            // 入力送信
            case Node::ETaskUpdateId_Input:
            {
                HE_ASSERT(in_rData.pData);
                const auto pInputMap = reinterpret_cast<EnhancedInput::InputMap*>(in_rData.pData);

                this->_VProcessInput(in_fDt, pInputMap);

                break;
            }

            // 更新
            case Node::ETaskUpdateId_Actor:
            {
                // Actorの制御
                this->_actorManager.BeginUpdate(in_fDt);
                {
                    this->_actorManager.Update(in_fDt, DEFAULT_TASK_DATA);

                    // コリジョン処理(コリジョンしてActor追加が起きてもpendingするように)
                    // this->_Colision();
                    {
        /*                for (Uint32 i = 0; i < this->_uiCollisionArray.Size(); ++i)
                        {

                        }
         */           }
                }
                this->_actorManager.LateUpdate(in_fDt);

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

    Actor::Object* Node::GetActor(const Core::Common::Handle& in_rActor)
    {
        HE_ASSERT(in_rActor.Null() == FALSE);
        return this->_actorManager.Get(in_rActor);
    }

    const Bool Node::ChainActor(const Core::Common::Handle& in_rChildActor,
                                const Core::Common::Handle& in_rParentActor)
    {
        HE_ASSERT(in_rChildActor.Null() == FALSE);
        HE_ASSERT(in_rParentActor.Null() == FALSE);

        Actor::Object* pParentActor = this->GetActor(in_rParentActor);
        // 子アクターに親アクターを設定する
        return pParentActor->AddChildTask(in_rChildActor);
    }

    void Node::_VProcessInput(const Float32 in_fDt, const EnhancedInput::InputMap* in_pInputMap)
    {
        this->_actorManagerDecorater.ProcessInput(in_fDt, in_pInputMap);
    }

    void Node::ActorMaanagerDecorater::ProcessInput(const Float32 in_fDt,
                                                    const EnhancedInput::InputMap* in_pInputMap)
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
