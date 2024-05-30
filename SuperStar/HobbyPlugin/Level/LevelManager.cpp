#include "LevelManager.h"

#include "HobbyPlugin/Actor/Actor.h"

namespace Level
{
    const Bool Manager::Init()
    {
        // タスク管理を初期化
        // 利用するタスク設定は内部で固定する
        if (this->_taskManager.Init(32, 1) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    const Bool Manager::End()
    {
        this->_taskManager.End();
        return TRUE;
    }

    void Manager::Update(const Float32 in_dt, Platform::InputSystemInterface* in_pInput)
    {
        E_ASSERT(in_pInput != NULL);

        // 入力更新
        {
            Core::TaskData taskData
            {
                Node::ETaskUpdateId_Input,
                reinterpret_cast<void*>(in_pInput)
            };

            this->_taskManager.UpdateAll(in_dt, &taskData);
        }

        // アクター更新
        {
            Core::TaskData taskData
            {
                Node::ETaskUpdateId_Actor,
                NULL,
            };

            this->_taskManager.UpdateAll(in_dt, &taskData);
        }
    }

    // ここから先は
    // レベルノードの実装

    void Node::Init(const Bool in_bAutoDelete)
    {
        Task::Init(in_bAutoDelete);
    }

    const Bool Node::Begin()
    {
        Actor::ActorManager* pActorManager = E_NEW(Actor::ActorManager, 0);
        this->_pActorManager = std::shared_ptr<Actor::ActorManager>(pActorManager);
        if (this->_pActorManager->Init() == FALSE)
        {
            E_ASSERT(FALSE && "レベルノードのアクター管理の初期化が失敗");
            return FALSE;
        }

        return TRUE;
    }

    const Bool Node::End()
    {
        if (this->_pActorManager->End() == FALSE)
        {
            E_ASSERT(FALSE && "レベルノードの終了に失敗");
        }
        // メモリ解放をしている
        this->_pActorManager.reset();

        return TRUE;
    }

    void Node::Update(const Float32 in_dt, const Core::TaskData* in_pData)
    {
        switch (in_pData->id)
        {
            // TODO: 入力送信
            case Node::ETaskUpdateId_Input:
            {
                Platform::InputSystemInterface* pInput = reinterpret_cast<Platform::InputSystemInterface*>(in_pData->pData);
                E_ASSERT(pInput != NULL);

                this->_pActorManager->ProcessInput(in_dt, pInput);

                break;
            }

            // TODO: 更新
            case Node::ETaskUpdateId_Actor:
            {
                // Actorの制御
                {
                    this->_pActorManager->Update(in_dt);

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
    void Node::RemoveActor(const Core::Common::Handle& in_hActor)
    {
        if (in_hActor.Null())
            return;

        this->_pActorManager->Remove(in_hActor);
    }

    Actor::Object* Node::GetActor(const Core::Common::Handle& in_hActor)
    {
        E_ASSERT(in_hActor.Null() == FALSE);
        return this->_pActorManager->Get(in_hActor);
    }

    const Bool Node::AddParentActor(const Core::Common::Handle& in_hActor, const Core::Common::Handle in_hParentActor)
    {
        E_ASSERT(in_hActor.Null() == FALSE);
        E_ASSERT(in_hParentActor.Null() == FALSE);

        Actor::Object* pActor = this->GetActor(in_hActor);
        const Bool bRet = pActor->SetParentActor(in_hParentActor);
        if (bRet)
        {
            Actor::Object* pParentActor = this->GetActor(in_hParentActor);
            pParentActor->OnAddChildActor(pActor);
        }

        return TRUE;
    }

    Actor::ActorManagerlnterface* Node::GetActorManagerDataAccess()
    {
        return this->_pActorManager.get();
    }
}

