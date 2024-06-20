/**
 *	@file 	WorldMapSeq.cpp
 *	@brief	タイトルシーケンス
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note
 *
 */
#include "WorldMapSeq.h"

#include "game/GameSystem.h"

namespace MainSeq
{
    C_WorldMapSeq::C_WorldMapSeq()
    {
    }
    C_WorldMapSeq::~C_WorldMapSeq()
    {
    }

    I_MainSeq* C_WorldMapSeq::update(C_Parent* ip_parent)
    {
        I_MainSeq* next          = this;
        GameLib::C_GameSystem& r = GameLib::C_GameSystem::inst();
        r.drawDebugText("WorldMap", r.getScreenWidth() >> 1, r.getScreenHeight() >> 1);

        return next;
    }
}  // namespace MainSeq
