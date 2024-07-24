#pragma once

#include "Engine/Core.h"
#include "Vector2.h"

namespace Core
{
    namespace Math
    {
        // 2Dの矩形クラス
        class Rect2
        {
        public:
            /// <summary>
            /// 矩形の基準位置タイプ
            /// </summary>
            enum EPivot
            {
                EPivot_Left = 0,
                EPivot_Center,
            };

            Rect2() {}

            Rect2(const Float32 in_fX, const Float32 in_fY, const Float32 in_fW,
                  const Float32 in_fH, const EPivot in_ePivot)
            {
                this->Clear();

                switch (in_ePivot)
                {
                    case EPivot_Left:
                    {
                        this->_fLeft   = in_fX;
                        this->_fRight  = in_fX + in_fW;
                        this->_fTop    = in_fY;
                        this->_fBottom = in_fY + in_fH;

                        break;
                    }
                    case EPivot_Center:
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
                this->_ePivot = in_ePivot;
            }

            /// <summary>
            /// Gets the position.
            /// </summary>
            const Vector2 Pos() const HE_NOEXCEPT
            {
                Float32 fX, fY;
                switch (this->_ePivot)
                {
                    case EPivot_Left:
                    {
                        fX = this->_fLeft;
                        fY = this->_fTop;
                        break;
                    }
                    case EPivot_Center:
                    {
                        fX = this->_fLeft + (this->Width() * 0.5f);
                        fY = this->_fTop + (this->Height() * 0.5f);

                        break;
                    }
                }
                return (Vector2(fX, fY));
            }

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
            const Bool InSideRect(Rect2& in_rOrderRect) HE_NOEXCEPT
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
            const Bool InSidePoint(const Vector2& in_rPos) const HE_NOEXCEPT
            {
                if (this->_fRight < in_rPos._fX) return FALSE;

                if (in_rPos._fX < this->_fLeft) return FALSE;

                if (this->_fBottom < in_rPos._fY) return FALSE;

                if (in_rPos._fY < this->_fTop) return FALSE;

                return TRUE;
            }

            /// <summary>
            /// Clears this instance.
            /// </summary>
            void Clear() HE_NOEXCEPT
            {
                this->_fLeft = this->_fRight = this->_fTop = this->_fBottom = 0.0f;
            }

            /// <summary>
            /// Operator=s the specified a.
            /// </summary>
            void operator=(const Vector2& in_rVec) HE_NOEXCEPT
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
            EPivot _ePivot   = EPivot_Left;

        private:
            /// <summary>
            /// Sets the position.
            /// </summary>
            void _SetPos(const Float32 in_fX, const Float32 in_fY) HE_NOEXCEPT
            {
                switch (this->_ePivot)
                {
                    case EPivot_Left:
                    {
                        this->_fLeft   = in_fX;
                        this->_fRight  = in_fX + this->Width();
                        this->_fTop    = in_fY;
                        this->_fBottom = in_fY + this->Height();
                        break;
                    }
                    case EPivot_Center:
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
        };
    }  // namespace Math
}  // namespace Core
