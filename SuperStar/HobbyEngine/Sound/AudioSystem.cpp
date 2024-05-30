#include "Sound/audioSystem.h"

// FMODのAPIを利用
#include <fmod_studio.hpp>
#include <fmod_errors.h>

#include "SDL/SDL.h"

namespace Sound
{
	AudioSystem::AudioSystem()
	{
		this->_Clear();
	}

	bool AudioSystem::Initialize()
	{
		// ログ出力
		// todo: リリースでは実行しないようにする
		FMOD::Debug_Initialize(
			FMOD_DEBUG_LEVEL_ERROR,
			FMOD_DEBUG_MODE_TTY
		);

		// todo: 初期化
		FMOD_RESULT result = FMOD_OK;
		{
			result = FMOD::Studio::System::create(&this->pSystem);
			if (result != FMOD_OK)
			{
				SDL_Log("FMODシステムの初期化に失敗しました => %s", FMOD_ErrorString(result));
				return false;
			}
		}

		{
			result = this->pSystem->initialize(
				512,
				FMOD_STUDIO_INIT_NORMAL,
				FMOD_INIT_NORMAL,
				nullptr
			);

			if (result != FMOD_OK)
			{
				SDL_Log("FMODシステムの初期化に失敗しました => %s", FMOD_ErrorString(result));
				return false;
			}
		}

		{
			result = this->pSystem->getLowLevelSystem(&this->pLowLevelSystem);
			if (result != FMOD_OK)
			{
				SDL_Log("FMODシステムの初期化に失敗しました => %s", FMOD_ErrorString(result));
				return false;
			}
		}

		return false;
	}

	void AudioSystem::Shutdown()
	{
		this->pSystem->release();
	}

	void AudioSystem::Update(float/* in_deltaTime*/)
	{
		this->pSystem->update();
	}

	void AudioSystem::LoadBank(const std::string& in_rName)
	{
		// すでにロードしているかチェック
		if (this->banks.find(in_rName) != this->banks.end())
		{
			return;
		}

		// バンクのロード
		FMOD::Studio::Bank* pBank = nullptr;
		FMOD_RESULT result =
			this->pSystem->loadBankFile(
				in_rName.c_str(),
				FMOD_STUDIO_LOAD_BANK_NORMAL,
				&pBank);

		if (result != FMOD_OK)
		{
			return;
		}

		const int maxPathLength = 512;
		this->banks.emplace(in_rName, pBank);

		pBank->loadSampleData();
		int numEvents = 0;
		pBank->getEventCount(&numEvents);
		if (numEvents <= 0)
		{
			return;
		}

		std::vector<FMOD::Studio::EventDescription*> bankEvents(numEvents);
		pBank->getEventList(
			bankEvents.data(),
			numEvents,
			&numEvents);

		char aEventName[maxPathLength];
		for (int i = 0; i < numEvents; ++i)
		{
			auto e = bankEvents[i];
			e->getPath(aEventName, maxPathLength, nullptr);

			SDL_Log("FMOD EventName => %s", aEventName);

			this->events.emplace(aEventName, e);
		}
	}

	void AudioSystem::UnloadBank(const std::string& in_rName)
	{
		auto bankIter = this->banks.find(in_rName);
		if (this->banks.end() == bankIter)
		{
			return;
		}

		auto pBank = bankIter->second;

		// イベント解除
		int numEvents = 0;
		pBank->getEventCount(&numEvents);
		if (numEvents > 0)
		{
			std::vector<FMOD::Studio::EventDescription*> bankEvents(numEvents);
			pBank->getEventList(
				bankEvents.data(),
				numEvents,
				&numEvents);

			const int maxPathLength = 512;
			char aEventName[maxPathLength];
			for (int i = 0; i < numEvents; ++i)
			{
				auto e = bankEvents[i];
				e->getPath(aEventName, maxPathLength, nullptr);
				auto eventIter = this->events.find(aEventName);
				if (eventIter == this->events.end())
				{
					continue;
				}

				this->events.erase(eventIter);
			}
		}

		this->banks.erase(bankIter);

		pBank->unloadSampleData();
		pBank->unload();
	}

	void AudioSystem::UnloadAllBank()
	{
		for (auto bank : this->banks)
		{
			bank.second->unloadSampleData();
			bank.second->unload();
		}

		this->events.clear();
		this->banks.clear();
	}

	void AudioSystem::PlayEvent(const std::string& in_rName)
	{
		auto iter = this->events.find(in_rName);
		if (iter == this->events.end())
		{
			// 存在しないイベントを参照
			return;
		}

		FMOD::Studio::EventInstance* pEvent = nullptr;
		iter->second->createInstance(&pEvent);
		if (pEvent == nullptr)
		{
			// インスタンス作成失敗
			return;
		}

		pEvent->start();
		// releaseを呼び出しているが、呼び出し直後に開放はしない
		// 再生が終了してから解放。
		// 解放予約だね。
		pEvent->release();
	}
}