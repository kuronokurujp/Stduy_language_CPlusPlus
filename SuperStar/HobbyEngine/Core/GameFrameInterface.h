#ifndef __MINIGAME_INTERFACE_H__
#define __MINIGAME_INTERFACE_H__

#include "window.h"

/// <summary>
/// ゲームフレームインターフェイス
/// </summary>
class IInterface_GameFrame
{
public:
    /// <summary>
    /// Initializes the specified in r interface window.
    /// </summary>
    /// <param name="in_rInterfaceWindow">The in r interface window.</param>
    /// <returns></returns>
    virtual bool Initialize(IInterface_Window& in_rInterfaceWindow) = 0;

    /// <summary>
    /// Shutdowns this instance.
    /// </summary>
    /// <returns></returns>
    virtual void Shutdown(IInterface_Window& in_rInterfaceWindow) = 0;

    /// <summary>
    /// Updates this instance.
    /// </summary>
    /// <param name="in_rInterfaceWindow">The in r interface window.</param>
    /// <param name="in_deltaTime">The in delta time.</param>
    virtual void Update(IInterface_Window& in_rInterfaceWindow, float in_deltaTime) = 0;

    /// <summary>
    /// Draws this instance.
    /// </summary>
    virtual void Draw(IInterface_Window& in_rInterfaceWindow) = 0;
};

#endif  // __MINIGAME_INTERFACE_H__
