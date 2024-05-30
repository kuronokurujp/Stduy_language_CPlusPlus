#ifndef __AUDIO_SYSTEM_H__
#define __AUDIO_SYSTEM_H__

#include <unordered_map>

// FMODのクラス前方宣言
namespace FMOD
{
	namespace Studio
	{
		class System;
		class Bank;
		class EventDescription;
	};

	class System;
};

namespace Sound
{
	class AudioSystem
	{
	public:
		AudioSystem();

		bool Initialize();
		void Shutdown();

		void Update(float in_deltaTime);

		void LoadBank(const std::string& in_rName);
		void UnloadBank(const std::string& in_rName);
		void UnloadAllBank();

		void PlayEvent(const std::string& in_rName);

	private:
		void _Clear()
		{
			this->pSystem = nullptr;
			this->pLowLevelSystem = nullptr;
		}

		FMOD::Studio::System* pSystem;
		FMOD::System* pLowLevelSystem;

		std::unordered_map<std::string, FMOD::Studio::Bank*> banks;
		std::unordered_map<std::string, FMOD::Studio::EventDescription*> events;
	};
}

#endif // __AUDIO_SYSTEM_H__
