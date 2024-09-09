#include "InGamePlayerActor.h"

#include "Actor/Component/InputComponent.h"
#include "Actor/Component/TransformComponent.h"

// 依存するモジュール一覧
#include "EnhancedInputModule.h"
#include "RenderModule.h"

#if 0

#include "actor/ActorCommon.h"
#include "common/Common.h"
#include "game/GameSystem.h"
#include "shot/HomingBulletEmit.h"
#include "shot/LaserBulletEmit.h"
#include "shot/NormalBulletEmit.h"
#include "shot/WayBulletEmit.h"
#include "system/System.h"
#include "tips/Primitive.h"
#endif

namespace InGame
{
    //	変数
    static const int s_PlayerInvCnt         = 30;
    static const float s_PlayerMoveSpeed    = 1.f;
    static const char* s_apPlayerShotName[] = {
        "Normal",
        "Laser",
        "Homing",
        "4Way",
    };

    namespace Local
    {
        // TODO: プレイヤーのユーザー入力
        static const Char* szInputMoveUp    = HE_STR_TEXT("Player_MoveUp");
        static const Char* szInputMoveLeft  = HE_STR_TEXT("Player_MoveLeft");
        static const Char* szInputMoveDown  = HE_STR_TEXT("Player_MoveDown");
        static const Char* szInputMoveRight = HE_STR_TEXT("Player_MoveRight");
        static const Char* szInputShot      = HE_STR_TEXT("Player_Shot");

        static const EnhancedInput::ActionMap mInputActionByPlay =
            {{szInputMoveUp, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_W})},
             {szInputMoveLeft, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_A})},
             {szInputMoveDown, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_S})},
             {szInputMoveRight, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_D})},
             {szInputShot, EnhancedInput::ActionData({Platform::EKeyboard::EKeyboard_SPACE})}};

        /// <summary>
        /// プレイヤーのユーザー入力
        /// </summary>
        class UserInputPlayerStrategy final : public Actor::InputStrategyBase
        {
            HE_CLASS_COPY_NG(UserInputPlayerStrategy);

        public:
            UserInputPlayerStrategy()
            {
                // ユーザー共通入力割り当て設定
                {
                    auto pInputModule =
                        Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
                    pInputModule->AddCommonMappingAction(Local::mInputActionByPlay);
                }
            }
            virtual ~UserInputPlayerStrategy()
            {
                // 専用の入力アクションを外す
                {
                    auto pInputModule =
                        Module::ModuleManager::I().Get<EnhancedInput::EnhancedInputModule>();
                    if (pInputModule != NULL)
                        pInputModule->RemoveCommonMappingAction(Local::mInputActionByPlay);
                }
            }

            void VProcessInput(const void* in_pInputMap,
                               Actor::Object* in_pSelfObject) override final
            {
                HE_ASSERT(in_pInputMap);
                HE_ASSERT(in_pSelfObject);

                auto pInputMap = reinterpret_cast<const EnhancedInput::InputMap*>(in_pInputMap);

                if (HE_GENERATED_CHECK_RTTI(*in_pSelfObject, InGamePlayerActor) == FALSE) return;

                auto pPlayer = reinterpret_cast<InGamePlayerActor*>(in_pSelfObject);

                Core::Math::Vector2 move;
                if (pInputMap->Contains(Local::szInputMoveUp))
                {
                    move += Core::Math::Vector2(0.0f, -1.0f);
                }

                if (pInputMap->Contains(Local::szInputMoveLeft))
                {
                    move += Core::Math::Vector2(-1.0f, 0.0f);
                }

                if (pInputMap->Contains(Local::szInputMoveDown))
                {
                    move += Core::Math::Vector2(0.0f, 1.0f);
                }

                if (pInputMap->Contains(Local::szInputMoveRight))
                {
                    move += Core::Math::Vector2(1.0f, 0.0f);
                }

                if (pInputMap->Contains(Local::szInputShot))
                {
                }

                move.Normalize();
                pPlayer->Move(move * pPlayer->GetParameter().speed);
            }
        };
    };  // namespace Local

    InGamePlayerActor::InGamePlayerActor() : Actor::Object()
    {
        this->_Clear();
    }

    const Bool InGamePlayerActor::VBegin()
    {
        if (Actor::Object::VBegin() == FALSE) return FALSE;

        // 座標関連のコンポーネント追加
        {
            this->_transformHandle = this->AddComponent<Actor::TransformComponent>(1);
            HE_ASSERT((this->_transformHandle.Null() == FALSE) &&
                      "トランスフォームコンポーネントの追加失敗");

            // 事前に設定していた座標をトランスフォームコンポーネントに設定
            this->SetPos(this->_pos);
        }

        // ユーザー入力コンポーネントを追加
        {
            auto handle = this->AddComponent<Actor::InputComponent>(0);
            HE_ASSERT(handle.Null() == FALSE);

            // プレイヤー用の入力ストラテジーを設定
            auto pStrategy = Core::Memory::MakeCustomSharedPtr<Local::UserInputPlayerStrategy>();
            this->GetComponent<Actor::InputComponent>(handle)->SetStrategy(pStrategy);
        }
#if 0
        //	自機が使用する弾を弾管理に追加する。
        {
            I_InterfaceBulletEmit* pShotList[eSHOT_MAX] = {NULL};

            {
                pShotList[eSHOT_BASIC]  = new C_NormalBulletEmit(20);
                pShotList[eSHOT_LASER]  = new C_LaserBulletEmit(1);
                pShotList[eSHOT_HOMING] = new C_HomingBulletEmit(8);
                pShotList[eSHOT_WAY]    = new C_WayBulletEmit(4, 12);
            }

            C_ColisionActorManager& inst = C_ColisionActorManager::inst();

            SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& rHandle =
                SystemProprtyInterfaceInGame::GetActorHandleData();

            ShotManagerActor* pShotManagerActor =
                (ShotManagerActor*)inst.GetData(rHandle.shotManager);
            if (pShotManagerActor != NULL)
            {
                for (int i = 0; i < eSHOT_MAX; ++i)
                {
                    //	弾を登録する。
                    m_aShotHandle[i] =
                        pShotManagerActor->Add(pShotList[i], ShotManagerActor::PLAER_PROPERTY);
                }
            }
        }
#endif

        return TRUE;
    }

    const Bool InGamePlayerActor::VEnd()
    {
        this->RemoveComponent(&this->_transformHandle);

        return Actor::Object::VEnd();
    }

    void InGamePlayerActor::VUpdate(const Float32 in_fDt)
    {
        Actor::Object::VUpdate(in_fDt);

        // TODO: 移動情報による座標更新
        {
            Core::Math::Vector2 newPos;
            newPos.SetAdd(this->_pos, this->_move);
            this->_move.Zero();

            this->SetPos(newPos);
        }

        // 描画座標取得
        Core::Math::Rect2 rect;
        {
            auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
            HE_ASSERT(pTrans);

            Core::Math::Rect2 srcRect(0.0f, 0.0f, this->_size._fX, this->_size._fY,
                                      Core::Math::Rect2::EAnchor_Center);
            pTrans->TransformLocalToWorldRect2D(&rect, srcRect);
        }

        // 描画コマンド追加
        Render::Command2DRectDraw(this->_viewHandle, rect, Render::RGB::White);

#if 0
        GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();

        //	パッド制御
        {
            //	上
            if (r.isButtonOn(Input::KEY_UP))
            {
                m_pos.y -= s_PlayerMoveSpeed;
            }

            //	下
            if (r.isButtonOn(Input::KEY_DOWN))
            {
                m_pos.y += s_PlayerMoveSpeed;
            }

            //	右
            if (r.isButtonOn(Input::KEY_RIGHT))
            {
                m_pos.x += s_PlayerMoveSpeed;
            }

            //	左
            if (r.isButtonOn(Input::KEY_LEFT))
            {
                m_pos.x -= s_PlayerMoveSpeed;
            }
        }

        if (m_InvincibleCnt > 0)
        {
            --m_InvincibleCnt;
        }

        // TODO: 描画コンポーネントに移行したほうがいいかも
        if ((m_InvincibleCnt % 2) == 0)
        {
            DrawPrimitive2DSimpleQuad(m_pos.x, m_pos.y, m_size, m_size, 0.f,
                                      D3DCOLOR_ARGB(255, 255, 255, 255));
        }
#endif
    }

    void InGamePlayerActor::SetPos(const Core::Math::Vector2& in_rPos)
    {
        this->_pos = in_rPos;

        if (this->_transformHandle.Null()) return;

        // 事前に設定していた座標をトランスフォームコンポーネントに設定
        auto pTrans = this->GetComponent<Actor::TransformComponent>(this->_transformHandle);
        HE_ASSERT(pTrans);

        pTrans->SetPos(Core::Math::Vector3(this->_pos));
    }

    void InGamePlayerActor::SetSize(const Core::Math::Vector2& in_rSize)
    {
        this->_size = in_rSize;
    }

    void InGamePlayerActor::SetViewHandle(const Core::Common::Handle& in_rHandle)
    {
        HE_ASSERT(in_rHandle.Null() == FALSE);
        this->_viewHandle = in_rHandle;
    }

#if 0
    /*
            @brief	現在の操作しているショット名を取得
            @return	ショット名
    */
    const char* C_PlayerActor::GetActiveShotName() const
    {
        return s_apPlayerShotName[m_shotType];
    }

    /*
            @brief	撃てる弾の名前を取得
            @return	ショット名を取得
    */
    const char* C_PlayerActor::GetShotName(const int in_Type) const
    {
        ASSERT(in_Type < eSHOT_MAX);
        return s_apPlayerShotName[in_Type];
    }

    /*
            @brief	弾を撃つ
    */
    void C_PlayerActor::Shot()
    {
        C_ColisionActorManager& inst = C_ColisionActorManager::inst();

        SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST& handle =
            SystemProprtyInterfaceInGame::GetActorHandleData();

        ShotManagerActor* pShotManagerActor = (ShotManagerActor*)inst.GetData(handle.shotManager);
        if (pShotManagerActor != NULL)
        {
            ShotManagerActor::DATA_ST* pShotData =
                pShotManagerActor->GetData(m_aShotHandle[m_shotType]);
            if (pShotData != NULL)
            {
                if (m_shotType == eSHOT_HOMING)
                {
                    // 誘導段の場合はどの座標に飛ぶか指定
                    C_HomingBulletEmit* pHomingBullteEmit = (C_HomingBulletEmit*)pShotData->pShot;

                    const int homingBulletMax = pHomingBullteEmit->GetBulletMax();

                    int bulletIdx = 0;
                    //	敵を探す
                    for (int i = 0; i < handle.enemyMax; ++i)
                    {
                        C_PaketSendActor* pEnemyActor =
                            (C_PaketSendActor*)inst.GetData(handle.aEnemy[i]);
                        if ((pEnemyActor != NULL) &&
                            (pEnemyActor->VGetRttiId() == C_ColisionActor::RTTI_ID))
                        {
                            C_ColisionActor* pEnemyCollisionActor = (C_ColisionActor*)pEnemyActor;
                            D3DXVECTOR3 pos                       = pEnemyCollisionActor->GetPos();

                            pHomingBullteEmit->setTargetPos(bulletIdx, pos);
                            ++bulletIdx;

                            if (bulletIdx >= homingBulletMax)
                            {
                                break;
                            }
                        }
                    }
                }

                pShotData->pShot->shot(m_pos);
            }
        }
    }

    /*
            @brief	弾の種類を返す
            @param	in_MoveNum	弾種類変更する方向を決める
    */
    void C_PlayerActor::MoveShotType(const int in_MoveNum)
    {
        if (in_MoveNum > 0)
        {
            m_shotType = (++m_shotType) % eSHOT_MAX;
        }
        else if (in_MoveNum < 0)
        {
            m_shotType = (--m_shotType) < 0 ? eSHOT_MAX - 1 : m_shotType;
        }
    }

    /*
            @brief	データ通知<BR>
            アクターシステム側からのデータ通知<BR>
    */
    void C_PlayerActor::VOnCallbackSendMessage(const int in_DataType, void* in_pData)
    {
        switch (in_DataType)
        {
            case eACTOR_SEND_MESS_HIT_ENEMY:
            {
                //	敵と接触した
                //	死亡。
                if (m_InvincibleCnt <= 0)
                {
                    printf("衝突\n");

                    --m_life;
                    m_InvincibleCnt = s_PlayerInvCnt;
                    if (m_life <= 0)
                    {
                        VDie();
                    }
                }

                break;
            }
        }
    }
#endif
}  // namespace InGame
