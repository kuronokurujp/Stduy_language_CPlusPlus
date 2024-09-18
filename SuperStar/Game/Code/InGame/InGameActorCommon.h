#pragma once

namespace InGame
{
    //	前方宣言
    class InGameShotComponent;

    //	アクター通知タイプ
    enum
    {
        eACTOR_SEND_MESS_HIT_ENEMY,
        eACTOR_SEND_MESS_HIT_SHOT,
    };

    //	eACTOR_SEND_MESS_HIT_SHOT通知時のデータ
    typedef struct
    {
        int proprety;
        int bulletIndex;
        InGameShotComponent* pShotEmit;
    } HIT_SHOT_SEND_DATA_ST;

}  // namespace InGame
