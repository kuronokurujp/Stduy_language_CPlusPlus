#include "InGameBulletNormal.h"

#include "Engine/Common/Hash.h"

// 依存するモジュール一覧
#include "RenderModule.h"

namespace InGame
{
    namespace Local
    {
        static const Char* szObjectName = HE_STR_TEXT("InGameBulletNormal");
    }

    const Char* InGameBulletNormalFactory::VName() const
    {
        return Local::szObjectName;
    }

    void InGameBulletNormalFactory::VConfiguration(InGameBulletWork* in_pWork)
    {
        InGameBulletNormalWork* pWork = reinterpret_cast<InGameBulletNormalWork*>(in_pWork->aWork);
        pWork->pos                    = this->_pos;
        pWork->dir                    = this->_dir;
        pWork->fSpeed                 = 1.0f;
        pWork->fFirstSpeed            = 6.0f;
        pWork->fAccele                = 1.0f;
        pWork->fSize                  = 32.0f;
        in_pWork->uCollisionHashCode  = this->_uCollisionHashCode;
    }

    Uint32 InGameBulletNormalStrategy::VWorkSize() const
    {
        return sizeof(InGameBulletNormalWork);
    }

    const Char* InGameBulletNormalStrategy::VName() const
    {
        return Local::szObjectName;
    }

    Bool InGameBulletNormalStrategy::VUpdate(InGameBulletWork* in_pWork,
                                             Core::Common::Handle& in_rViewHandle)
    {
        InGameBulletNormalWork* pWork = reinterpret_cast<InGameBulletNormalWork*>(in_pWork->aWork);

        Core::Math::Vector2 shotVec = pWork->dir * pWork->fSpeed;
        pWork->pos += shotVec;

        // 描画コマンド追加
        // TODO: 描画内容を動的に切り替える
        // 円描画や矩形描画とか
        Render::Command2DCircleDraw(in_rViewHandle, pWork->pos, pWork->fSize * 0.5f,
                                    Render::RGB::White);

        return TRUE;
    }

    Bool InGameBulletNormalStrategy::VIsScreenInSide(const InGameBulletWork* const in_pWork,
                                                     const Core::Math::Rect2& in_rScreenInSide)
    {
        const InGameBulletNormalWork* const pWork =
            reinterpret_cast<const InGameBulletNormalWork* const>(in_pWork->aWork);

        if (in_rScreenInSide.InSidePoint(pWork->pos))
        {
            return TRUE;
        }

        return FALSE;
    }

    Bool InGameBulletNormalStrategy::VOutputCollisionData(CollisionData* out,
                                                          const InGameBulletWork* const in_pObj)
    {
        const InGameBulletNormalWork* const pWork =
            reinterpret_cast<const InGameBulletNormalWork* const>(in_pObj->aWork);

        out->eType                 = ECollisionType::ECollisionType_Circle2D;
        out->data.circle2D.fRadius = pWork->fSize * 0.5f;
        out->data.circle2D.pos     = pWork->pos;
        out->uHashCode             = in_pObj->uCollisionHashCode;
        HE_ASSERT(out->uHashCode != 0);

        return TRUE;
    }

}  // namespace InGame
