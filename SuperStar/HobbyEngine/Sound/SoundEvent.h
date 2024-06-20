#ifndef __SOUND_EVENT_H__
#define __SOUND_EVENT_H__

namespace Sound
{
    class AudioSystem;

    class SoundEvent
    {
    public:
        SoundEvent() {}

    private:
        friend AudioSystem;

        SoundEvent(AudioSystem* in_pAudioSystem, int in_handle) {}
    };
}  // namespace Sound

#endif  // __SOUND_EVENT_H__
