#pragma once

#include "Engine/Core.h"
#include "Vector2.h"

namespace Core::Math
{
    // 2Dの矩形クラス
    class Rect2
    {
    public:
        /// <summary>
        /// 矩形の基準位置タイプ
        /// </summary>
        enum EAnchor
        {
            EAnchor_Left = 0,
            EAnchor_Center,
        };

        Rect2() { this->Clear(); }

        Rect2(const Float32 in_fX, const Float32 in_fY, const Float32 in_fW, const Float32 in_fH,
              const EAnchor in_eAnchor);

        void Set(const Float32 in_fX, const Float32 in_fY, const Float32 in_fW, const Float32 in_fH,
                 const EAnchor in_eAnchor);

        /// <summary>
        /// Gets the position.
        /// </summary>
        const Vector2 Pos() const;

        /// <summary>
        /// Widthes this instance.
        /// </summary>
        inline const Float32 Width() const HE_NOEXCEPT
        {
            return fabsf(this->_fRight - this->_fLeft);
        }

        /// <summary>
        /// Widthes the half.
        /// </summary>
        inline const Float32 WidthHalf() const HE_NOEXCEPT { return (this->Width() * 0.5f); }

        /// <summary>
        /// Heights this instance.
        /// </summary>
        inline const Float32 Height() const HE_NOEXCEPT
        {
            return fabsf(this->_fBottom - this->_fTop);
        }

        /// <summary>
        /// Heights the half.
        /// </summary>
        inline const Float32 HeightHalf() const HE_NOEXCEPT { return (this->Height() * 0.5f); }

        /// <summary>
        /// Ins the side rect.
        /// </summary>
        const Bool InSideRect(Rect2& in_rOrderRect);

        /// <summary>
        /// 座標が矩形の中に入っているか
        /// </summary>
        const Bool InSidePoint(const Vector2& in_rPos) const;

        /// <summary>
        /// Clears this instance.
        /// </summary>
        inline void Clear() HE_NOEXCEPT
        {
            this->_fLeft = this->_fRight = this->_fTop = this->_fBottom = 0.0f;
            this->_eAnchor                                              = EAnchor_Left;
        }

        /// <summary>
        /// Operator=s the specified a.
        /// </summary>
        inline void operator=(const Vector2& in_rVec) HE_NOEXCEPT
        {
            this->_SetPos(in_rVec._fX, in_rVec._fY);
        }

        /// <summary>
        /// Operator+=s the specified a.
        /// </summary>
        inline void operator+=(const Vector2& in_rVec) HE_NOEXCEPT
        {
            this->_fLeft += in_rVec._fX;
            this->_fRight += in_rVec._fX;

            this->_fTop += in_rVec._fY;
            this->_fBottom += in_rVec._fY;
        }

        Float32 _fLeft   = 0.0f;
        Float32 _fTop    = 0.0f;
        Float32 _fRight  = 0.0f;
        Float32 _fBottom = 0.0f;
        EAnchor _eAnchor = EAnchor_Left;

    private:
        /// <summary>
        /// Sets the position.
        /// </summary>
        void _SetPos(const Float32 in_fX, const Float32 in_fY);
    };
}  // namespace Core::Math
