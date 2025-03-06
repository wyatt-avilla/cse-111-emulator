// src/vr.cpp
#include "vr.h"

#include <algorithm> // for std::min, std::max
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

VideoRecorder::VideoRecorder(int width, int height)
    : width(width), height(height), recording(false), playing(false),
      paused(false), current_frame(0), playback_delay_ms(17), // ~60fps
      last_frame_time(0), window(nullptr), renderer(nullptr), texture(nullptr),
      dragging_progress(false) {

    display_buffer.resize(width * height);
}

VideoRecorder::~VideoRecorder() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

void VideoRecorder::startRecording() {
    frames.clear();
    current_frame = 0;
    recording = true;
    playing = false;
    std::cout << "Recording started" << std::endl;
}

void VideoRecorder::stopRecording() {
    recording = false;
    std::cout << "Recording stopped. Captured " << frames.size() << " frames."
              << std::endl;
}

void VideoRecorder::addFrame(const uint8_t* pixels) {
    if (!recording)
        return;

    // Create a new frame and copy the pixel data
    std::vector<uint8_t> frame(width * height);
    std::copy(pixels, pixels + (width * height), frame.begin());
    frames.push_back(frame);
}

bool VideoRecorder::initPlaybackWindow() {
    if (frames.empty()) {
        std::cerr << "No frames to play back" << std::endl;
        return false;
    }

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize: " << SDL_GetError()
                      << std::endl;
            return false;
        }
    }

    window = SDL_CreateWindow(
        "Recording Playback",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * 4, // Scale up for better visibility
        height * 4,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created: " << SDL_GetError()
                  << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created: " << SDL_GetError()
                  << std::endl;
        return false;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!texture) {
        std::cerr << "Texture could not be created: " << SDL_GetError()
                  << std::endl;
        return false;
    }

    // Initialize progress bar
    progress_bar.x = 20;
    progress_bar.y = height * 4 - 30; // 30 pixels from bottom
    progress_bar.w = width * 4 - 40;  // Full width minus margins
    progress_bar.h = 10;
    
    progress_indicator.x = progress_bar.x;
    progress_indicator.y = progress_bar.y;
    progress_indicator.w = 10;
    progress_indicator.h = progress_bar.h;
    
    dragging_progress = false;

    current_frame = 0;
    convertFrameToRGBA(current_frame);

    return true;
}

void VideoRecorder::closePlaybackWindow() {
    // Clean up SDL resources
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    playing = false;
    paused = false;
    std::cout << "Playback window closed" << std::endl;
}

void VideoRecorder::play() {
    if (frames.empty())
        return;

    playing = true;
    paused = false;
    last_frame_time = SDL_GetTicks();
    std::cout << "Playback started" << std::endl;
}

void VideoRecorder::pause() {
    paused = !paused;
    std::cout << "Playback " << (paused ? "paused" : "resumed") << std::endl;
}

void VideoRecorder::stop() {
    playing = false;
    current_frame = 0;
    std::cout << "Playback stopped" << std::endl;
}

void VideoRecorder::nextFrame() {
    if (frames.empty())
        return;

    if (current_frame < frames.size() - 1) {
        current_frame++;
        convertFrameToRGBA(current_frame);
        renderCurrentFrame();
    } else {
        // Reached the end, stop playing
        playing = false;
    }
}

void VideoRecorder::previousFrame() {
    if (frames.empty())
        return;

    if (current_frame == 0) {
        current_frame = frames.size() - 1;
    } else {
        current_frame--;
    }

    convertFrameToRGBA(current_frame);
    renderCurrentFrame();
}

void VideoRecorder::setFrameIndex(size_t index) {
    if (frames.empty() || index >= frames.size())
        return;

    current_frame = index;
    convertFrameToRGBA(current_frame);
    renderCurrentFrame();
}

void VideoRecorder::updateDisplay() {
    if (!playing || paused || frames.empty())
        return;

    uint32_t current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= playback_delay_ms) {
        // Check if we're at the last frame
        if (current_frame >= frames.size() - 1) {
            // Stop playback instead of looping
            playing = false;
            return;
        }
        
        nextFrame();
        last_frame_time = current_time;
    }
}

bool VideoRecorder::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                // Check if click is within progress bar
                if (event.button.x >= progress_bar.x && 
                    event.button.x <= progress_bar.x + progress_bar.w &&
                    event.button.y >= progress_bar.y && 
                    event.button.y <= progress_bar.y + progress_bar.h) {
                    
                    dragging_progress = true;
                    
                    // Calculate and set frame position based on click position
                    float clickPosition = static_cast<float>(event.button.x - progress_bar.x) / progress_bar.w;
                    clickPosition = std::max(0.0f, std::min(1.0f, clickPosition));
                    
                    // Make sure frames.size() > 1 to avoid division by zero
                    size_t frameIndex = 0;
                    if (frames.size() > 1) {
                        frameIndex = static_cast<size_t>(clickPosition * (frames.size() - 1));
                    }
                    
                    setFrameIndex(frameIndex);
                    
                    // If we're at the end (playback stopped) and user moves back
                    // to an earlier position, restart playback
                    if (!playing && frameIndex < frames.size() - 1) {
                        play();
                    }
                }
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                dragging_progress = false;
            }
            break;
            
        case SDL_MOUSEMOTION:
            if (dragging_progress) {
                // If dragging, update position based on mouse movement
                float movePosition = static_cast<float>(event.motion.x - progress_bar.x) / progress_bar.w;
                movePosition = std::max(0.0f, std::min(1.0f, movePosition));
                
                // Make sure frames.size() > 1 to avoid division by zero
                size_t frameIndex = 0;
                if (frames.size() > 1) {
                    frameIndex = static_cast<size_t>(movePosition * (frames.size() - 1));
                }
                
                setFrameIndex(frameIndex);
                
                // If we're at the end (playback stopped) and user drags back
                // to an earlier position, restart playback
                if (!playing && frameIndex < frames.size() - 1) {
                    play();
                }
            }
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                if (playing) {
                    pause();
                } else {
                    play();
                }
                break;

            case SDLK_RIGHT:
                nextFrame();
                break;

            case SDLK_LEFT:
                previousFrame();
                break;

            case SDLK_ESCAPE:
                return false;

            case SDLK_r:
                if (!recording) {
                    startRecording();
                } else {
                    stopRecording();
                }
                break;
            }
            break;
        }
    }
    return true;
}

void VideoRecorder::convertFrameToRGBA(size_t frame_index) {
    if (frame_index >= frames.size())
        return;

    const auto& frame = frames[frame_index];
    for (size_t i = 0; i < frame.size(); i++) {
        uint8_t gray = frame[i];
        display_buffer[i] = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
    }
}

void VideoRecorder::renderCurrentFrame() {
    SDL_UpdateTexture(
        texture,
        nullptr,
        display_buffer.data(),
        width * sizeof(uint32_t)
    );
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    
    // Update progress indicator position based on current frame
    if (!frames.empty() && frames.size() > 1) {
        float progress = static_cast<float>(current_frame) / (frames.size() - 1);
        progress_indicator.x = progress_bar.x + static_cast<int>(progress * (progress_bar.w - progress_indicator.w));
    }
    
    // Draw progress bar background (dark gray)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &progress_bar);
    
    // Draw progress indicator (yellow)
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_RenderFillRect(renderer, &progress_indicator);
    
    SDL_RenderPresent(renderer);
}

bool VideoRecorder::saveRecording(const std::string& filename) {
    if (frames.empty()) {
        std::cerr << "No frames to save" << std::endl;
        return false;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename
                  << std::endl;
        return false;
    }

    // Write header: width, height, frame count
    uint32_t frame_count = static_cast<uint32_t>(frames.size());
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(
        reinterpret_cast<const char*>(&frame_count),
        sizeof(frame_count)
    );

    // Write all frames
    for (const auto& frame : frames) {
        file.write(reinterpret_cast<const char*>(frame.data()), frame.size());
    }

    std::cout << "Saved " << frame_count << " frames to " << filename
              << std::endl;
    return true;
}

bool VideoRecorder::loadRecording(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filename
                  << std::endl;
        return false;
    }

    // Read header
    int file_width, file_height;
    uint32_t frame_count;

    file.read(reinterpret_cast<char*>(&file_width), sizeof(file_width));
    file.read(reinterpret_cast<char*>(&file_height), sizeof(file_height));
    file.read(reinterpret_cast<char*>(&frame_count), sizeof(frame_count));

    if (file_width != width || file_height != height) {
        std::cerr << "Recording dimensions (" << file_width << "x"
                  << file_height << ") don't match current dimensions ("
                  << width << "x" << height << ")" << std::endl;
        return false;
    }

    // Clear existing frames and load new ones
    frames.clear();

    for (uint32_t i = 0; i < frame_count; i++) {
        std::vector<uint8_t> frame(width * height);
        file.read(reinterpret_cast<char*>(frame.data()), frame.size());
        frames.push_back(frame);
    }

    current_frame = 0;
    std::cout << "Loaded " << frames.size() << " frames from " << filename
              << std::endl;
    return true;
}