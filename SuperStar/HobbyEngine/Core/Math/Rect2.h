#pragma once

#include "Core/Core.h"

#include "Vector2.h"

namespace Core
{
    namespace Math
    {
        // 矩形クラス
        class Rect2
        {
        public:
            Rect2() {}

            Rect2(Float32 x, Float32 y, Sint32 w, Sint32 h)
            {
                this->Clear();

                Sint32 halfW = w >> 1;
                Sint32 halfH = h >> 1;

                this->left = x - static_cast<Float32>(halfW);
                this->right = x + static_cast<Float32>(halfW);
                this->top = y - static_cast<Float32>(halfH);
                this->bottom = y + static_cast<Float32>(halfH);
            }

            /// <summary>
            /// Gets the position.
            /// </summary>
            /// <returns></returns>
            const Vector2 Pos() const
            {
                Float32 x, y;
                x = this->left + static_cast<Float32>(this->Width() >> 1);
                y = this->top + static_cast<Float32>(this->Height() >> 1);

                return (Vector2(x, y));
            }

            /// <summary>
            /// Widthes this instance.
            /// </summary>
            /// <returns></returns>
            inline const Sint32 Width() const
            {
                return static_cast<Sint32>(fabsf(this->right - this->left));
            }

            /// <summary>
            /// Widthes the half.
            /// </summary>
            /// <returns></returns>
            inline const Sint32 WidthHalf() const
            {
                return (this->Width() >> 1);
            }

            /// <summary>
            /// Heights this instance.
            /// </summary>
            /// <returns></returns>
            inline const Sint32 Height() const
            {
                return static_cast<Sint32>(fabsf(this->bottom - this->top));
            }

            /// <summary>
            /// Ins the side rect.
            /// </summary>
            /// <param name="in_rOrderRect">The in r order rect.</param>
            /// <returns></returns>
            const Bool InSideRect(Rect2& in_rOrderRect)
            {
                const Vector2&& pos = this->Pos();
                const Vector2&& line = Vector2::Sub(pos, in_rOrderRect.Pos());

                const Float32 w = (Float32)(this->WidthHalf() + in_rOrderRect.WidthHalf());
                const Float32 h = (Float32)(this->HeightHalf() + in_rOrderRect.HeightHalf());

                if (w < fabs(line.x))
                    return FALSE;

                if (h < fabs(line.y))
                    return FALSE;

                return TRUE;
            }

            /// <summary>
            /// 座標が矩形の中に入っているか
            /// </summary>
            /// <param name="in_rPos"></param>
            /// <returns></returns>
            const Bool InSidePoint(const Vector2& in_rPos) const
            {
                if (this->right < in_rPos.x)
                    return FALSE;

                if (in_rPos.x < this->left)
                    return FALSE;

                if (this->bottom < in_rPos.y)
                    return FALSE;

                if (in_rPos.y < this->top)
                    return FALSE;

                return TRUE;
            }

            /// <summary>
            /// Heights the half.
            /// </summary>
            /// <returns></returns>
            inline const Sint32 HeightHalf() const
            {
                return (this->Height() >> 1);
            }

            /// <summary>
            /// Clears this instance.
            /// </summary>
            void Clear()
            {
                this->left = this->right = this->top = this->bottom = 0.0f;
            }

            /// <summary>
            /// Operator=s the specified a.
            /// </summary>
            /// <param name="a">a.</param>
            void operator=(const Vector2& a)
            {
                this->_SetPos(a.x, a.y);
            }

            /// <summary>
            /// Operator+=s the specified a.
            /// </summary>
            /// <param name="a">a.</param>
            inline void operator+=(const Vector2& a)
            {
                this->left += a.x;
                this->right += a.x;

                this->top += a.y;
                this->bottom += a.y;
            }

            Float32 left = 0.0f;
            Float32 top = 0.0f;
            Float32 right = 0.0f;
            Float32 bottom = 0.0f;

        private:
            /// <summary>
            /// Sets the position.
            /// </summary>
            /// <param name="x">The x.</param>
            /// <param name="y">The y.</param>
            void _SetPos(const Float32 x, const Float32 y)
            {
                Float32 halfW = static_cast<Float32>(this->Width() >> 1);
                Float32 halfH = static_cast<Float32>(this->Height() >> 1);

                this->left = x - halfW;
                this->right = x + halfW;
                this->top = y - halfH;
                this->bottom = y + halfH;
            }
        };
    }
}
