/**
 *	@file 	PlaySeq.cpp
 *	@brief	タイトルシーケンス
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
//	インクルード
#include "PlaySeq.h"

#include "game/GameSystem.h"

#include "system/System.h"
#include "actor/ActorSystem.h"
#include "actor/player/Player.h"
#include "actor/stage/Manager.h"
#include "actor/shot/Manager.h"
#include "actor/bg/BgStartPoint.h"

#ifdef _DEBUG
#include "sequence/debug/DebugSeq.h"
#include "scripting/LuaStateManager.h"
#endif

#include "sequence/title/TitleSeq.h"

#include <string>

namespace MainSeq
{
	C_PlaySeq::C_PlaySeq()
	{
		C_ColisionActorManager&	inst	= C_ColisionActorManager::inst();
		inst.init();

		//	ステージ作成

		//	アクター登録
		{
			SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST&	rHandle	= SystemProprtyInterfaceInGame::GetActorHandleData();
			rHandle.enemyMax	= 0;

			SystemProprtyInterfaceInGame::SetFlgGameEnd( false );
			SystemProprtyInterfaceInGame::SetPoint( 0 );

			//	背景設定
			inst.add( new BgStartPoint() );

			//	弾管理を登録
			rHandle.shotManager	= inst.add( new ShotManagerActor() );

			//	ステージ管理登録
			inst.add( new StageManagerActor() );
		}
	}

	C_PlaySeq::~C_PlaySeq()
	{
		C_ColisionActorManager&	inst	= C_ColisionActorManager::inst();
		inst.final();
	}

	/*
		@brief	更新
		@return	ステート	
	*/
	I_MainSeq* C_PlaySeq::update( C_Parent* ip_parent )
	{
		I_MainSeq* next = this;

		C_ColisionActorManager&	inst	= C_ColisionActorManager::inst();

		//	アクター更新
		{
			inst.update();
			inst.draw();
		}

		{
			if( SystemProprtyInterfaceInGame::IsGameEnd() == true )
			{
				next	= new C_TitleSeq();
			}
		}

		return next;
	}
} // namespace MainSeq


