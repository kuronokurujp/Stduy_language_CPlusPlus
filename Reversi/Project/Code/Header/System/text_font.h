#ifndef __SYSTEM_TEXT_FONT_H__
#define __SYSTEM_TEXT_FONT_H__

#include <stdio.h>

#include "Font/Code/Header/font_text_data_interface.h"

class TextFont
{
public:
	TextFont() {
		this->_Clear();
	}

	~TextFont() {
	}

	// TODO: アクセス子は後で修正
	float _x, _y;
	char _text[256];

private:
	void _Clear()
	{
		this->_x = this->_y = 0.f;
		memset(this->_text, 0, sizeof(this->_text));
	}
};

#endif // __SYSTEM_TEXT_FONT_H__
