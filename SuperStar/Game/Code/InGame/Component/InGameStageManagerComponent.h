#pragma once

// #include <vector>

// #include "actor/ActorSystem.h"
#include "Actor/Component/Component.h"
#include "Engine/MiniEngine.h"

namespace InGame
{
    class InGameStageManagerComponent : public Actor::Component
    {
        HE_CLASS_COPY_NG(InGameStageManagerComponent);
        HE_CLASS_MOVE_NG(InGameStageManagerComponent);
        GENERATED_CLASS_BODY_HEADER(InGameStageManagerComponent, Actor::Component);

    public:
        //	コンストラクタ
        InGameStageManagerComponent();
        virtual ~InGameStageManagerComponent() {}

        /// <summary>
        /// タスク開始
        /// </summary>
        const Bool VBegin() override final;

        /// <summary>
        /// タスク終了
        /// </summary>
        const Bool VEnd() override final;

        /*
                // 初期化、終了
                // 更新、描画で十分
                //	ここでコリジョン判定をする
                virtual bool update(void);

                //	データ通知
                virtual void VOnCallbackSendMessage(const int in_DataType, void* in_pData);
                */

    private:
        /*
            //	定義
            typedef struct
            {
                char* pEventName;
                void (StageManagerActor::*pFunc)(lua_State* in_pLuaState);
            } _EVENT_FUNC;

            enum
            {
                _STATE_SCROLL = 0,
                _STATE_BOSS,
                _STATE_GAME_END,
            };

            static _EVENT_FUNC m_aEventFuncList[];

            //	オーバーライド関数
            //	登録した自分以外のアクターを取得する。
            virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
            //	スクリプトで通知したデータを受け取る
            virtual void _VOnCallbackScriptEvent(lua_State* in_pLuaState);

    */
        //	関数
        //	クリア
        void _Clear();

        /*
                void _CreateStageEvent(lua_State* in_pLuaState) {}

                //	変数
                int m_StageIndex;
                int m_W;
                int m_MapXPos;
                int m_Speed;
                int m_ScrollEndXPos;
                int m_State;
                bool m_bClearPoint;
                unsigned m_ClearPointNum;
                int m_BossHandle;

                std::vector<class C_EnemyActorBase*> m_aMapSettingEnemyList;
                */
    };

}  // namespace InGame
