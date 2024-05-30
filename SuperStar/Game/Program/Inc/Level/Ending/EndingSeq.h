/**
 *	@file 	EndingSeq.h
 *	@brief	
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
#ifndef __EndingSeq_H__
#define __EndingSeq_H__

#include "sequence/GameInterface.h"

namespace MainSeq
{
	class C_EndingSeq : public I_MainSeq
	{
	public:
		C_EndingSeq();
		~C_EndingSeq();

		I_MainSeq* update( C_Parent* ip_parent );
	};
} // namespace MainSeq

#endif // __EndingSeq_H__
