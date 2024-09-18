/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/shot_sample/src/HomingBulletEmit.h
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#ifndef __HOMINGBULLETEMIT_H__
#define __HOMINGBULLETEMIT_H__

#include "InterfaceBulletEmit.h"
#include "common/Common.h"

class C_HomingBullet;

class C_HomingBulletEmit : public I_InterfaceBulletEmit
{
public:
    C_HomingBulletEmit(const int i_bullet_max);
    ~C_HomingBulletEmit();

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
            @brief	ホーミング対象の座標指定
            @param	in_Idx			:	設定するデータIdx
            @param	i_target_pos	:	対象座標
    */
    void setTargetPos(const int in_Idx, const D3DXVECTOR3& i_target_pos);

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

    //	弾最大数
    const int GetBulletMax() const { return m_max; }

private:
    //	関数
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	変数
    C_HomingBullet** mpp_Bullet;
    float m_base_angle;
    int m_max;
    int m_shot_max;
};

#endif  // __HOMINGBULLETEMIT_H__
