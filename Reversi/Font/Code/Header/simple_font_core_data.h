#ifndef __FONT_LIB_SIMPLE_FONT_CORE_DATA_H__
#define __FONT_LIB_SIMPLE_FONT_CORE_DATA_H__

#include <assert.h>

#include "ft2build.h"
#include FT_FREETYPE_H

// フォントコアデータ
// 外部のフォントシステム「FreeType2」の利用している

// FreeType2について参考にしたサイト
// https://tnzk.hatenadiary.org/entry/20081005/1223208785
// http://blendgimper.hatenablog.jp/entry/2016/01/01/074615
class SimpleFontCoreData
{
public:
	SimpleFontCoreData()
	{
		this->_Clear();

		FT_Error error;
		error = FT_Init_FreeType(&this->_library);
		if (error != 0) {
			// TODO エラーになった
		}
	}

	~SimpleFontCoreData()
	{
		if (this->_library == nullptr)
			return;

		FT_Done_FreeType(this->_library);

		this->_library = nullptr;
	}

	inline const FT_Library GetLibrary() const {
		assert(this->_library);
		return this->_library;
	}

private:
	void _Clear()
	{
		this->_library = nullptr;
	}

	FT_Library _library;
};

#endif // __FONT_LIB_SIMPLE_FONT_CORE_DATA_H__
