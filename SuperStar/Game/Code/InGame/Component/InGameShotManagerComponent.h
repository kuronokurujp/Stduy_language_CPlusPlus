#pragma once

//	インクルード
// #include "actor/ActorSystem.h"
// #include "shot/InterfaceBulletEmit.h"
#include "Engine/MiniEngine.h"
#include "Level/Component/LevelBaseComponent.h"

namespace InGame
{
    /// <summary>
    /// 味方や敵の弾を管理するコンポーネント
    /// </summary>
    class InGameShotManagerComponent final : public Level::LevelBaseComponent
    {
        HE_CLASS_COPY_NG(InGameShotManagerComponent);
        HE_CLASS_MOVE_NG(InGameShotManagerComponent);
        HE_GENERATED_CLASS_BODY_HEADER(InGameShotManagerComponent, Level::LevelBaseComponent);

    public:
        /*
            //	定義
            //	弾属性について
            typedef enum
            {
                PLAER_PROPERTY = 0,
                ENEMY_PROPERTY,
            } PROPERTY_ENUM;

            typedef struct
            {
                I_InterfaceBulletEmit* pShot;
                PROPERTY_ENUM Proprety;
            } DATA_ST;
            */

        //	コンストラクタ
        InGameShotManagerComponent();
        virtual ~InGameShotManagerComponent();

        //	関数
        // bool update();
        // void draw();

        //	弾オブジェクトを登録。
        // int Add(I_InterfaceBulletEmit* in_pShot, const PROPERTY_ENUM in_Property);
        //	弾オブジェクトを取得
        // DATA_ST* GetData(const unsigned in_UniqId);

    private:
        //	変数
        // std::map<int, DATA_ST> m_aShotList;

        //	関数
        // virtual void _VOnCallbackOtherActorData(C_PaketSendActor* in_pOtherActor);
    };
}  // namespace InGame
