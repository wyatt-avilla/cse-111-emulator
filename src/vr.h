#pragma once

#include "bit_definitions.h"

#include <SDL2/SDL.h>
#include <cstdint>
#include <string>
#include <vector>

class VideoRecorder {
  public:
    static constexpr int32_t RECORDING_WIDTH = 128;
    static constexpr int32_t RECORDING_HEIGHT = 128;
    static constexpr int32_t PROGRESS_BAR_MARGIN = 20;
    static constexpr int32_t PROGRESS_BAR_HEIGHT = 10;
    static constexpr int32_t PROGRESS_BAR_BOTTOM_MARGIN = 30;
    static constexpr int32_t PROGRESS_INDICATOR_WIDTH = 10;
    static constexpr int32_t DEFAULT_PLAYBACK_DELAY_MS = 17; // ~60fps
    static constexpr int32_t SCALE_FACTOR = 4;

    // Color and bit shift constants
    static constexpr uint32_t COLOR_ALPHA_FULL = 0xFF;
    static constexpr int32_t ALPHA_SHIFT = 24;
    static constexpr int32_t RED_SHIFT = 16;
    static constexpr int32_t GREEN_SHIFT = 8;
    static constexpr int32_t BLUE_SHIFT = 0;

    VideoRecorder(int32_t width, int32_t height);
    ~VideoRecorder();

    void startRecording();
    void stopRecording();
    void addFrame(const uint8_t* pixels);

    void play();
    void pause();
    void stop();
    void nextFrame();
    void previousFrame();
    void setFrameIndex(size_t index);

    [[nodiscard]] bool isRecording() const { return recording; }
    [[nodiscard]] bool isPlaying() const { return playing; }
    [[nodiscard]] size_t getFrameCount() const { return frames.size(); }
    [[nodiscard]] size_t getCurrentFrameIndex() const { return current_frame; }

    bool initPlaybackWindow();
    void closePlaybackWindow();
    void updateDisplay();
    bool handleEvents();

    bool saveRecording(const std::string& filename);
    bool loadRecording(const std::string& filename);
    void setColorTint(uint8_t red, uint8_t green, uint8_t blue) {
        color_tint.r = red;
        color_tint.g = green;
        color_tint.b = blue;
    }


  private:
    int32_t width;
    int32_t height;
    bool recording;
    bool playing;
    bool paused;
    size_t current_frame;
    uint32_t playback_delay_ms;
    uint32_t last_frame_time;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_Rect progress_bar;
    SDL_Rect progress_indicator;
    bool dragging_progress;

    std::vector<std::vector<uint8_t>> frames;
    std::vector<uint32_t> display_buffer;

    bool initializeSDL();
    bool createSDLResources();
    void initializeProgressBar();
    void cleanupSDLResources();

    void handleMouseButtonDown(const SDL_Event& event);
    void handleMouseMotion(const SDL_Event& event);
    void updateFrameFromMousePosition(float normalized_pos);
    void handleKeyDown(const SDL_Event& event);

    void convertFrameToRGBA(size_t frame_index);
    void renderCurrentFrame();
    struct {
        uint8_t r = BYTE_MASK;
        uint8_t g = BYTE_MASK;
        uint8_t b = BYTE_MASK;
    } color_tint;
};
