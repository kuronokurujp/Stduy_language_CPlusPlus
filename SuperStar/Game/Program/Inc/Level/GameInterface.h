/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/seq_sample/src/sequence/GameInterface.h
 *	@brief	
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note 
 *
 */
#ifndef __GameInterface_H__
#define __GameInterface_H__

namespace MainSeq
{
	class C_Parent;

	// ゲームシーケンスのインターフェイス
	class I_MainSeq
	{
	public:
		I_MainSeq() {}
		virtual ~I_MainSeq() {}
	
		virtual I_MainSeq*	update( C_Parent* ip_parent ) = 0;
	};

} // MainSeq

#endif // __GameInterface_H__
