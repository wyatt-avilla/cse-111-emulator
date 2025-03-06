#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class VideoRecorder {
public:
    VideoRecorder(int width, int height);
    ~VideoRecorder();

    // Start recording frames
    void startRecording();

    // Stop recording frames
    void stopRecording();

    // Add a frame to the recording
    void addFrame(const uint8_t* pixels);

    // Playback controls
    void play();
    void pause();
    void stop();
    void nextFrame();
    void previousFrame();
    void setFrameIndex(size_t index);

    // Current state queries
    [[nodiscard]] bool isRecording() const { return recording; }
    [[nodiscard]] bool isPlaying() const { return playing; }
    [[nodiscard]] size_t getFrameCount() const { return frames.size(); }
    [[nodiscard]] size_t getCurrentFrameIndex() const { return current_frame; }

    // Initialize and display the playback window
    bool initPlaybackWindow();

    // Close the playback window and clean up resources
    void closePlaybackWindow();

    // Update the playback display
    void updateDisplay();

    // Handle SDL events for the playback window
    bool handleEvents();

    // Save and load recordings
    bool saveRecording(const std::string& filename);
    bool loadRecording(const std::string& filename);

private:
    // Constants to replace magic numbers
    static constexpr int PROGRESS_BAR_MARGIN = 20;
    static constexpr int PROGRESS_BAR_HEIGHT = 10;
    static constexpr int PROGRESS_BAR_BOTTOM_MARGIN = 30;
    static constexpr int PROGRESS_INDICATOR_WIDTH = 10;
    static constexpr int DEFAULT_PLAYBACK_DELAY_MS = 17; // ~60fps
    static constexpr int SCALE_FACTOR = 4;
    static constexpr int COLOR_ALPHA_FULL = 0xFF;
    static constexpr int PROGRESS_BAR_BG_COLOR_R = 50;
    static constexpr int PROGRESS_BAR_BG_COLOR_G = 50;
    static constexpr int PROGRESS_BAR_BG_COLOR_B = 50;
    static constexpr int PROGRESS_INDICATOR_COLOR_R = 255;
    static constexpr int PROGRESS_INDICATOR_COLOR_G = 215;
    static constexpr int PROGRESS_INDICATOR_COLOR_B = 0;

    int width;
    int height;
    bool recording;
    bool playing;
    bool paused;
    size_t current_frame;
    uint32_t playback_delay_ms;
    uint32_t last_frame_time;

    // SDL objects for playback window
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    // Progress bar variables
    SDL_Rect progress_bar;
    SDL_Rect progress_indicator;
    bool dragging_progress;

    // Store frames as grayscale data
    std::vector<std::vector<uint8_t>> frames;

    // Buffer for converting the current frame to RGBA for display
    std::vector<uint32_t> display_buffer;

    // Convert grayscale to RGBA for display
    void convertFrameToRGBA(size_t frame_index);

    // Render the current frame
    void renderCurrentFrame();
};