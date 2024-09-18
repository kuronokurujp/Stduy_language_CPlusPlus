/**
 *	@file 	NormalBulletEmit.h
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#ifndef __NORMALBULLETEMIT_H__
#define __NORMALBULLETEMIT_H__

#include "InterfaceBulletEmit.h"
#include "common/Common.h"

class C_NormalBullet;

class C_NormalBulletEmit : public I_InterfaceBulletEmit
{
public:
    C_NormalBulletEmit(const int i_bullet_max);
    ~C_NormalBulletEmit();

    void init();
    void release();
    void update();
    void draw() const;

    /*
            @brief	打つ角度
            @param	角度指定
    */
    void setBulletAngle(float i_angle);

    /*
            @brief	弾を撃つ
            @param	i_spos	:	弾の初期座標
    */
    void shot(const D3DXVECTOR3& i_spos);

    /*
            @brief	交差判定
            @param	i_cpos		:	オブジェクトの中心座標
            @param	i_size_x	:	横サイズ
            @param	i_size_y	:	縦サイズ
            @return	衝突[true]	/	非衝突[false]
    */
    const int isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x, float i_size_y);

    //	弾を消す
    void Delete(const int in_Index);

private:
    //	関数
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	変数
    C_NormalBullet** mpp_Bullet;
    int m_max;
    int m_shot_max;
    int m_interval;
};

#endif  // __NORMALBULLETEMIT_H__
