/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/shot_sample/src/HomingBulletEmit.cpp
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#include "HomingBulletEmit.h"

#include "bullet/HomingBullet.h"
#include "common/Env.h"
#include "debug/DebugScreenInterface.h"

static int s_HomingDebugHandle = DBG_SCENE_HANDLE_NON;

C_HomingBulletEmit::C_HomingBulletEmit(const int i_bullet_max)
    : mpp_Bullet(NULL), m_base_angle(0.f), m_max(i_bullet_max), m_shot_max(0)
{
    if (s_HomingDebugHandle == DBG_SCENE_HANDLE_NON)
    {
        s_HomingDebugHandle = DBG_SCENE_CREATE("Homing");
        C_HomingBullet::setDebug(s_HomingDebugHandle);
    }

    ASSERT(i_bullet_max > 0);
}

C_HomingBulletEmit::~C_HomingBulletEmit()
{
    if (s_HomingDebugHandle != DBG_SCENE_HANDLE_NON)
    {
        DBG_SCENE_DELETE(s_HomingDebugHandle);
        s_HomingDebugHandle = DBG_SCENE_HANDLE_NON;
    }
}

/*
        ������
*/
void C_HomingBulletEmit::init()
{
    int max    = m_max;
    mpp_Bullet = new C_HomingBullet*[max];
    ASSERT(mpp_Bullet);
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i] = new C_HomingBullet();
        mpp_Bullet[i]->init();
    }
}

/*
        �J��
*/
void C_HomingBulletEmit::release()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->release();
        SAFE_DELETE(mpp_Bullet[i]);
    }

    SAFE_DELETE_ARRAY(mpp_Bullet);
}

/*
        �X�V
*/
void C_HomingBulletEmit::update()
{
    int max = m_shot_max;
    for (int i = 0; i < max; ++i)
    {
        C_HomingBullet* p_actvie = mpp_Bullet[i];
        p_actvie->update();

        if (p_actvie->isShot() == false)
        {
            Delete(i);
            max = m_shot_max;
        }
    }
}

/*
        �`��
*/
void C_HomingBulletEmit::draw() const
{
    int max = m_shot_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->draw();
    }
}

/*
        @brief	�e������
        @param	i_spos	:	�e�̏������W
*/
void C_HomingBulletEmit::shot(const D3DXVECTOR3& i_spos)
{
    int set_idx = m_shot_max;
    int max     = m_max;
    if (set_idx <= 0)
    {
        for (int i = set_idx, angleIdx = 0; i < max; ++i, ++angleIdx)
        {
            mpp_Bullet[i]->setBulletAngle(m_base_angle + 90.f * angleIdx);
            mpp_Bullet[i]->shot(i_spos);

            ++m_shot_max;
        }
    }
}

/*
        @brief	�łp�x
        @param	�p�x�w��
*/
void C_HomingBulletEmit::setBulletAngle(float i_angle)
{
    m_base_angle = i_angle;
}

/*
        @brief	�z�[�~���O�Ώۂ̍��W�w��
        @param	in_Idx			:	�ݒ肷��f�[�^Idx
        @param	i_target_pos	:	�Ώۍ��W
*/
void C_HomingBulletEmit::setTargetPos(const int in_Idx, const D3DXVECTOR3& i_target_pos)
{
    int max = m_max;
    if (in_Idx >= max)
    {
        return;
    }

    if (mpp_Bullet[in_Idx]->isShot() == false)
    {
        mpp_Bullet[in_Idx]->setTargetPos(i_target_pos);
    }
}

/*
        @brief	��������
        @param	i_cpos		:	�I�u�W�F�N�g�̒��S���W
        @param	i_size_x	:	���T�C�Y
        @param	i_size_y	:	�c�T�C�Y
        @return	�Փ� : �e��Index	/	��Փ� : -1
*/
const int C_HomingBulletEmit::isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x,
                                            float i_size_y)
{
    int max = m_shot_max;
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
void C_HomingBulletEmit::_delete(int& in_rIndex, int& in_rDataMax)
{
    C_HomingBullet* p_actvie = mpp_Bullet[in_rIndex];

    int delete_num = (in_rDataMax - in_rIndex) - 1;
    if (delete_num > 0)
    {
        ::memcpy(&mpp_Bullet[in_rIndex], &mpp_Bullet[in_rIndex + 1],
                 delete_num * sizeof(mpp_Bullet[0]));
        mpp_Bullet[in_rDataMax - 1] = p_actvie;
    }

    p_actvie->SetShotFlg(false);

    --in_rDataMax;
}