/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/shot_sample/src/NormalBulletEmit.cpp
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#include "NormalBulletEmit.h"

#include "bullet/NormalBullet.h"

const int gc_interval = 15;

C_NormalBulletEmit::C_NormalBulletEmit(const int i_bullet_max)
    : mpp_Bullet(NULL), m_max(i_bullet_max), m_shot_max(0), m_interval(0)
{
}
C_NormalBulletEmit::~C_NormalBulletEmit()
{
}
void C_NormalBulletEmit::init()
{
    int max    = m_max;
    mpp_Bullet = new C_NormalBullet*[max];
    ASSERT(mpp_Bullet);
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i] = new C_NormalBullet();
        mpp_Bullet[i]->init();
    }
}
void C_NormalBulletEmit::release()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->release();
        SAFE_DELETE(mpp_Bullet[i]);
    }
    SAFE_DELETE_ARRAY(mpp_Bullet);
}
void C_NormalBulletEmit::update()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        C_NormalBullet* p_actvie = mpp_Bullet[i];
        if (p_actvie->isShot() == true)
        {
            p_actvie->update();
        }
    }

    --m_interval;
    if (m_interval <= 0)
    {
        m_interval = 0;
    }
}
void C_NormalBulletEmit::draw() const
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        if (mpp_Bullet[i]->isShot() == true)
        {
            mpp_Bullet[i]->draw();
        }
    }
}

/*
        @brief	弾を撃つ
        @param	i_spos	:	弾の初期座標
*/
void C_NormalBulletEmit::shot(const D3DXVECTOR3& i_spos)
{
    if ((m_shot_max < m_max) && (m_interval <= 0))
    {
        m_interval = gc_interval;

        int max = m_max;
        for (int i = 0; i < max; ++i)
        {
            if (mpp_Bullet[i]->isShot() == false)
            {
                mpp_Bullet[i]->shot(i_spos);
                break;
            }
        }
    }
}
/*
        @brief	打つ角度
        @param	角度指定
*/
void C_NormalBulletEmit::setBulletAngle(float i_angle)
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->setBulletAngle(i_angle);
    }
}

/*
        @brief	交差判定
        @param	i_cpos		:	オブジェクトの中心座標
        @param	i_size_x	:	横サイズ
        @param	i_size_y	:	縦サイズ
        @return	衝突 : 弾のIndex	/	非衝突 : -1
*/
const int C_NormalBulletEmit::isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x,
                                            float i_size_y)
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        if (mpp_Bullet[i]->isShot() == true)
        {
            if (mpp_Bullet[i]->isIntersected(i_cpos, i_size_x, i_size_y) == true)
            {
                return i;
            }
        }
    }

    return -1;
}

/*
        @brief	弾を消す
*/
void C_NormalBulletEmit::Delete(const int in_Index)
{
    if (m_max > 0)
    {
        int index = in_Index;
        _delete(index, m_max);
    }
}

/*
        @brief	弾を消す
*/
void C_NormalBulletEmit::_delete(int& in_rIndex, int& in_rDataMax)
{
    if (in_rDataMax <= 0)
    {
        return;
    }

    C_NormalBullet* p_actvie = mpp_Bullet[in_rIndex];

    int delete_num = (in_rDataMax - in_rIndex) - 1;
    if (delete_num > 0)
    {
        ::memcpy(&mpp_Bullet[in_rIndex], &mpp_Bullet[in_rIndex + 1],
                 delete_num * sizeof(mpp_Bullet[0]));
        mpp_Bullet[in_rDataMax - 1] = p_actvie;
    }

    p_actvie->SetShotFlg(false);
}
