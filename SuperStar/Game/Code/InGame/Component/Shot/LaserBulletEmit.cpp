/**
 *	@file 	LaserBulletEmit.cpp
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-12-27
 *	@note
 *
 */
#include "LaserBulletEmit.h"

#include "bullet/LaserBullet.h"

C_LaserBulletEmit::C_LaserBulletEmit(const int i_bullet_max)
    : mpp_Bullet(NULL), m_max(i_bullet_max), m_shot_max(0), m_interval(0)
{
}

C_LaserBulletEmit::~C_LaserBulletEmit()
{
}

void C_LaserBulletEmit::init()
{
    int max    = m_max;
    mpp_Bullet = new C_LaserBullet*[max];
    ASSERT(mpp_Bullet);
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i] = new C_LaserBullet();
        mpp_Bullet[i]->init();
    }
}

void C_LaserBulletEmit::release()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->release();
        SAFE_DELETE(mpp_Bullet[i]);
    }

    SAFE_DELETE_ARRAY(mpp_Bullet);
}

void C_LaserBulletEmit::update()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        C_LaserBullet* p_actvie = mpp_Bullet[i];
        p_actvie->update();
    }
}

void C_LaserBulletEmit::draw() const
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->draw();
    }
}

/*
        @brief	�e������
        @param	i_spos	:	�e�̏������W
*/
void C_LaserBulletEmit::shot(const D3DXVECTOR3& i_spos)
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->shot(i_spos);
    }
}

/*
        @brief	�łp�x
        @param	�p�x�w��
*/
void C_LaserBulletEmit::setBulletAngle(float i_angle)
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->setBulletAngle(i_angle);
    }
}

/*
        @brief	��������
        @param	i_cpos		:	�I�u�W�F�N�g�̒��S���W
        @param	i_size_x	:	���T�C�Y
        @param	i_size_y	:	�c�T�C�Y
        @return	�Փ� : �e��Index	/	��Փ� : -1
*/
const int C_LaserBulletEmit::isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x,
                                           float i_size_y)
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        if (mpp_Bullet[i]->isIntersected(i_cpos, i_size_x, i_size_y) == true)
        {
            return i;
        }
    }

    return -1;
}

/*
        @brief	�e������
*/
void C_LaserBulletEmit::_delete(int& in_rIndex, int& in_rDataMax)
{
    C_LaserBullet* p_actvie = mpp_Bullet[in_rIndex];

    int delete_num = (in_rDataMax - in_rIndex) - 1;
    if (delete_num > 0)
    {
        ::memcpy(&mpp_Bullet[in_rIndex], &mpp_Bullet[in_rIndex + 1],
                 delete_num * sizeof(mpp_Bullet[0]));
        mpp_Bullet[in_rDataMax - 1] = p_actvie;
    }

    --in_rIndex;
    --in_rDataMax;
}
