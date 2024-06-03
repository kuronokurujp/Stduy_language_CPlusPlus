/********************************************************************
	created:	2012/02/13
	created:	13:2:2012   23:34
	file base:	BossDebugSeq
	
	purpose:	登場するボステスト用
*********************************************************************/

#ifndef __BOSSDEBUGSEQ_H__
#define __BOSSDEBUGSEQ_H__

#include "sequence/GameInterface.h"

namespace MainSeq
{
	class C_BossDebugSeq : public I_MainSeq
	{
	public:
		C_BossDebugSeq();
		~C_BossDebugSeq();

		I_MainSeq* update( C_Parent* ip_parent );
	};
} // namespace MainSeq

#endif // __BOSSDEBUGSEQ_H__