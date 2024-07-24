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

    void Manager::Update(const Float32 in_fDt, Platform::InputSystemInterface* in_pInput)
    {
        HE_ASSERT(in_pInput != NULL);

        // 入力更新
        {
            Core::TaskData taskData{Node::ETaskUpdateId_Input, reinterpret_cast<void*>(in_pInput)};

            this->_nodeManager.Update(in_fDt, taskData);
        }

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
    }

    // ここから先は
    // レベルノードの実装

    void Node::Setup(const Bool in_bAutoDelete)
    {
        Task::Setup(in_bAutoDelete);
    }

    const Bool Node::Begin()
    {
        //Actor::ActorManager* pActorManager = HE_NEW(Actor::ActorManager, 0);
        //this->_pActorManager               = std::shared_ptr<Actor::ActorManager>(pActorManager);
        this->_pActorManager               = Core::Memory::MakeCustomShared<Actor::ActorManager>();
        if (this->_pActorManager->Start(256, 32) == FALSE)
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
            // TODO: 入力送信
            case Node::ETaskUpdateId_Input:
            {
                Platform::InputSystemInterface* pInput =
                    reinterpret_cast<Platform::InputSystemInterface*>(in_rData.pData);
                HE_ASSERT(pInput != NULL);

                this->_pActorManager->ProcessInput(in_fDt, pInput);

                break;
            }

            // TODO: 更新
            case Node::ETaskUpdateId_Actor:
            {
                // Actorの制御
                {
                    Core::TaskData taskData{Actor::Object::ETaskUpdateId_Object, NULL};
                    this->_pActorManager->Update(in_fDt, taskData);

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

    // TODO: レベルに追加されたアクターを削除
    void Node::RemoveActor(const Core::Common::Handle& in_rActor)
    {
        if (in_rActor.Null()) return;

        this->_pActorManager->Remove(in_rActor);
    }

    Actor::Object* Node::GetActor(const Core::Common::Handle& in_rActor)
    {
        HE_ASSERT(in_rActor.Null() == FALSE);
        return this->_pActorManager->Get(in_rActor);
    }

    const Bool Node::AddParentActor(const Core::Common::Handle& in_rChildActor,
                                    const Core::Common::Handle& in_rParentActor)
    {
        HE_ASSERT(in_rChildActor.Null() == FALSE);
        HE_ASSERT(in_rParentActor.Null() == FALSE);

        Actor::Object* pChildActor  = this->GetActor(in_rChildActor);
        Actor::Object* pParentActor = this->GetActor(in_rParentActor);
        // 子アクターに親アクターを設定する
        const Bool bRet = pChildActor->SetParentActor(in_rParentActor, pParentActor->GetDepth());
        if (bRet)
        {
            // 子アクターの更新順を変える
            this->_pActorManager->MoveDepth(in_rChildActor, pChildActor->GetDepth());

            pParentActor->OnAddChildActor(pChildActor);
        }

        return TRUE;
    }

    /*
        Actor::ActorManagerlnterface* Node::GetActorManagerDataAccess()
        {
            return this->_pActorManager.get();
        }
        */
}  // namespace Level
