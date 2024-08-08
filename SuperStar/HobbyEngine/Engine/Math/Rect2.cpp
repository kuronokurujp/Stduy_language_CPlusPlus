#include "Rect2.h"

namespace Core::Math
{
    // 2Dの矩形クラス
    Rect2::Rect2(const Float32 in_fX, const Float32 in_fY, const Float32 in_fW, const Float32 in_fH,
                 const EAnchor in_eAnchor)
    {
        this->Set(in_fX, in_fY, in_fW, in_fH, in_eAnchor);
    }

    void Rect2::Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fW,
                    const Float32 in_fH, const EAnchor in_eAnchor)
    {
        this->Clear();

        switch (in_eAnchor)
        {
            case EAnchor_Left:
            {
                this->_fLeft   = in_fX;
                this->_fRight  = in_fX + in_fW;
                this->_fTop    = in_fY;
                this->_fBottom = in_fY + in_fH;

                break;
            }
            case EAnchor_Center:
            {
                auto halfW = in_fW * 0.5f;
                auto halfH = in_fH * 0.5f;

                this->_fLeft   = in_fX - halfW;
                this->_fRight  = in_fX + halfW;
                this->_fTop    = in_fY - halfH;
                this->_fBottom = in_fY + halfH;

                break;
            }
        }
        this->_eAnchor = in_eAnchor;
    }

    /// <summary>
    /// Gets the position.
    /// </summary>
    const Vector2 Rect2::Pos() const
    {
        Float32 fX, fY;
        switch (this->_eAnchor)
        {
            case EAnchor_Left:
            {
                fX = this->_fLeft;
                fY = this->_fTop;
                break;
            }
            case EAnchor_Center:
            {
                fX = this->_fLeft + (this->Width() * 0.5f);
                fY = this->_fTop + (this->Height() * 0.5f);

                break;
            }
        }
        return (Vector2(fX, fY));
    }

    /// <summary>
    /// Ins the side rect.
    /// </summary>
    const Bool Rect2::InSideRect(Rect2& in_rOrderRect)
    {
        const Vector2&& crrPos  = this->Pos();
        const Vector2&& crrLine = Vector2::Sub(crrPos, in_rOrderRect.Pos());

        const Float32 cfW = this->WidthHalf() + in_rOrderRect.WidthHalf();
        const Float32 cfH = this->HeightHalf() + in_rOrderRect.HeightHalf();

        if (cfW < fabs(crrLine._fX)) return FALSE;

        if (cfH < fabs(crrLine._fY)) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// 座標が矩形の中に入っているか
    /// </summary>
    const Bool Rect2::InSidePoint(const Vector2& in_rPos) const
    {
        if (this->_fRight < in_rPos._fX) return FALSE;

        if (in_rPos._fX < this->_fLeft) return FALSE;

        if (this->_fBottom < in_rPos._fY) return FALSE;

        if (in_rPos._fY < this->_fTop) return FALSE;

        return TRUE;
    }

    /// <summary>
    /// Sets the position.
    /// </summary>
    void Rect2::_SetPos(const Float32 in_fX, const Float32 in_fY)
    {
        switch (this->_eAnchor)
        {
            case EAnchor_Left:
            {
                this->_fLeft   = in_fX;
                this->_fRight  = in_fX + this->Width();
                this->_fTop    = in_fY;
                this->_fBottom = in_fY + this->Height();
                break;
            }
            case EAnchor_Center:
            {
                const Float32 halfW = this->WidthHalf();
                const Float32 halfH = this->HeightHalf();

                this->_fLeft   = in_fX - halfW;
                this->_fRight  = in_fX + halfW;
                this->_fTop    = in_fY - halfH;
                this->_fBottom = in_fY + halfH;

                break;
            }
        }
    }
}  // namespace Core::Math
