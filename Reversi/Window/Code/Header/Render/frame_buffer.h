#ifndef __WINDOW_FRAME_BUFFER_H__
#define __WINDOW_FRAME_BUFFER_H__

/*
	画面のフレームバッファクラス
	NoneClipとついている関数はクリッピングチェックをしないので高速
*/
class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(const int in_width, const int in_height);
	virtual ~FrameBuffer();

	void Free();

	virtual bool Resize(const int in_width, const int in_height);

	/// <summary>
	/// バッファの一括クリア.
	/// </summary>
	void Cls();
	/// <summary>
	/// グレースケール値で一括クリア.
	/// </summary>
	/// <param name="in_grayscale_value">The in_grayscale_value.</param>
	void Cls(const unsigned char in_grayscale_value);

	/// <summary>
	/// 画素のアドレス取得.
	/// </summary>
	/// <param name="in_x">The in x.</param>
	/// <param name="in_y">The in y.</param>
	/// <returns></returns>
	virtual inline unsigned long* GetAddressPixel(const int in_x, const int in_y) {
		return (this->_width * in_y + in_x) + this->_p_buffer;
	}

	virtual inline const unsigned long* GetAddressPixel(const int in_x, const int in_y) const {
		return (this->_width * in_y + in_x) + this->_p_buffer;
	}

	/// <summary>
	/// Widthes this instance.
	/// </summary>
	/// <returns></returns>
	inline const int Width() const { return this->_width; }

	/// <summary>
	/// Heights this instance.
	/// </summary>
	/// <returns></returns>
	inline const int Height() const { return this->_height; }

	/// <summary>
	/// フレームのバッファ取得.
	/// </summary>
	/// <returns></returns>
	void* Buffer() { return this->_p_buffer; }
	const void* Buffer() const { return this->_p_buffer; }

	const int BufferSize() const { return this->_buffer_size; }

	/// <summary>
	/// 画素設定.
	/// </summary>
	/// <param name="in_x">The in x.</param>
	/// <param name="in_y">The in y.</param>
	/// <param name="color">The color.</param>
	inline void SetPixel(const int in_x, const int in_y, const unsigned long color)
	{
		unsigned long* p_address = (unsigned long*)this->GetAddressPixel(in_x, in_y);
		*p_address = color;
	}

	inline void SetPixel(const int in_x, const int in_y,
		const unsigned char in_r, const unsigned char in_g, const unsigned char in_b)
	{
		// ARGBをフォーマットとしている
		const unsigned long color = (in_r << 16) + (in_g << 8) + in_b;

		unsigned long* p_address = (unsigned long*)this->GetAddressPixel(in_x, in_y);
		*p_address = color;
	}

	/// <summary>
	/// 画素取得.
	/// </summary>
	/// <param name="in_x">The in x.</param>
	/// <param name="in_y">The in y.</param>
	/// <returns></returns>
	unsigned long GetPixel(const int in_x, const int in_y) const;

protected:
	virtual void _FreeBuffer();

	int _width, _height;
	//void* _p_buffer;
	unsigned long* _p_buffer;

	long _buffer_size;
	long _pixel_byte_size;

private:
	void _Clear();
};

#endif // __WINDOW_FRAME_BUFFER_H__
