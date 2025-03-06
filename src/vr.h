// src/vr.h
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
    bool isRecording() const { return recording; }
    bool isPlaying() const { return playing; }
    size_t getFrameCount() const { return frames.size(); }
    size_t getCurrentFrameIndex() const { return current_frame; }
    
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
    
    // Store frames as RGB8 data
    std::vector<std::vector<uint8_t>> frames;
    
    // Buffer for converting the current frame to RGBA for display
    std::vector<uint32_t> display_buffer;
    
    // Convert grayscale to RGBA for display
    void convertFrameToRGBA(size_t frame_index);
    
    // Render the current frame
    void renderCurrentFrame();
};
