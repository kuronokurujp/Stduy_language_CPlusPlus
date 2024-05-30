#include "Level/Title/TitleLevel.h"

#include "Core/Common/FixMap.h"

namespace Level
{
    static const char*	s_pTitleSeqTitleNameText	= "STG";
    static const char*	s_pTitleSeqKeyGuideText		= " Space : GAME START a : GAME END";

    TitleLevel::TitleLevel() : Level::Node()
    {
    }

    TitleLevel::~TitleLevel() {}

    const Bool TitleLevel::Begin()
    {
        Level::Node::Begin();

        return TRUE;
    }

    const Bool TitleLevel::End()
    {
        Level::Node::End();

        return TRUE;
    }

    void TitleLevel::Update(const Float32 in_dt, const Core::TaskData* in_pData)
    {
        Level::Node::Update(in_dt, in_pData);
    }

/*
    I_MainSeq* TitleLevel::update( C_Parent* ip_parent )
    {
        I_MainSeq* next = this;

        GameLib::C_GameSystem& r =GameLib::C_GameSystem::inst();

        //	パッド制御
        {
            if( r.isPadTriggerOn( Input::PAD_KEY_BTN ) || r.isButtonTriggerOn( Input::KEY_SPACE ) )
            {
                //	次のステートへ
                next	= new	C_PlaySeq();
            }
            else if( r.isButtonTriggerOn( 'a' ) )
            {
                next	= NULL;
            }
        }

        //	画面真ん中にタイトル名を表示
        r.drawDebugText( s_pTitleSeqTitleNameText, r.getScreenWidth() >> 1, r.getScreenHeight() >> 1 );

        std::string	keyGuideText	= s_pTitleSeqKeyGuideText;
        r.drawDebugText( s_pTitleSeqKeyGuideText, ( r.getScreenWidth() >> 1 ) - ( ( keyGuideText.length() / 2 ) * 8 ), ( r.getScreenHeight() >> 1 ) + 32 );

        return next;
    }
*/
}


