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
            @brief	�łp�x
            @param	�p�x�w��
    */
    void setBulletAngle(float i_angle);

    /*
            @brief	�z�[�~���O�Ώۂ̍��W�w��
            @param	in_Idx			:	�ݒ肷��f�[�^Idx
            @param	i_target_pos	:	�Ώۍ��W
    */
    void setTargetPos(const int in_Idx, const D3DXVECTOR3& i_target_pos);

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

    //	�e�ő吔
    const int GetBulletMax() const { return m_max; }

private:
    //	�֐�
    void _delete(int& in_rIndex, int& in_rDataMax);

    //	�ϐ�
    C_HomingBullet** mpp_Bullet;
    float m_base_angle;
    int m_max;
    int m_shot_max;
};

#endif  // __HOMINGBULLETEMIT_H__
