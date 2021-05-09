#ifndef __FONT_LIB_FONT_TEXT_DATA_INTERFACE_H__
#define __FONT_LIB_FONT_TEXT_DATA_INTERFACE_H__

#include "ft2build.h"
#include FT_FREETYPE_H

// フォントテキストデータのインターフェイス
class FontTextData_Interface
{
public:
	// FreeType2の構造体を返す作りになっているので蜜結合になってしまっている
	virtual inline FT_Bitmap* LoadFontChar(unsigned long in_char_utf32) = 0;
	// 文字１つのフォント情報を取得
	virtual inline FT_Bitmap* GetFontCharData(unsigned long in_char_utf32) = 0;
};

#endif // __FONT_LIB_FONT_TEXT_DATA_INTERFACE_H__
