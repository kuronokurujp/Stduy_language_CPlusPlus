#include "Window/Code/Header/Render/frame_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

FrameBuffer::FrameBuffer()
{
	this->_Clear();
}

FrameBuffer::FrameBuffer(const int in_width, const int in_height)
{
	this->_Clear();

	this->Resize(in_width, in_height);
	this->Cls();
}

FrameBuffer::~FrameBuffer()
{
	this->Free();
}

/// <summary>
/// バッファのリサイズをする.
/// </summary>
/// <param name="in_width">Width of the in.</param>
/// <param name="in_height">Height of the in.</param>
bool FrameBuffer::Resize(const int in_width, const int in_height)
{
	assert(in_width > 0);
	assert(in_height > 0);

	this->_FreeBuffer();

	this->_width = in_width;
	this->_height = in_height;

	// 画素はunsigned longの4byte前提
	this->_pixel_byte_size = sizeof(unsigned long);

	// バッファのサイズ計算
	this->_buffer_size = this->_width * this->_height * this->_pixel_byte_size;

	// バッファメモリ確保
	// 画素１つが4バイト前提
	this->_p_buffer = (unsigned long*)malloc(this->_buffer_size);

	return true;
}

void FrameBuffer::Cls()
{
	this->Cls(0);
}

void FrameBuffer::Cls(const unsigned char in_grayscale_value)
{
	assert(this->_p_buffer != NULL);
	memset(this->_p_buffer, in_grayscale_value, this->_buffer_size);
}

/// <summary>
/// 画素取得.
/// </summary>
/// <param name="in_x">The in x.</param>
/// <param name="in_y">The in y.</param>
/// <returns></returns>
unsigned long FrameBuffer::GetPixel(const int in_x, const int in_y) const
{
	assert(this->_p_buffer != NULL);

	const unsigned long* p_address = this->GetAddressPixel(in_x, in_y);
	return *p_address;
}

void FrameBuffer::_Clear()
{
	this->_p_buffer = NULL;
	this->_width = 0;
	this->_height = 0;
	this->_buffer_size = 0;
	this->_pixel_byte_size = 0;
}

/// <summary>
/// インスタンス解放時の後始末処理.
/// </summary>
void FrameBuffer::Free()
{
	this->_FreeBuffer();
}

/// <summary>
/// 確保したバッファ解放.
/// </summary>
void FrameBuffer::_FreeBuffer()
{
	if (this->_p_buffer == NULL)
		return;

	free(this->_p_buffer);
	this->_p_buffer = NULL;
}