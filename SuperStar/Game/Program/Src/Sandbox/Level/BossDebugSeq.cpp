/********************************************************************
	created:	2012/02/13
	created:	13:2:2012   23:36
	file base:	BossDebugSeq
	
	purpose:	
*********************************************************************/
#include "BossDebugSeq.h"

#include "game/GameSystem.h"
#include "DebugSeq.h"

#include "actor/ActorSystem.h"
#include "actor/boss/BossStage1.h"

namespace MainSeq
{
	static int	g_BossDebugHandle	= -1;
	/*
		@brief	コンストラクタ	
	*/
	C_BossDebugSeq::C_BossDebugSeq()
	{
		g_BossDebugHandle	= -1;
		C_ColisionActorManager::inst().init();
	}

	/*
		@brief	デストラクタ
	*/
	C_BossDebugSeq::~C_BossDebugSeq()
	{
		C_ColisionActorManager::inst().final();
	}

	/*
		@brief	更新
	*/
	I_MainSeq* C_BossDebugSeq::update( C_Parent* ip_parent )
	{
		I_MainSeq*	pNext	= this;

		C_ColisionActorManager&	rActorMan	= C_ColisionActorManager::inst();
		rActorMan.update();
		rActorMan.draw();

		GameLib::C_GameSystem&	r	= GameLib::C_GameSystem::inst();
		if( r.isButtonTriggerOn( 'a' ) )
		{
			pNext	= new C_DebugSeq;
		}
		else if( r.isButtonTriggerOn( 'b' ) )
		{
			//	ボス作成
			if( g_BossDebugHandle != -1 )
			{
				rActorMan.destroy( g_BossDebugHandle );
				g_BossDebugHandle	= -1;
			}

			g_BossDebugHandle	= rActorMan.add( new BossStage1 );
		}

		return	pNext;
	}
} // namespace MainSeq
