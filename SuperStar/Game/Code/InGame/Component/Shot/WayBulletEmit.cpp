/**
 *	@file 	D:/ProgramStady/Game/bom_panic/source/title/shot_sample/src/WayBulletEmit.cpp
 *	@brief
 *	@author	yuto uchida
 *	@data   2011-01-31
 *	@note
 *
 */
#include "WayBulletEmit.h"

#include "bullet/NormalBullet.h"
#include "common/Env.h"

const int gc_interval = 30;

C_WayBulletEmit::C_WayBulletEmit(const int i_way_num, const int i_bullet_max)
    : mpp_Bullet(NULL),
      m_base_angle(0.f),
      m_max(i_bullet_max * i_way_num),
      m_shot_max(0),
      m_interval(0),
      m_way_num(i_way_num)
{
    ASSERT(i_way_num > 0);
    ASSERT(i_bullet_max > 0);
}

C_WayBulletEmit::~C_WayBulletEmit()
{
}

void C_WayBulletEmit::init()
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

void C_WayBulletEmit::release()
{
    int max = m_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->release();
        SAFE_DELETE(mpp_Bullet[i]);
    }

    SAFE_DELETE_ARRAY(mpp_Bullet);
}

void C_WayBulletEmit::update()
{
    int max = m_shot_max;
    for (int i = 0; i < max; ++i)
    {
        C_NormalBullet* p_actvie = mpp_Bullet[i];
        p_actvie->update();

        if (p_actvie->isShot() == false)
        {
            Delete(i);
            max = m_shot_max;
        }
    }

    --m_interval;
    if (m_interval <= 0)
    {
        m_interval = 0;
    }
}

void C_WayBulletEmit::draw() const
{
    int max = m_shot_max;
    for (int i = 0; i < max; ++i)
    {
        mpp_Bullet[i]->draw();
    }
}

/*
        @brief	弾を撃つ
        @param	i_spos	:	弾の初期座標
*/
void C_WayBulletEmit::shot(const D3DXVECTOR3& i_spos)
{
    int shot_max = m_shot_max + m_way_num;
    if ((shot_max <= m_max) && (m_interval <= 0))
    {
        // 個数を超えることがある場合は打たない
        if (shot_max > m_max)
        {
            return;
        }

        m_interval = gc_interval;

        int set_idx = m_shot_max;
        int way_num = m_way_num;
        m_shot_max  = shot_max;

        float angle = m_base_angle;
        if (way_num > 2)
        {
            float add_angle = static_cast<float>(m_way_num >> 1);
            angle += (add_angle * 10.f);
        }

        if ((way_num % 2) == 0)
        {
            // 偶数
            angle -= 5.0f;
        }

        for (int i = set_idx; i < shot_max; ++i)
        {
            // 打つ角度を変える
            if (mpp_Bullet[i]->isShot() == false)
            {
                mpp_Bullet[i]->setBulletAngle(angle);
                mpp_Bullet[i]->shot(i_spos);
                angle -= 10.f;
            }
        }
    }
}

/*
        @brief	打つ角度
        @param	角度指定
*/
void C_WayBulletEmit::setBulletAngle(float i_angle)
{
    m_base_angle = i_angle;
}

/*
        @brief	交差判定
        @param	i_cpos		:	オブジェクトの中心座標
        @param	i_size_x	:	横サイズ
        @param	i_size_y	:	縦サイズ
        @return	衝突 : 弾のIndex	/	非衝突 : -1
*/
const int C_WayBulletEmit::isIntersected(const D3DXVECTOR3& i_cpos, float i_size_x, float i_size_y)
{
    int way_num = m_shot_max;
    for (int i = 0; i < way_num; ++i)
    {
        if (mpp_Bullet[i]->isIntersected(i_cpos, i_size_x, i_size_y) == true)
        {
            return i;
        }
    }

    return -1;
}

/*
        @brief	弾を消す
*/
void C_WayBulletEmit::_delete(int& in_rIndex, int& in_rDataMax)
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

    --in_rDataMax;
}
