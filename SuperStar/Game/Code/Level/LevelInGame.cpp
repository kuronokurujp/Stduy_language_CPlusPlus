#include "LevelInGame.h"

#include "InGame/Component/InGameCollisionComponent.h"
#include "InGame/Component/InGameShotManagerComponent.h"
#include "InGame/Component/InGameStageManagerComponent.h"
#include "InGame/Component/InGameSystemComponent.h"
#include "LevelInGame/LevelInGame_BG.h"
#include "RenderModule.h"

namespace Level
{

    LevelInGame::LevelInGame() : Level::Node()
    {
    }

    const Bool LevelInGame::VBegin()
    {
        const Bool bRet = Node::VBegin();
        HE_ASSERT(bRet);

        // 背景のレベル追加
        this->AddLevel<LevelInGame_BG>();

        // レンダリングビュー作成
        {
            auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
            this->_viewHandle  = pRenderModule->AddView();
        }

        // インゲームのシステムコンポーネントを追加
        {
            this->_systemComponentHandle = this->AddComponent<InGame::InGameSystemComponent>(0);
            auto pSystemComponent =
                this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
            pSystemComponent->SetFlgGameEnd(FALSE);
            pSystemComponent->SetPoint(0);
        }

        // TODO: 味方や敵の弾を管理するコンポーネント追加
        {
            this->_shotManagerComponentHandle =
                this->AddComponent<InGame::InGameShotManagerComponent>(0);
        }

        // TODO: インゲームのステージコンポーネント追加
        {
            this->_stageManagerComponentHandle =
                this->AddComponent<InGame::InGameStageManagerComponent>(0);
        }

        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.init();
        */

        return TRUE;
    }

    const Bool LevelInGame::VEnd()
    {
        auto pRenderModule = Module::ModuleManager::I().Get<Render::RenderModule>();
        if (pRenderModule != NULL) pRenderModule->RemoveView(this->_viewHandle);

        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.final();
        */
        return Node::VEnd();
    }

    void LevelInGame::VUpdate(const Float32 in_fDt)
    {
        Node::VUpdate(in_fDt);

        auto pSystemComponent =
            this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
        {
            if (pSystemComponent->IsGameEnd())
            {
                // next = new C_TitleSeq();
            }
        }

        // TODO: コリジョン処理をする
        {
            InGame::InGameCollisionComponent::CollisionAll();
        }
    }

}  // namespace Level
