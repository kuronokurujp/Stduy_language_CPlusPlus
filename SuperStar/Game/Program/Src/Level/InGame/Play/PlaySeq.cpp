/**
 *	@file 	PlaySeq.cpp
 *	@brief	�^�C�g���V�[�P���X
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
//	�C���N���[�h
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

		//	�X�e�[�W�쐬

		//	�A�N�^�[�o�^
		{
			SystemProprtyInterfaceInGame::ACTOR_HANDLE_DATA_ST&	rHandle	= SystemProprtyInterfaceInGame::GetActorHandleData();
			rHandle.enemyMax	= 0;

			SystemProprtyInterfaceInGame::SetFlgGameEnd( false );
			SystemProprtyInterfaceInGame::SetPoint( 0 );

			//	�w�i�ݒ�
			inst.add( new BgStartPoint() );

			//	�e�Ǘ���o�^
			rHandle.shotManager	= inst.add( new ShotManagerActor() );

			//	�X�e�[�W�Ǘ��o�^
			inst.add( new StageManagerActor() );
		}
	}

	C_PlaySeq::~C_PlaySeq()
	{
		C_ColisionActorManager&	inst	= C_ColisionActorManager::inst();
		inst.final();
	}

	/*
		@brief	�X�V
		@return	�X�e�[�g	
	*/
	I_MainSeq* C_PlaySeq::update( C_Parent* ip_parent )
	{
		I_MainSeq* next = this;

		C_ColisionActorManager&	inst	= C_ColisionActorManager::inst();

		//	�A�N�^�[�X�V
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


