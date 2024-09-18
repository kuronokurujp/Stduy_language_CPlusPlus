#include "InGameShotManagerComponent.h"

namespace InGame
{
    InGameShotManagerComponent::InGameShotManagerComponent() : Level::LevelBaseComponent()
    {
    }

    InGameShotManagerComponent::~InGameShotManagerComponent()
    {
        /*
            map<int, DATA_ST>::iterator it;
            for (it = m_aShotList.begin(); it != m_aShotList.end(); it++)
            {
                (*it).second.pShot->release();
                SAFE_DELETE((*it).second.pShot);
            }
            */
    }

    /*
bool InGameShotManagerComponent::update()
{
        //	登録した弾の更新
        map<int, DATA_ST>::iterator it;
        for (it = m_aShotList.begin(); it != m_aShotList.end(); it++)
        {
            (*it).second.pShot->update();
        }

    return true;
}
        */
    /*
        void InGameShotManagerComponent::draw()
        {
                //	登録した弾の描画
                map<int, DATA_ST>::iterator it;
                for (it = m_aShotList.begin(); it != m_aShotList.end(); it++)
                {
                    (*it).second.pShot->draw();
                }
        }
                */

    /*
        void InGameShotManagerComponent::_VOnCallbackOtherActorData(C_PaketSendActor*
       in_pOtherActor)
        {
                //	RTTI判定衝突用のアクターかチェック
                if (in_pOtherActor->VGetRttiId() != C_ColisionActor::RTTI_ID)
                {
                    return;
                }

                C_ColisionActor* pCollisionActor = NULL;
                pCollisionActor                  = (C_ColisionActor*)in_pOtherActor;

                //	登録した弾すべての当たり判定
                map<int, DATA_ST>::iterator it;
                for (it = m_aShotList.begin(); it != m_aShotList.end(); it++)
                {
                    int ret = (*it).second.pShot->isIntersected(pCollisionActor->GetPos(),
                                                                pCollisionActor->GetSize(),
                                                                pCollisionActor->GetSize());
                    if (ret >= 0)
                    {
                        //	ヒットしたと通知
                        //	接触した
                        HIT_SHOT_SEND_DATA_ST data = {0};
                        data.proprety              = (int)(*it).second.Proprety;
                        data.bulletIndex           = ret;
                        data.pShotEmit             = (*it).second.pShot;
                        in_pOtherActor->VOnCallbackSendMessage(eACTOR_SEND_MESS_HIT_SHOT,
       (void*)&data);
                    }
                }
        }

                */
    /*
int InGameShotManagerComponent::Add(I_InterfaceBulletEmit* in_pShot,
                                          const PROPERTY_ENUM in_Property)
{
        if (in_pShot == NULL)
        {
            return -1;
        }

        //	弾初期化
        in_pShot->init();
        in_pShot->setBulletAngle(0.f);

        int setHandle = (int)m_aShotList.size();

        DATA_ST data;
        data.pShot    = in_pShot;
        data.Proprety = in_Property;
        pair<int, DATA_ST> p(setHandle, data);
        m_aShotList.insert(p);

        return (unsigned)setHandle;
}
        */

    /*
InGameShotManagerComponent::DATA_ST* InGameShotManagerComponent::GetData(
    const unsigned int in_UniqId)
{
        map<int, DATA_ST>::iterator it;
        it = m_aShotList.find(in_UniqId);
        if (it != m_aShotList.end())
        {
            return &(*it).second;
        }
    return NULL;
}
        */
}  // namespace InGame
