#include "InGameActorSystem.h"

#if 0
#include <vector>

#include "common/Common.h"

namespace InGame
{
    InGameActorManager InGameActorManager::m_Manager;

    using namespace std;

    void C_ActorManager::init(void)
    {
        m_Handle = 0;
        if (m_ActorHashTable.empty() == false)
        {
            final();
        }
    }

    void C_ActorManager::final(void)
    {
        I_Actor* pActor = NULL;
        // すべてのオブジェクトを削除
        multimap<int, I_Actor*>::iterator it;  // メッセージ→タスクハッシュのイテレータ
        for (it = m_ActorHashTable.begin(); it != m_ActorHashTable.end(); it++)
        {
            pActor = (I_Actor*)(*it).second;
            //	終了処理
            pActor->final();

            //	アクター削除
            delete (*it).second;
        }
        m_ActorHashTable.clear();

        m_Handle = 0;
    }

    // 追加、削除が必要。
    // 追加時ハンドルを返す
    int C_ActorManager::add(I_Actor* in_pClass)
    {
        if (in_pClass == NULL)
        {
            return I_ActorManager::NOT_HANDLE;
        }

        ++m_Handle;
        //	ハンドルをローカルへ保存(init関数でadd関数を呼ぶこともあるので)
        int handle = m_Handle;

        pair<int, I_Actor*> p(handle, in_pClass);
        m_ActorHashTable.insert(p);

        in_pClass->init(handle);

        return handle;
    }

    // 削除にはハンドルが必要。
    void C_ActorManager::destroy(int in_handle)
    {
        I_Actor* pData = GetData(in_handle);
        if (pData != NULL)
        {
            pData->final();

            SAFE_DELETE(pData);
            // multimapから削除
            multimap<int, I_Actor*>::iterator it;
            it = m_ActorHashTable.find(in_handle);
            if (it != m_ActorHashTable.end())
            {
                m_ActorHashTable.erase(it);
            }
        }
    }

    //	更新
    void C_ActorManager::update(void)
    {
        multimap<int, I_Actor*>::iterator it;
        for (it = m_ActorHashTable.begin(); it != m_ActorHashTable.end(); it++)
        {
            (*it).second->update();
        }
    }

    //	描画
    void C_ActorManager::draw(void)
    {
        multimap<int, I_Actor*>::iterator it;
        for (it = m_ActorHashTable.begin(); it != m_ActorHashTable.end(); it++)
        {
            (*it).second->draw();
        }
    }

    // ハンドルからアクセスする
    I_Actor* C_ActorManager::GetData(int in_handle)
    {
        multimap<int, I_Actor*>::iterator it;
        it = m_ActorHashTable.find(in_handle);
        if (it != m_ActorHashTable.end())
        {
            return (*it).second;
        }

        return NULL;
    }

    const Bool InGameActorManager::Start(const Uint32 in_uActorCapacity,
                                         const Uint32 in_uActorGroupMax)
    {
        if (Level::Node::CustomActorManager::Start(in_uActorCapacity, in_uActorGroupMax) == FALSE)
            return FALSE;
        /*
                //	Lua側で呼び出す関数追加
                //	スクリプト側からの送信
                LuaStateManager::AddFunc("Actor", "TriggerEvent", TriggerEventLua);
                */

        return TRUE;
    }

    void InGameActorManager::Update(const Float32 in_fDt, const Core::TaskData& in_rTaskData)
    {
        Level::Node::CustomActorManager::Update(in_fDt, in_rTaskData);

        /*
                vector<I_Actor*> aDieActorList;

                multimap<int, I_Actor*>::iterator it;
                multimap<int, I_Actor*>::iterator ithit;
                for (it = m_ActorHashTable.begin(); it != m_ActorHashTable.end(); ++it)
                {
                    C_PaketSendActor* pColisionActor = (C_PaketSendActor*)((*it).second);
                    if (pColisionActor == NULL || pColisionActor->VIsStop() == true)
                    {
                        continue;
                    }

                    pColisionActor->update();
                    if (pColisionActor->VIsDie() == true)
                    {
                        //	自殺させる
                        //	更新がすべて終わったら
                        aDieActorList.push_back(pColisionActor);
                        continue;
                    }

                    bool bEndLoop = false;
                    for (ithit = m_ActorHashTable.begin();
                         (ithit != m_ActorHashTable.end()) && (bEndLoop == false); ++ithit)
                    {
                        if (ithit != it)
                        {
                            C_PaketSendActor* pColisionActorChk =
           (C_PaketSendActor*)((*ithit).second); if ((pColisionActorChk == NULL) ||
           (pColisionActorChk->VIsDie() == true))
                            {
                                continue;
                            }

                            //	衝突対象チェックのアクターを呼び出す
                            pColisionActor->_VOnCallbackOtherActorData(pColisionActorChk);

                            if (pColisionActor->VIsDie() == true)
                            {
                                bEndLoop = true;
                                continue;
                            }
                        }
                    }
                }

                //	ここで自殺したやつを完全削除
                {
                    vector<I_Actor*>::iterator it;
                    for (it = aDieActorList.begin(); it != aDieActorList.end(); ++it)
                    {
                        //	削除
                        destroy((*it)->VGetHandle());
                    }
                }
        */
    }

    int InGameActorManager::add(I_Actor* in_pClass)
    {
        I_Actor* pActor = dynamic_cast<C_PaketSendActor*>(in_pClass);
        ASSERT(pActor != NULL);
        return C_ActorManager::add(in_pClass);
    }

    /*
            @brief スクリプト側からのデータ受信
    */
    int InGameActorManager::TriggerEventLua(lua_State* in_pState)
    {
        InGameActorManager& inst = InGameActorManager::inst();

        //	スクリプト側からの送信をアクターに通知
        multimap<int, I_Actor*>::iterator it;
        multimap<int, I_Actor*>::iterator ithit;
        for (it = inst.GetList().begin(); it != inst.GetList().end(); ++it)
        {
            C_PaketSendActor* pColisionActor = (C_PaketSendActor*)((*it).second);
            if ((pColisionActor != NULL) && (pColisionActor->VIsDie() == false))
            {
                //	スクリプト側のデータ送信結果を受け取る
                pColisionActor->_VOnCallbackScriptEvent(in_pState);
            }
        }

        return 0;
    }
}  // namespace InGame
#endif
