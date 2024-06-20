#ifndef __GAME_WINDOW_H__
#define __GAME_WINDOW_H__

#include <vector>

#include "Actor/actorManager.h"
#include "input.h"
#include "window.h"

class IInterface_GameFrame;
class IInterface_GameSystemInterface;
class Renderer;
class ActorManager;

/// <summary>
/// 画面のウィンドウ
/// </summary>
class GameWindow : public IInterface_Window, ActorManager
{
public:
    GameWindow() { this->_Clear(); }

    /// <summary>
    /// Initializes the specified in r window cofing data.
    /// </summary>
    /// <param name="in_rWindowCofingData">The in r window cofing data.</param>
    /// <returns></returns>
    bool Initalize(const WindowConfigData& in_rWindowCofingData,
                   IInterface_GameSystemInterface* in_pGameSystemInterfaec);

    /// <summary>
    /// Shutdowns this instance.
    /// </summary>
    void Shutdown();

    bool Load();

    /// <summary>
    /// Attaches the frame.
    /// </summary>
    /// <param name="in_pGameFrame">The in p game frame.</param>
    /// <param name="in_bResetFrame">The in b reset frame.</param>
    /// <returns></returns>
    const bool AttachFrame(IInterface_GameFrame* in_pGameFrame, const bool in_bResetFrame);

    /// <summary>
    /// Detaches the frame.
    /// </summary>
    /// <returns></returns>
    void DetachFrame();

    /// <summary>
    /// Updates this instance.
    /// </summary>
    void Update(float in_deltaTime);

    /// <summary>
    /// Draws this instance.
    /// </summary>
    void GenerateOutput();

    /// <summary>
    /// Gets the configuration data.
    /// </summary>
    /// <returns></returns>
    inline const WindowConfigData& GetReferenceConfigData() override final
    {
        return this->configData;
    }

    /// <summary>
    /// Renders the fill rect.
    /// </summary>
    /// <param name="in_rect">The in rect.</param>
    void RenderFillRect(const SDL_Rect& in_rect) override final;

    /// <summary>
    /// Gets the input.
    /// </summary>
    /// <returns></returns>
    IInterface_Input* GetInput() override final { return &this->input; }

    /// <summary>
    /// Sets the state of the keyboard.
    /// </summary>
    /// <param name="in_pState">State of the in p.</param>
    void SetKeyboardState(const Uint8* in_pState, SDL_Event in_eventData);

    /// <summary>
    /// Closes this instance.
    /// </summary>
    void Close();

    /// <summary>
    /// Determines whether this instance is close.
    /// </summary>
    /// <returns>
    ///   <c>true</c> if this instance is close; otherwise, <c>false</c>.
    /// </returns>
    const bool IsClose() { return this->closeFlag; }

    /// <summary>
    /// Gets the game.
    /// </summary>
    /// <returns></returns>
    IInterface_GameSystemInterface* GetGame() { return this->pGame; };

    Renderer* GetRenderer() override { return this->pRenderer; }
    ActorManager* GetActorManager() override { return this; }

private:
    /// <summary>
    /// Colisions this instance.
    /// </summary>
    /// <returns></returns>
    void _Colision();

    /// <summary>
    /// Clears this instance.
    /// </summary>
    void _Clear()
    {
        this->closeFlag = true;
        this->pWindow   = nullptr;
        this->pRenderer = nullptr;
        this->pFrame    = nullptr;
        this->pGame     = nullptr;
        this->pRenderer = nullptr;
        SDL_zero(this->configData);
    }

    bool closeFlag;

    SDL_Window* pWindow;

    // メソッドの引数で渡すことになるが、参照なので元のデータが消えたらハングするので注意！
    IInterface_GameFrame* pFrame;
    IInterface_GameSystemInterface* pGame;

    Renderer* pRenderer;

    Windows_Input input;

    WindowConfigData configData;
};

#endif  // __GAME_WINDOW_H__