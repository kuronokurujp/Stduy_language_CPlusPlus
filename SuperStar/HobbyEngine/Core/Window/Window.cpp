#include "gameWindow.h"
#include "gameFrameInterface.h"

#include "gameSystemInterface.h"
#include "Component/spriteComponent.h"
#include "Actor/actor.h"
#include "renderer.h"

#include <GL/glew.h>

/// <summary>
/// Initializes the specified in r window cofing data.
/// </summary>
/// <param name="in_rWindowCofingData">The in r window cofing data.</param>
/// <returns></returns>
bool GameWindow::Initalize(
	const WindowConfigData& in_rWindowCofingData,
	IInterface_GameSystemInterface* in_pInterfaecGameSystem)
{
	SDL_assert((in_pInterfaecGameSystem != nullptr));

	SDL_memcpy(&this->configData, &in_rWindowCofingData, sizeof(this->configData));

	this->pGame = in_pInterfaecGameSystem;

	this->pRenderer = new Renderer();
	this->pRenderer->Initalize(this->configData.w, this->configData.h);

	// windowを作成
	// openglを扱うようにする
	this->pWindow = SDL_CreateWindow(
		this->configData.pTitle,
		this->configData.x,
		this->configData.y,
		this->configData.w,
		this->configData.h,
		SDL_WINDOW_OPENGL
	);

	if (this->pWindow == nullptr)
	{
		SDL_Log("Falide to create window: %s", SDL_GetError());
		return false;
	}

	if (!this->pRenderer->CreateContext(this->pWindow))
	{
		return false;
	}

	this->closeFlag = false;

	return true;
}

/// <summary>
/// Shutdowns this instance.
/// </summary>
void GameWindow::Shutdown()
{
	// 登録したActorはすべて削除
	{
		this->DeleteAllActorsAndMemFree(this->pendingActors,
			[](Actor * /*in_pActor*/) { return true; });

		this->DeleteAllActorsAndMemFree(this->actors,
			[](Actor * /*in_pActor*/) { return true; });
	}

	if (this->pFrame != nullptr)
	{
		this->pFrame->Shutdown(*this);
		this->pFrame = nullptr;
	}

	if (this->pRenderer != nullptr)
	{
		this->pRenderer->Shutdown();
		SAFETY_MEM_RELEASE(this->pRenderer);
	}

	if (this->pWindow != nullptr)
	{
		SDL_DestroyWindow(this->pWindow);
		this->pWindow = nullptr;
	}
	else
	{
		SDL_Log("Unable to shutdown SDK to Unable to create window");
	}
}

/// <summary>
/// Loads this instance.
/// </summary>
/// <returns></returns>
bool GameWindow::Load()
{
	if (!this->pRenderer->Load())
	{
		return false;
	}

	return true;
}

/// <summary>
/// Attaches the frame.
/// 第一引数のポインタはnewしているので注意
/// </summary>
/// <param name="in_pGameFrame">The in p game frame.</param>
/// <param name="in_bResetFrame">The in b reset frame.</param>
/// <returns></returns>
const bool GameWindow::AttachFrame(class IInterface_GameFrame* in_pGameFrame, const bool in_bResetFrame)
{
	SDL_assert(in_pGameFrame != nullptr);
	if (this->pFrame != nullptr)
	{
		this->DetachFrame();
	}

	// ゲームフレームをアタッチして初期化
	this->pFrame = in_pGameFrame;
	if (in_bResetFrame)
	{
		if (this->pFrame->Initialize(*this) == false)
		{
			return false;
		}
	}
	else
	{
		// todo アタッチはするが初期化しないケースの対応がある
		SDL_assert(false);
	}

	return true;
}

/// <summary>
/// Detaches the frame.
/// </summary>
/// <returns></returns>
void GameWindow::DetachFrame()
{
	SDL_assert(this->pFrame != nullptr);

	this->pFrame->Shutdown(*this);
	SAFETY_MEM_RELEASE(this->pFrame);
}

/// <summary>
/// Updates this instance.
/// </summary>
/// <param name="in_deltaTime">The in delta time.</param>
void GameWindow::Update(float in_deltaTime)
{
	if (this->pFrame == nullptr)
	{
		return;
	}

	this->pFrame->Update(*this, in_deltaTime);

	// Actorの制御
	this->updatingActors = true;
	{
		for (auto actor : this->actors)
		{
			actor->Update(in_deltaTime);
		}

		// コリジョン処理(コリジョンしてActor追加が起きてもpendingするように)
		this->_Colision();
	}
	this->updatingActors = false;

	// キャッシュに登録しているActorを更新用のリストに移行する
	for (auto pending : this->pendingActors)
	{
		// 更新する前に座標更新
		pending->ComputeWorldTransform();
		this->actors.emplace_back(pending);
	}
	this->pendingActors.clear();

	// Actorを削除するかチェック
	{
		this->DeleteAllActorsAndMemFree(this->actors, [](Actor *in_pValidateActor)
		{
			if (in_pValidateActor->GetState() == Actor::EState_Dead)
			{
				return true;
			}

			return false;
		});
	}
}

/// <summary>
/// Draws this instance.
/// </summary>
void GameWindow::GenerateOutput()
{
	this->pRenderer->BeginDraw(this->configData.screenColor);

	{
		this->pRenderer->Draw();

		if (this->pFrame != nullptr)
		{
			this->pFrame->Draw(*this);
		}
	}

	this->pRenderer->EndDraw(this->pWindow);
}

/// <summary>
/// Renders the fill rect.
/// </summary>
/// <param name="in_rect">The in rect.</param>
void GameWindow::RenderFillRect(const SDL_Rect& /*in_rect*/)
{
	//	SDL_RenderFillRect(this->pRenderer, &in_rect);
}

/// <summary>
/// Sets the state of the keyboard.
/// </summary>
/// <param name="in_pState">State of the in p.</param>
void GameWindow::SetKeyboardState(const Uint8* in_pState, SDL_Event in_eventData)
{
	// アクターにキーボード入力結果を渡す
	this->updatingActors = true;
	{
		for (auto actor : this->actors)
		{
			actor->ProcessInput(in_pState);
		}
	}
	this->updatingActors = false;

	this->input.SetKeyboardState(in_pState, in_eventData);
}

/// <summary>
/// Closes this instance.
/// </summary>
void GameWindow::Close()
{
	this->closeFlag = true;
}

/// <summary>
/// Colisions this instance.
/// </summary>
/// <returns></returns>
void GameWindow::_Colision()
{
}