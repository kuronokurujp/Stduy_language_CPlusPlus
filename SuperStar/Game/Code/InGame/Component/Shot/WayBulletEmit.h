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
            @brief	�łp�x
            @param	�p�x�w��
    */
    void setBulletAngle(float i_angle);

    /*
            @brief	�e������
            @param	i_spos	:	�e�̏������W
    */
    void shot(const D3DXVECTOR3& i_spos);

    /*
            @brief	��������
            @param	i_cpos		:	�I�u�W�F�N�g�̒��S���W
            @param	i_size_x	:	���T�C�Y
            @param	i_size_y	:	�c�T�C�Y
            @return	�Փ�[true]	/	��Փ�[false]
    */
    const int isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x, float i_size_y);

    //	�e������
    void Delete(const int in_Index)
    {
        int i = in_Index;
        _delete(i, m_shot_max);
    }

private:
    //	�֐�
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	�ϐ�
    C_NormalBullet** mpp_Bullet;
    float m_base_angle;
    int m_max;
    int m_shot_max;
    int m_interval;
    int m_way_num;
};

#endif  // __WAYBULLETEMIT_H__
