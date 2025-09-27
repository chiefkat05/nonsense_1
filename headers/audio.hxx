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
    std::map<std::string, ma_sound *> sounds;
    std::vector<std::string> usedStr;

    ~audio_player_struct()
    {
        for (int i = 0; i < usedStr.size(); ++i)
        {
            delete sounds[usedStr[i]];
        }
    }

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
        for (int i = 0; i < usedStr.size(); ++i)
        {
            if (usedStr[i] == id)
                return;
        }
        sounds[id] = new ma_sound();
        result = ma_sound_init_from_file(&engine, path.c_str(), 0, NULL, NULL, sounds[id]);
        if (result != MA_SUCCESS)
        {
            std::cout << "Error: audio at path " << path << " failed to load." << std::endl;
            return;
        }
        usedStr.push_back(id);
    }
    void play_audio(std::string id)
    {
        if (ma_sound_is_playing(sounds[id]))
            return;

        ma_sound_start(sounds[id]);
    }
    void force_play_audio(std::string id)
    {
        ma_sound_seek_to_pcm_frame(sounds[id], 0);
        ma_sound_start(sounds[id]);
    }
    void stop_audio(std::string id)
    {
        if (!ma_sound_is_playing(sounds[id]))
            return;

        ma_sound_stop(sounds[id]);
    }
    void set_fade_in(std::string id, double time)
    {
        if (!ma_sound_is_playing(sounds[id]))
            return;

        ma_sound_set_fade_in_milliseconds(sounds[id], 0.0, 1.0, (ma_uint64)(time * 10000));
    }
    void set_fade_out(std::string id, double time)
    {
        if (!ma_sound_is_playing(sounds[id]))
            return;

        ma_sound_set_fade_in_milliseconds(sounds[id], 1.0, 0.0, (ma_uint64)(time * 10000));
    }

    void loop(std::string id, double offset)
    {
        if (ma_sound_at_end(sounds[id]))
        {
            ma_sound_seek_to_second(sounds[id], offset);
            ma_sound_start(sounds[id]);
        }
    }
};

#endif