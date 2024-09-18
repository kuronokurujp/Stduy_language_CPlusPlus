#pragma once

#include "Engine/MiniEngine.h"
#include "Level/Component/LevelBaseComponent.h"

namespace InGame
{
    /// <summary>
    /// インゲームのシステムコンポーネント
    /// </summary>
    class InGameSystemComponent final : public Level::LevelBaseComponent
    {
        HE_CLASS_COPY_NG(InGameSystemComponent);
        HE_CLASS_MOVE_NG(InGameSystemComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameSystemComponent, Level::LevelBaseComponent);

    public:
        //	定義
        enum
        {
            ENEMY_MAX = 200,
        };

        InGameSystemComponent() : Level::LevelBaseComponent() {}

        /*
                //	アクターハンドル
                typedef struct
                {
                    Sint32 stageManager = 0;
                    Sint32 player = 0;
                    Sint32 aEnemy[ENEMY_MAX];
                    Sint32 enemyMax = 0;
                } ACTOR_HANDLE_DATA_ST;

                ACTOR_HANDLE_DATA_ST& GetActorHandleData() const { return this->_actorHandleData; }
                */

        Bool IsGameEnd() const { return this->_bGameEnd; }
        void SetFlgGameEnd(const Bool in_bFlg) { this->_bGameEnd = in_bFlg; }
        void SetPoint(const Uint32 in_uPoint) { this->_uPoint = in_uPoint; }
        Uint32 GetPoint() const { return this->_uPoint; }
        void AddPoint(const Uint32 in_uAddPoint) { this->_uPoint += in_uAddPoint; }

    private:
        void _Clear();

    private:
        //	変数
        //        ACTOR_HANDLE_DATA_ST _actorHandleData;
        Bool _bGameEnd = FALSE;
        Uint32 _uPoint = 0;
    };

}  // namespace InGame
