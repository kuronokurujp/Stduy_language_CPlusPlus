#ifndef __GAME_SYTEM_INTERFACE_H__
#define __GAME_SYTEM_INTERFACE_H__

#include <vector>

#include "Sound/audioSystem.h"

/// <summary>
/// ゲームシステムのインターフェイス
/// </summary>
class IInterface_PublicEngine
{
public:
    /// <summary>
    /// Outputs the windows list.
    /// </summary>
    /// <param name="in_rOutputList">The in r output list.</param>
    /// <returns></returns>
    virtual void OutputWindows(std::vector<class IInterface_Window*>& in_rOutputList) = 0;

    /// <summary>
    /// Gets the audio system.
    /// </summary>
    /// <returns></returns>
    virtual Sound::AudioSystem* GetAudioSystem() = 0;
};

#endif  // __GAME_SYTEM_INTERFACE_H__