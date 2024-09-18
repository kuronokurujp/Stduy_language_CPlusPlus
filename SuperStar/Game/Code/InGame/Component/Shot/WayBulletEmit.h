/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/shot_sample/src/WayBulletEmit.h
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#ifndef __WAYBULLETEMIT_H__
#define __WAYBULLETEMIT_H__

#include "InterfaceBulletEmit.h"
#include "common/Common.h"

class C_NormalBullet;

class C_WayBulletEmit : public I_InterfaceBulletEmit
{
public:
    C_WayBulletEmit(const int i_way_num, const int i_bullet_max);
    ~C_WayBulletEmit();

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
    void Delete(const int in_Index)
    {
        int i = in_Index;
        _delete(i, m_shot_max);
    }

private:
    //	関数
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	変数
    C_NormalBullet** mpp_Bullet;
    float m_base_angle;
    int m_max;
    int m_shot_max;
    int m_interval;
    int m_way_num;
};

#endif  // __WAYBULLETEMIT_H__
