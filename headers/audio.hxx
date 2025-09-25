#ifndef AUDIO_HXX
#define AUDIO_HXX

#define MINIAUDIO_IMPLEMENTATION
#include "../deps/miniaudio.h"
#include <vector>
#include <iostream>
#include <map>

class audio_player_struct
{
private:
    static audio_player_struct audio_player_instance;

    ma_result result;
    ma_engine engine;
    std::map<std::string, ma_sound> sounds;

public:
    static audio_player_struct *getInstance()
    {
        return &audio_player_instance;
    }

    audio_player_struct()
    {
        result = ma_engine_init(NULL, &engine);
        if (result != MA_SUCCESS)
        {
            std::cout << "Error: audio engine failed to initiate." << std::endl;
        }
    }

    void load_audio(std::string id, std::string path)
    {
        sounds[id] = ma_sound();
        result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, &sounds[id]);
        if (result != MA_SUCCESS)
        {
            std::cout << "Error: audio at path " << path << " failed to load." << std::endl;
            return;
        }
    }
    void play_audio(std::string id)
    {
        ma_sound_seek_to_pcm_frame(&sounds[id], 0);
        ma_sound_start(&sounds[id]);
    }
    void stop_audio(std::string id)
    {
        ma_sound_stop(&sounds[id]);
    }

    void loop(std::string id, double offset)
    {
        if (ma_sound_at_end(&sounds[id]))
        {
            ma_sound_seek_to_second(&sounds[id], offset);
            ma_sound_start(&sounds[id]);
        }
    }
};

#endif