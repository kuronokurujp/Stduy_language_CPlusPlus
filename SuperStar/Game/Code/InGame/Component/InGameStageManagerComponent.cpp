#include "InGameStageManagerComponent.h"

#include "InGame/Actor/Player/InGamePlayerActor.h"
/*
#include "actor/boss/BossStage1.h"
#include "actor/enemy/Snake.h"
#include "actor/enemy/Split.h"
#include "actor/enemy/Zako.h"
#include "actor/player/Player.h"
#include "event/data/Stage.h"
#include "game/GameSystem.h"
#include "scripting/LuaStateManager.h"
#include "system/System.h"
*/

namespace InGame
{
    /*
        static const char* s_pStageManagerScriptName[] = {
            "./resource/script/stage/stage01.lua",
        };

        static const char* s_pPlayerKeyGuideText = "a: SHOT d/s: NEXT SHOT TYPE";

    */
    //	受け取るイベントリスト
    /*
    InGameStageManagerComponent::_EVENT_FUNC InGameStageManagerComponent::m_aEventFuncList[] = {
        {"RequestStageManagerCreate", &InGameStageManagerComponent::_CreateStageEvent},
    };
    */

    InGameStageManagerComponent::InGameStageManagerComponent() : Level::LevelBaseComponent()
    {
        _Clear();
    }

    Bool InGameStageManagerComponent::VBegin()
    {
        if (Level::LevelBaseComponent::VBegin() == FALSE) return FALSE;
        /*
            LuaStateManager::DoFile(s_pStageManagerScriptName[m_StageIndex], "Init");

            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                SystemProprtyInterfaceInGame::GetActorHandleData();
            C_ColisionActorManager& iActorManager = C_ColisionActorManager::inst();


            m_State = _STATE_SCROLL;
            */

        // プレイヤーアクター生成
        this->_playerHandle = this->AddActor<InGamePlayerActor>();
        // プレイヤーの外部からの初期設定
        {
            auto pPlayer = this->GetActor<InGamePlayerActor>(this->_playerHandle);
            // 位置
            pPlayer->SetPos(Core::Math::Vector2(100.0f, 100.0f));
            // サイズ
            pPlayer->SetSize(Core::Math::Vector2(30.0f, 30.0f));

            pPlayer->SetViewHandle(this->_viewHandle);
        }

        return TRUE;
    }

    Bool InGameStageManagerComponent::VEnd()
    {
        this->RemoveActor(&this->_playerHandle);
        return LevelBaseComponent::VEnd();

        /*
            std::vector<C_EnemyActorBase*>::iterator it;
            for (it = m_aMapSettingEnemyList.begin(); it != m_aMapSettingEnemyList.end(); ++it)
            {
                SAFE_DELETE(*it);
            }
            */
    }

    void InGameStageManagerComponent::SetViewHandle(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        this->_viewHandle = in_rHandle;
    }

    /*
            @brief	更新
    */
    /*
    bool StageManagerActor::update()
    {
        GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

        C_ColisionActorManager& iActorManager = C_ColisionActorManager::inst();

        SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
            SystemProprtyInterfaceInGame::GetActorHandleData();
        C_PlayerActor* pPlayerActor = (C_PlayerActor*)iActorManager.GetData(rHandle.player);

        //	プレイヤー入力
        if (pPlayerActor != NULL)
        {
            if (r.isButtonOn('a'))
            {
                pPlayerActor->Shot();
            }
            else if (r.isButtonTriggerOn('s'))
            {
                pPlayerActor->MoveShotType(1);
            }
            else if (r.isButtonTriggerOn('d'))
            {
                pPlayerActor->MoveShotType(-1);
            }

            //	プレイヤー状態表示
            std::string ActiveShotName = pPlayerActor->GetActiveShotName();
            for (int i = 0, posX = 0; i < pPlayerActor->GetShotTypeNum(); ++i)
            {
                std::string Name = pPlayerActor->GetShotName(i);

                posX += (int)(Name.length() * 16);

                unsigned int color = 0xffffffff;
                if (ActiveShotName == Name)
                {
                    //	現在撃てる弾名
                    color = 0xff00ffff;
                }

                r.drawDebugText(Name.c_str(), 0 + posX, r.getScreenHeight() - 32, color);
            }

            //	キーガイド
            {
                r.drawDebugText(s_pPlayerKeyGuideText, 0, r.getScreenHeight() - 46);
            }

            //	プレイヤーHP表示
            char pLifeString[256] = {NULL};
            sprintf_s(pLifeString, "Life %d", pPlayerActor->GetLifeNum());
            r.drawDebugText(pLifeString, 0, r.getScreenHeight() - (46 + 16));
        }

        if (pPlayerActor == NULL)
        {
            //	自機死亡
            const char* pStageClearString = "Game Over";
            r.drawDebugText(pStageClearString,
                            (r.getScreenWidth() >> 1) - strlen(pStageClearString) * (16 >> 1),
                            (r.getScreenHeight() >> 1));

            const char* pBtnGuideString = "[a]:End";
            r.drawDebugText(pBtnGuideString,
                            (r.getScreenWidth() >> 1) - strlen(pBtnGuideString) * (16 >> 1),
                            (r.getScreenHeight() >> 1) + 32);

            if (r.isButtonTriggerOn('a') == true)
            {
                //	ゲーム終了
                SystemProprtyInterfaceInGame::SetFlgGameEnd(true);
            }
        }
        else
        //	マップ座標を動かす
        {
            switch (m_State)
            {
                case _STATE_SCROLL:
                {
                    if (m_MapXPos < m_ScrollEndXPos)
                    {
                        m_MapXPos += m_Speed;

                        //	指定した位置に到達したら敵を登録する。
                        if (!m_aMapSettingEnemyList.empty())
                        {
                            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                                SystemProprtyInterfaceInGame::GetActorHandleData();

                            //	マップ配置している敵がいる。
                            std::vector<C_EnemyActorBase*>::iterator it;
                            for (it = m_aMapSettingEnemyList.begin();
                                 it != m_aMapSettingEnemyList.end();)
                            {
                                Vec3 pos = (*it)->GetPos();
                                pos.x -= (float)m_Speed;
                                float limitXPos = (float)r.getScreenWidth() - (*it)->GetSize();
                                if (pos.x <= limitXPos)
                                {
                                    //	座標をずらして指定した範囲にいればアクターに登録
                                    Vec3 initPos                     = (*it)->GetPos();
                                    rHandle.aEnemy[rHandle.enemyMax] = iActorManager.add(*it);
                                    (*it)->VStop(false);
                                    (*it)->SetPos(initPos);

                                    ++rHandle.enemyMax;

                                    it = m_aMapSettingEnemyList.erase(it);
                                }
                                else
                                {
                                    (*it)->SetPos(pos);
                                    ++it;
                                }
                            }
                        }
                    }
                    else
                    {
                        //	ステージの端にきた。
                        for (int i = 0; i < rHandle.enemyMax; ++i)
                        {
                            iActorManager.destroy(rHandle.aEnemy[i]);
                        }
                        rHandle.enemyMax = 0;

                        if (m_bClearPoint == true)
                        {
                            //	ゲーム終了
                            m_State = _STATE_GAME_END;
                        }
                        else
                        {
                            m_BossHandle = iActorManager.add(new BossStage1);
                            m_State      = _STATE_BOSS;
                        }
                    }

                    break;
                }
                case _STATE_BOSS:
                {
                    if (iActorManager.GetData(m_BossHandle) == NULL)
                    {
                        //	終了
                        m_State = _STATE_GAME_END;
                    }

                    break;
                }
                case _STATE_GAME_END:
                {
                    char aStageEndMessageString[256] = {NULL};
                    if (m_bClearPoint == true)
                    {
                        if (SystemProprtyInterfaceInGame::GetPoint() >= m_ClearPointNum)
                        {
                            sprintf_s(aStageEndMessageString, "Stage Clear");
                        }
                        else
                        {
                            sprintf_s(aStageEndMessageString, "Game Over");
                        }
                    }
                    else
                    {
                        sprintf_s(aStageEndMessageString, "Stage Clear");
                    }

                    r.drawDebugText(aStageEndMessageString,
                                    (r.getScreenWidth() >> 1) -
                                        strlen(aStageEndMessageString) * (16 >> 1),
                                    (r.getScreenHeight() >> 1));

                    const char* pBtnGuideString = "[a]:End";
                    r.drawDebugText(pBtnGuideString,
                                    (r.getScreenWidth() >> 1) - strlen(pBtnGuideString) * (16 >> 1),
                                    (r.getScreenHeight() >> 1) + 32);

                    if (r.isButtonTriggerOn('a') == true)
                    {
                        //	ゲーム終了
                        SystemProprtyInterfaceInGame::SetFlgGameEnd(true);
                    }

                    break;
                }
            }

            //	スコア表示
            char aScoreString[256] = {NULL};
            sprintf_s(aScoreString, "Score : %d", SystemProprtyInterfaceInGame::GetPoint());
            r.drawDebugText(aScoreString, 0, 32);
        }

        return true;
    }
    */

    /*
            @brief	データ通知
    */
    /*
    void StageManagerActor::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
    }
    */

    /*
            @brief	登録した自分以外のアクターを取得する。
    */
    /*
    void StageManagerActor::_VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor)
    {
    }
    */

    /*
            @brief	スクリプトで通知したデータを受け取る
    */
    /*
    void StageManagerActor::_VOnCallbackScriptEvent(lua_State* in_pLuaState)
    {
        //	スクリプトからデータを復元
        if (in_pLuaState == NULL)
        {
            return;
        }

        //	データタイプを取得
        const char* pEventTypeName = lua_tostring(in_pLuaState, 1);
        if (pEventTypeName == NULL)
        {
            return;
        }

        if (strncmp(pEventTypeName, "RequestInitStage", strlen(pEventTypeName)) == 0)
        {
            //	ステージ初期化
            StageInitPaeramEventData eventData;
            if (eventData.VBuildLuaEventData(in_pLuaState) == false)
            {
                return;
            }

            m_MapXPos       = 0;
            m_Speed         = eventData.m_Speed;
            m_W             = eventData.m_W;
            m_bClearPoint   = eventData.m_ClearPointFlg;
            m_ClearPointNum = eventData.m_ClearPoint;

            GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();
            m_ScrollEndXPos          = m_W - r.getScreenWidth();
        }
        else if (strncmp(pEventTypeName, "RequestCreateEnemy", strlen(pEventTypeName)) == 0)
        {
            //	敵作成
            StageCreateEnemyEventData eventData;

            eventData.VBuildLuaEventData(in_pLuaState);

            //	敵を作成するが、登場をしない。
            C_EnemyActorBase* pEnemy = NULL;

            Vec3 initPos((float)eventData.m_XPos, (float)eventData.m_YPos, 0.f);
            //	雑魚
            if (strncmp(eventData.m_pName, "Zako", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new C_EnemyActorZako(
                    C_EnemyActorBase::ChangeAblityTypeStringToNum(eventData.m_pTypeName));
            }
            //	蛇
            else if (strncmp(eventData.m_pName, "Snake", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new EnemyActorSnake();
            }
            //	分裂
            else if (strncmp(eventData.m_pName, "Split", strlen(eventData.m_pName)) == 0)
            {
                pEnemy = new EnemyActorSplit();
            }
            else
            {
                STRING_ASSERT(false, "生成する敵タイプが間違っています。");
            }

            if (pEnemy != NULL)
            {
                pEnemy->VStop(true);
                pEnemy->SetPos(initPos);

                m_aMapSettingEnemyList.push_back(pEnemy);
            }
        }
    }
    */

    /*
            @brief	クリア
    */
    void InGameStageManagerComponent::_Clear()
    {
        this->_playerHandle.Clear();
        this->_viewHandle.Clear();
        /*
            m_StageIndex    = 0;
            m_Speed         = 0;
            m_MapXPos       = 0;
            m_W             = 0;
            m_ScrollEndXPos = 0;
            m_State         = _STATE_SCROLL;
            m_bClearPoint   = false;
            m_ClearPointNum = 0;
            m_BossHandle    = 0;
            */
    }
}  // namespace InGame
