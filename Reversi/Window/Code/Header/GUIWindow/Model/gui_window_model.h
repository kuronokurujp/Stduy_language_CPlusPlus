#ifndef __GUI_WINDOW_MODEL_H__
#define __GUI_WINDOW_MODEL_H__

#include <stdio.h>
#include <memory>

#include "Render/frame_buffer.h"

#include "window_frame_buffer.h"

// GUIWindowのデータモデル
class GUIWindowModel
{
public:
	GUIWindowModel(const int in_w, const int in_h);
	~GUIWindowModel();

	inline int Width() { return this->_width; }
	inline int Height() { return this->_height; }
	inline std::shared_ptr<WindowFrameBuffer> ShaderWindowFrameBuffer() { return this->_window_frame_buffer; }

	// 描画バッファを更新
	virtual void UpdateRender();

protected:
	int _frame_buffer_count;
	FrameBuffer* _p_frame_buffer[2];

private:
	void _Clear()
	{
		this->_width = this->_height = 0;
		this->_frame_buffer_count = 0;

		this->_p_frame_buffer[0] = NULL;
		this->_p_frame_buffer[1] = NULL;
	}

	int _width, _height;
	// ウィンドウに転送する画像情報を転送するバッファ
	std::shared_ptr<WindowFrameBuffer> _window_frame_buffer;
};

#endif // __GUI_WINDOW_MODEL_H__
