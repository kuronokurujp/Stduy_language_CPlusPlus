#include "LevelManager.h"

#include "ActorModule.h"
#include "Engine/Memory/Memory.h"

namespace Level
{
    const Bool Manager::Init()
    {
        // タスク管理を初期化
        // 利用するタスク設定は内部で固定する
        if (this->_nodeManager.Start(32, 2) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    const Bool Manager::End()
    {
        this->_nodeManager.End();
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

        this->_nodeManager.UpdatePending();
        if (this->_prevLevel.Null() == FALSE)
        {
            this->_nodeManager.Remove(&this->_prevLevel);
        }
    }

    /// <summary>
    /// カレントレベルを取得
    /// </summary>
    /// <returns></returns>
    Level::Node* Manager::CurrentLevel()
    {
        Level::Node* pNode =
            reinterpret_cast<Level::Node*>(this->_nodeManager.Get(this->_currentLevel));
        return pNode;
    }

    // ここから先は
    // レベルノードの実装

    const Bool Node::Begin()
    {
        this->_pActorManager = Core::Memory::MakeCustomSharedPtr<CustomActorManager>();
        if (this->_pActorManager->Start(256, 2) == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードのアクター管理の初期化が失敗");
            return FALSE;
        }

        return TRUE;
    }

    const Bool Node::End()
    {
        if (this->_pActorManager->End() == FALSE)
        {
            HE_ASSERT(FALSE && "レベルノードの終了に失敗");
        }
        // メモリ解放をしている
        this->_pActorManager.reset();

        return TRUE;
    }

    void Node::Update(const Float32 in_fDt, const Core::TaskData& in_rData)
    {
        switch (in_rData.uId)
        {
            // 入力送信
            case Node::ETaskUpdateId_Input:
            {
                HE_ASSERT(in_rData.pData);
                const auto pInputMap = reinterpret_cast<EnhancedInput::InputMap*>(in_rData.pData);

                // レベルノードで入力処理が出来る
                this->_ProcessInput(in_fDt, pInputMap);

                this->_pActorManager->ProcessInput(in_fDt, pInputMap);

                break;
            }

            // 更新
            case Node::ETaskUpdateId_Actor:
            {
                // Actorの制御
                {
                    this->_pActorManager->Update(in_fDt, DEFAULT_TASK_DATA);

                    // コリジョン処理(コリジョンしてActor追加が起きてもpendingするように)
                    // this->_Colision();
                    {
        /*                for (Uint32 i = 0; i < this->_uiCollisionArray.Size(); ++i)
                        {

                        }
         */           }
                }

                // キャッシュに登録しているActorを更新用のリストに移行する
                this->_pActorManager->UpdatePending();

                break;
            }
        }
    }

    // レベルに追加されたアクターを削除
    void Node::RemoveActor(Core::Common::Handle* in_pActor)
    {
        HE_ASSERT(in_pActor);
        if (in_pActor->Null()) return;

        this->_pActorManager->Remove(in_pActor);
    }

    Actor::Object* Node::GetActor(const Core::Common::Handle& in_rActor)
    {
        HE_ASSERT(in_rActor.Null() == FALSE);
        return this->_pActorManager->Get(in_rActor);
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

    void Node::CustomActorManager::ProcessInput(const Float32 in_fDt,
                                                const EnhancedInput::InputMap* in_pInputMap)
    {
        HE_ASSERT(in_pInputMap);

        const void* pInputMap = reinterpret_cast<const void*>(in_pInputMap);
        for (auto it = this->_lstInputComponent.BeginItr(); it != this->_lstInputComponent.EndItr();
             ++it)
        {
            it->ProcessInput(pInputMap);
        }
    }

    void Node::CustomActorManager::RegistInputComponent(Actor::InputComponent& in_rInputComponent)
    {
        this->_lstInputComponent.PushBack(in_rInputComponent);
    }

    void Node::CustomActorManager::UnRegistInputComponent(Actor::InputComponent& in_rInputComponent)
    {
        this->_lstInputComponent.Erase(&in_rInputComponent);
    }

}  // namespace Level
