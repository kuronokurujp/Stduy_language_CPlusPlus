#include "GUIWindow/Model/gui_window_model.h"

// GUIWindowのデータモデル
GUIWindowModel::GUIWindowModel(const int in_w, const int in_h)
{
	this->_Clear();

	this->_width = in_w;
	this->_height = in_h;

	// アプリ側が書き込む画像情報
	// ダブルバッファを用意
	this->_p_frame_buffer[0] =
		new FrameBuffer(this->_width, this->_height);
	this->_p_frame_buffer[1] =
		new FrameBuffer(this->_width, this->_height);

	// ウィンドウに転送する画像情報を転送するバッファ
	this->_window_frame_buffer = std::make_shared<WindowFrameBuffer>(this->_width, this->_height);
}

GUIWindowModel::~GUIWindowModel()
{
	for (int i = 0; i < 2; ++i)
	{
		delete this->_p_frame_buffer[i];
		this->_p_frame_buffer[i] = NULL;
	}

	this->_window_frame_buffer->Free();
}

// 描画バッファを更新

void GUIWindowModel::UpdateRender()
{
	// カレントフレームバッファで書き込む
	this->_window_frame_buffer->Flash(this->_p_frame_buffer[this->_frame_buffer_count]);
	// バッファを切り替え
	this->_frame_buffer_count = (this->_frame_buffer_count + 1) % 2;
}