#include "LevelInGame.h"

#include "InGame/Component/InGameSystemComponent.h"

namespace Level
{
    const Bool LevelInGame::Begin()
    {
        const Bool bRet = Node::Begin();
        HE_ASSERT(bRet);

        // インゲームのシステムコンポーネントを追加
        this->_systemComponentHandle = this->AddComponent<InGame::InGameSystemComponent>(0);
        auto pSystemComponent =
            this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);

        pSystemComponent->SetFlgGameEnd(FALSE);
        pSystemComponent->SetPoint(0);

        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.init();

        //	アクター登録
        {
            //	背景設定
            inst.add(new BgStartPoint());

            //	弾管理を登録
            rHandle.shotManager = inst.add(new ShotManagerActor());

            //	ステージ管理登録
            inst.add(new StageManagerActor());
        }
        */

        return TRUE;
    }

    const Bool LevelInGame::End()
    {
        /*
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();
        inst.final();
        */
        const Bool bRet = Node::End();
        HE_ASSERT(bRet);

        return TRUE;
    }

    void LevelInGame::Update(const Float32 in_fDt, const Core::TaskData& in_rTaskData)
    {
        Node::Update(in_fDt, in_rTaskData);

        auto pSystemComponent =
            this->GetComponent<InGame::InGameSystemComponent>(this->_systemComponentHandle);
        {
            if (pSystemComponent->IsGameEnd())
            {
                // next = new C_TitleSeq();
            }
        }
    }

}  // namespace Level
