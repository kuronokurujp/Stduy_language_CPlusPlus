/**
 *	@file 	WorldMapSeq.h
 *	@brief	
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
#ifndef __WorldMapSeq_H__
#define __WorldMapSeq_H__

#include "sequence/GameInterface.h"

namespace MainSeq
{
	class C_WorldMapSeq : public I_MainSeq
	{
	public:
		C_WorldMapSeq();
		~C_WorldMapSeq();

		I_MainSeq* update( C_Parent* ip_parent );
	};
} // namespace MainSeq

#endif // __WorldMapSeq_H__
