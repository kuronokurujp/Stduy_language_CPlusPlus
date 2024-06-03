/**
 *	@file 	SpriteDebugSeq.h
 *	@brief	
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
#ifndef __SPRITE_DEBUG_SEQ_H__
#define __SPRITE_DEBUG_SEQ_H__

#include "sequence/GameInterface.h"

namespace MainSeq
{
	class C_SpriteDebugSeq : public I_MainSeq
	{
	public:
		C_SpriteDebugSeq();
		~C_SpriteDebugSeq();

		I_MainSeq* update( C_Parent* ip_parent );
	};
} // namespace MainSeq

#endif // __SPRITE_DEBUG_SEQ_H__
