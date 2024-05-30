/**
 *	@file 	EndingSeq.cpp
 *	@brief	タイトルシーケンス
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
#include "EndingSeq.h"

#include "game/GameSystem.h"

namespace MainSeq
{
	C_EndingSeq::C_EndingSeq()
	{

	}
	C_EndingSeq::~C_EndingSeq()
	{

	}

	I_MainSeq* C_EndingSeq::update( C_Parent* ip_parent )
	{
		I_MainSeq* next = this;
		GameLib::C_GameSystem& r =GameLib::C_GameSystem::inst();
		r.drawDebugText( "Ending", r.getScreenWidth() >> 1, r.getScreenHeight() >> 1 );

		return next;
	}
} // namespace MainSeq


