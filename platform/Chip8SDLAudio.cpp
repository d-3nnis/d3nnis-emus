#include "Chip8SDLAudio.hpp"
#include "SDL3/SDL_init.h"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_log.h>
#include <cmath>
#include <vector>

Chip8Audio::Chip8Audio() {
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec,
                                       nullptr, nullptr);

    if (!stream) {
        SDL_Log("Failed to open audio: %s", SDL_GetError());
        return;
    }

    SDL_PauseAudioStreamDevice(stream);
}

Chip8Audio::~Chip8Audio() {
    if (stream) {
        SDL_DestroyAudioStream(stream);
        stream = nullptr;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Chip8Audio::generateSquareWave(float *buffer, int samples) {
    const float phaseIncrement = (2.0f * M_PI * FREQUENCY) / SAMPLE_RATE;

    for (int i = 0; i < samples; i++) {
        buffer[i] = (std::sin(phase) >= 0.0f ? AMPLITUDE : -AMPLITUDE);
        phase += phaseIncrement;

        if (phase >= 2.0f * M_PI) {
            phase -= 2.0f * M_PI;
        }
    }
}

void Chip8Audio::play() {
    if (!stream || playing) {
        return;
    }

    std::vector<float> buffer(BUFFER_SIZE);
    generateSquareWave(buffer.data(), BUFFER_SIZE);

    SDL_PutAudioStreamData(stream, buffer.data(), BUFFER_SIZE * sizeof(float));
    SDL_ResumeAudioStreamDevice(stream);
    playing = true;
}

void Chip8Audio::stop() {
    if (!stream || !playing) {
        return;
    }

    SDL_PauseAudioStreamDevice(stream);
    SDL_ClearAudioStream(stream);
    phase = 0.0f;
    playing = false;
}

void Chip8Audio::update() {
    if (!stream || !playing) {
        return;
    }

    int queued = SDL_GetAudioStreamQueued(stream);
    if (queued < MIN_QUEUED_BYTES) {
        std::vector<float> buffer(BUFFER_SIZE);
        generateSquareWave(buffer.data(), BUFFER_SIZE);
        SDL_PutAudioStreamData(stream, buffer.data(),
                               BUFFER_SIZE * sizeof(float));
    }
}
