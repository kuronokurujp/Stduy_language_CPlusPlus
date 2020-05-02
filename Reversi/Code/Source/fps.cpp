#include "fps.h"
#include "common.h"

#include <windows.h>

// timeGetTiem関数の利用に必要
#pragma comment(lib, "winmm.lib")

Fps::Fps()
{
	this->_Clear();
}

float Fps::WaitUpdate()
{
	const int timeAVGMax = StaticSingleArrayLength(this->_aPreviousTime);
	while (true)
	{
		// 16ms経過するまで待機する
		if (16 < (timeGetTime() - this->_aPreviousTime[timeAVGMax - 1]))
		{
			break;
		}
		else
		{
			Sleep(1);
		}
	}

	memcpy(this->_aPreviousTime, &this->_aPreviousTime[1], sizeof(this->_aPreviousTime) - 1);

	unsigned long currentTime = timeGetTime();
	this->_aPreviousTime[timeAVGMax - 1] = currentTime;

	unsigned long frameTime = currentTime - this->_aPreviousTime[timeAVGMax - 2];
	return frameTime / 1000.0f;
}

void Fps::_Clear()
{
	memset(this->_aPreviousTime, 0, sizeof(this->_aPreviousTime));
}