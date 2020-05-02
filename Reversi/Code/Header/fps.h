#ifndef __FPS_H__
#define __FPS_H__

class Fps
{
public:
	Fps();

	/// <summary>
	/// 更新待ちをする.
	/// </summary>
	float WaitUpdate();

private:
	void _Clear();

	unsigned long _aPreviousTime[10] = { 0 };
};

#endif // __FPS_H__
