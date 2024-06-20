/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/seq_sample/src/sequence/GameParent.h
 *	@brief	ゲーム全体のシーケンス管理
 *	@author	yuto uchida
 *	@data   2011-02-17
 *	@note
 *
 */
#ifndef __GameParent_H__
#define __GameParent_H__

namespace MainSeq
{
    class I_MainSeq;

    class C_Parent
    {
    public:
        C_Parent(I_MainSeq* ip_FirstSeq);
        ~C_Parent();

        /*
                @brief	シーケンス更新
        */
        bool update();

    private:
        I_MainSeq* mp_seq;
    };
}  // namespace MainSeq

#endif  // __GameParent_H__
