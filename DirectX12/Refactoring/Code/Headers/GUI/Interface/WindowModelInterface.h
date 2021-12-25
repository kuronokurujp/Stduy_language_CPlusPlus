#pragma once

#include <stdio.h>
#include <memory>

namespace GUI
{
	/// <summary>
	/// Windowのデータモデルインターフェイス
	/// DirectX12やOpenGLなどの描画APIの扱いを吸収
	/// </summary>
	class WindowModelInterface
	{
	public:
		virtual ~WindowModelInterface() {}

		// 縦と横のウィンドウサイズ
		virtual inline UINT32 Width() = 0;
		virtual inline UINT32 Height() = 0;

		virtual const bool IsUpdate() = 0;
		virtual void EnableUpdate(const bool in_flag) = 0;

		// 描画バッファを更新
		virtual void UpdateRender() = 0;
	};
}
