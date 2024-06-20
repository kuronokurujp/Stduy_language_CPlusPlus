/**
 *	@file 	PlaySeq.h
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note
 *
 */
#ifndef __PlaySeq_H__
#define __PlaySeq_H__

#include "sequence/GameInterface.h"

namespace MainSeq
{
    class C_PlaySeq : public I_MainSeq
    {
    public:
        C_PlaySeq();
        ~C_PlaySeq();

        I_MainSeq* update(C_Parent* ip_parent);
    };
}  // namespace MainSeq

#endif  // __PlaySeq_H__
