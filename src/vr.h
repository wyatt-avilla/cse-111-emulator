#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class VideoRecorder {
public:
    // Constants to replace magic numbers
    static constexpr int RECORDING_WIDTH = 128;
    static constexpr int RECORDING_HEIGHT = 128;
    static constexpr int PROGRESS_BAR_MARGIN = 20;
    static constexpr int PROGRESS_BAR_HEIGHT = 10;
    static constexpr int PROGRESS_BAR_BOTTOM_MARGIN = 30;
    static constexpr int PROGRESS_INDICATOR_WIDTH = 10;
    static constexpr int DEFAULT_PLAYBACK_DELAY_MS = 17; // ~60fps
    static constexpr int SCALE_FACTOR = 4;
    
    // Color and bit shift constants
    static constexpr uint32_t COLOR_ALPHA_FULL = 0xFF;
    static constexpr int ALPHA_SHIFT = 24;
    static constexpr int RED_SHIFT = 16;
    static constexpr int GREEN_SHIFT = 8;
    static constexpr int BLUE_SHIFT = 0;

    VideoRecorder(int width, int height);
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

private:
    int width;
    int height;
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

    // New method declarations to reduce complexity
    void handleMouseButtonDown(const SDL_Event& event);
    void handleMouseMotion(const SDL_Event& event);
    void updateFrameFromMousePosition(float normalized_pos);
    void handleKeyDown(const SDL_Event& event);

    void convertFrameToRGBA(size_t frame_index);
    void renderCurrentFrame();
};