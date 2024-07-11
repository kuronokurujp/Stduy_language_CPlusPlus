#pragma once

#include "Core/Core.h"
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

            Rect2(const Float32 x, const Float32 y, const Float32 w, const Float32 h,
                  const EPivot in_pivot)
            {
                this->Clear();

                switch (in_pivot)
                {
                    case EPivot_Left:
                    {
                        this->left   = x;
                        this->right  = x + w;
                        this->top    = y;
                        this->bottom = y + h;

                        break;
                    }
                    case EPivot_Center:
                    {
                        auto halfW = w * 0.5f;
                        auto halfH = h * 0.5f;

                        this->left   = x - halfW;
                        this->right  = x + halfW;
                        this->top    = y - halfH;
                        this->bottom = y + halfH;

                        break;
                    }
                }
                this->pivot = in_pivot;
            }

            /// <summary>
            /// Gets the position.
            /// </summary>
            /// <returns></returns>
            const Vector2 Pos() const E_NOEXCEPT
            {
                Float32 x, y;
                switch (this->pivot)
                {
                    case EPivot_Left:
                    {
                        x = this->left;
                        y = this->top;
                        break;
                    }
                    case EPivot_Center:
                    {
                        x = this->left + (this->Width() * 0.5f);
                        y = this->top + (this->Height() * 0.5f);

                        break;
                    }
                }
                return (Vector2(x, y));
            }

            /// <summary>
            /// Widthes this instance.
            /// </summary>
            /// <returns></returns>
            inline const Float32 Width() const E_NOEXCEPT
            {
                return fabsf(this->right - this->left);
            }

            /// <summary>
            /// Widthes the half.
            /// </summary>
            /// <returns></returns>
            inline const Float32 WidthHalf() const E_NOEXCEPT { return (this->Width() * 0.5f); }

            /// <summary>
            /// Heights this instance.
            /// </summary>
            /// <returns></returns>
            inline const Float32 Height() const E_NOEXCEPT
            {
                return fabsf(this->bottom - this->top);
            }

            /// <summary>
            /// Heights the half.
            /// </summary>
            /// <returns></returns>
            inline const Float32 HeightHalf() const E_NOEXCEPT { return (this->Height() * 0.5f); }

            /// <summary>
            /// Ins the side rect.
            /// </summary>
            /// <param name="in_rOrderRect">The in r order rect.</param>
            /// <returns></returns>
            const Bool InSideRect(Rect2& in_rOrderRect) E_NOEXCEPT
            {
                const Vector2&& pos  = this->Pos();
                const Vector2&& line = Vector2::Sub(pos, in_rOrderRect.Pos());

                const Float32 w = this->WidthHalf() + in_rOrderRect.WidthHalf();
                const Float32 h = this->HeightHalf() + in_rOrderRect.HeightHalf();

                if (w < fabs(line.x)) return FALSE;

                if (h < fabs(line.y)) return FALSE;

                return TRUE;
            }

            /// <summary>
            /// 座標が矩形の中に入っているか
            /// </summary>
            /// <param name="in_rPos"></param>
            /// <returns></returns>
            const Bool InSidePoint(const Vector2& in_rPos) const E_NOEXCEPT
            {
                if (this->right < in_rPos.x) return FALSE;

                if (in_rPos.x < this->left) return FALSE;

                if (this->bottom < in_rPos.y) return FALSE;

                if (in_rPos.y < this->top) return FALSE;

                return TRUE;
            }

            /// <summary>
            /// Clears this instance.
            /// </summary>
            void Clear() E_NOEXCEPT { this->left = this->right = this->top = this->bottom = 0.0f; }

            /// <summary>
            /// Operator=s the specified a.
            /// </summary>
            /// <param name="a">a.</param>
            void operator=(const Vector2& a) E_NOEXCEPT { this->_SetPos(a.x, a.y); }

            /// <summary>
            /// Operator+=s the specified a.
            /// </summary>
            /// <param name="a">a.</param>
            inline void operator+=(const Vector2& a) E_NOEXCEPT
            {
                this->left += a.x;
                this->right += a.x;

                this->top += a.y;
                this->bottom += a.y;
            }

            Float32 left   = 0.0f;
            Float32 top    = 0.0f;
            Float32 right  = 0.0f;
            Float32 bottom = 0.0f;
            EPivot pivot   = EPivot_Left;

        private:
            /// <summary>
            /// Sets the position.
            /// </summary>
            /// <param name="x">The x.</param>
            /// <param name="y">The y.</param>
            void _SetPos(const Float32 x, const Float32 y) E_NOEXCEPT
            {
                switch (this->pivot)
                {
                    case EPivot_Left:
                    {
                        this->left   = x;
                        this->right  = x + this->Width();
                        this->top    = y;
                        this->bottom = y + this->Height();
                        break;
                    }
                    case EPivot_Center:
                    {
                        const Float32 halfW = this->WidthHalf();
                        const Float32 halfH = this->HeightHalf();

                        this->left   = x - halfW;
                        this->right  = x + halfW;
                        this->top    = y - halfH;
                        this->bottom = y + halfH;

                        break;
                    }
                }
            }
        };
    }  // namespace Math
}  // namespace Core
