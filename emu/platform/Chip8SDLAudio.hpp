#pragma once
#include <SDL3/SDL_audio.h>
#include <cmath>

class Chip8Audio {
  public:
    Chip8Audio();
    ~Chip8Audio();

    void play();
    void stop();
    bool isPlaying() const { return playing; }
    void update();

  private:
    SDL_AudioStream *stream = nullptr;
    bool playing = false;

    static constexpr int SAMPLE_RATE = 44100;
    static constexpr int FREQUENCY = 440;
    static constexpr float AMPLITUDE = 0.1f;
    static constexpr int BUFFER_SIZE = 4096;
    static constexpr int MIN_QUEUED_BYTES = 2048;

    float phase = 0.0f;
    void generateSquareWave(float *buffer, int samples);
};
