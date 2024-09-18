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
    void Delete(const int in_Index);

private:
    //	�֐�
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	�ϐ�
    C_NormalBullet** mpp_Bullet;
    int m_max;
    int m_shot_max;
    int m_interval;
};

#endif  // __NORMALBULLETEMIT_H__
