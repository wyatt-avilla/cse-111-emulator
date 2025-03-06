#include "vr.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <limits>

namespace RenderColors {
    constexpr int PROGRESS_BAR_BG_R = 50;
    constexpr int PROGRESS_BAR_BG_G = 50;
    constexpr int PROGRESS_BAR_BG_B = 50;
    constexpr int PROGRESS_BAR_BG_ALPHA = 255;

    constexpr int PROGRESS_INDICATOR_R = 255;
    constexpr int PROGRESS_INDICATOR_G = 215;
    constexpr int PROGRESS_INDICATOR_B = 0;
    constexpr int PROGRESS_INDICATOR_ALPHA = 255;
}

VideoRecorder::VideoRecorder(int width, int height)
    : width(width), 
      height(height), 
      recording(false), 
      playing(false),
      paused(false), 
      current_frame(0), 
      playback_delay_ms(DEFAULT_PLAYBACK_DELAY_MS),
      last_frame_time(0), 
      window(nullptr), 
      renderer(nullptr), 
      texture(nullptr),
      dragging_progress(false) {
    display_buffer.resize(static_cast<size_t>(
        static_cast<long long>(width) * static_cast<long long>(height)
    ));
}

VideoRecorder::~VideoRecorder() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
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
    std::cout << "Recording stopped. Captured " << frames.size() << " frames." << std::endl;
}

void VideoRecorder::addFrame(const uint8_t* pixels) {
    if (!recording)
        return;

    std::vector<uint8_t> frame(static_cast<size_t>(
        static_cast<long long>(width) * static_cast<long long>(height)
    ));
    
    std::copy(pixels, 
              pixels + static_cast<ptrdiff_t>(width * height), 
              frame.begin());
    frames.push_back(frame);
}

void VideoRecorder::handleMouseButtonDown(const SDL_Event& event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        if (event.button.x >= progress_bar.x && 
            event.button.x <= progress_bar.x + progress_bar.w &&
            event.button.y >= progress_bar.y && 
            event.button.y <= progress_bar.y + progress_bar.h) {
            
            dragging_progress = true;
            updateFrameFromMousePosition(
                static_cast<float>(event.button.x - progress_bar.x) / 
                static_cast<float>(progress_bar.w)
            );
        }
    }
}

void VideoRecorder::handleMouseMotion(const SDL_Event& event) {
    if (dragging_progress) {
        updateFrameFromMousePosition(
            static_cast<float>(event.motion.x - progress_bar.x) / 
            static_cast<float>(progress_bar.w)
        );
    }
}

void VideoRecorder::updateFrameFromMousePosition(float normalized_pos) {
    normalized_pos = std::clamp(normalized_pos, 0.0F, 1.0F);
    
    size_t frame_index = 0;
    if (frames.size() > 1) {
        frame_index = static_cast<size_t>(
            normalized_pos * static_cast<float>(frames.size() - 1)
        );
    }
    
    setFrameIndex(frame_index);
    
    if (!playing && frame_index < frames.size() - 1) {
        play();
    }
}

void VideoRecorder::handleKeyDown(const SDL_Event& event) {
    switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            playing ? pause() : play();
            break;

        case SDLK_RIGHT:
            nextFrame();
            break;

        case SDLK_LEFT:
            previousFrame();
            break;

        case SDLK_ESCAPE:
            return;

        case SDLK_r:
            recording ? stopRecording() : startRecording();
            break;

        default:
            break;
    }
}

bool VideoRecorder::initPlaybackWindow() {
    if (frames.empty()) {
        std::cerr << "No frames to play back" << std::endl;
        return false;
    }

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL could not initialize: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    window = SDL_CreateWindow(
        "Recording Playback",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * SCALE_FACTOR,
        height * SCALE_FACTOR,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (texture == nullptr) {
        std::cerr << "Texture could not be created: " << SDL_GetError() << std::endl;
        return false;
    }

    progress_bar.x = PROGRESS_BAR_MARGIN;
    progress_bar.y = height * SCALE_FACTOR - PROGRESS_BAR_BOTTOM_MARGIN;
    progress_bar.w = width * SCALE_FACTOR - (PROGRESS_BAR_MARGIN * 2);
    progress_bar.h = PROGRESS_BAR_HEIGHT;
    
    progress_indicator.x = progress_bar.x;
    progress_indicator.y = progress_bar.y;
    progress_indicator.w = PROGRESS_INDICATOR_WIDTH;
    progress_indicator.h = progress_bar.h;
    
    dragging_progress = false;

    current_frame = 0;
    convertFrameToRGBA(current_frame);

    return true;
}

void VideoRecorder::closePlaybackWindow() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window != nullptr) {
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

    const uint32_t current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= playback_delay_ms) {
        if (current_frame >= frames.size() - 1) {
            playing = false;
            return;
        }
        
        nextFrame();
        last_frame_time = current_time;
    }
}

bool VideoRecorder::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT:
                return false;

            case SDL_MOUSEBUTTONDOWN:
                handleMouseButtonDown(event);
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging_progress = false;
                }
                break;
                
            case SDL_MOUSEMOTION:
                handleMouseMotion(event);
                break;

            case SDL_KEYDOWN:
                handleKeyDown(event);
                break;

            default:
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
        const uint8_t gray = frame[i];
        display_buffer[i] = (static_cast<uint32_t>(COLOR_ALPHA_FULL) << ALPHA_SHIFT) | 
                            (static_cast<uint32_t>(gray) << RED_SHIFT) | 
                            (static_cast<uint32_t>(gray) << GREEN_SHIFT) | 
                            static_cast<uint32_t>(gray);
    }
}

void VideoRecorder::renderCurrentFrame() {
    SDL_UpdateTexture(
        texture,
        nullptr,
        display_buffer.data(),
        static_cast<int>(width * sizeof(uint32_t))
    );
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    
    if (!frames.empty() && frames.size() > 1) {
        const float progress = std::clamp(
            static_cast<float>(current_frame) / static_cast<float>(frames.size() - 1),
            0.0F, 1.0F
        );
        progress_indicator.x = progress_bar.x + 
            static_cast<int>(progress * static_cast<float>(progress_bar.w - progress_indicator.w));
    }
    
    SDL_SetRenderDrawColor(
        renderer, 
        RenderColors::PROGRESS_BAR_BG_R, 
        RenderColors::PROGRESS_BAR_BG_G, 
        RenderColors::PROGRESS_BAR_BG_B, 
        RenderColors::PROGRESS_BAR_BG_ALPHA
    );
    SDL_RenderFillRect(renderer, &progress_bar);
    
    SDL_SetRenderDrawColor(
        renderer, 
        RenderColors::PROGRESS_INDICATOR_R, 
        RenderColors::PROGRESS_INDICATOR_G, 
        RenderColors::PROGRESS_INDICATOR_B, 
        RenderColors::PROGRESS_INDICATOR_ALPHA
    );
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
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    const auto frame_count = static_cast<uint32_t>(frames.size());
    file.write(reinterpret_cast<const char*>(&width), sizeof(width));
    file.write(reinterpret_cast<const char*>(&height), sizeof(height));
    file.write(reinterpret_cast<const char*>(&frame_count), sizeof(frame_count));

    for (const auto& frame : frames) {
        file.write(reinterpret_cast<const char*>(frame.data()), 
                   static_cast<std::streamsize>(frame.size()));
    }

    std::cout << "Saved " << frame_count << " frames to " << filename << std::endl;
    return true;
}

bool VideoRecorder::loadRecording(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }

    int file_width = 0;
    int file_height = 0;
    uint32_t frame_count = 0;

    file.read(reinterpret_cast<char*>(&file_width), sizeof(file_width));
    file.read(reinterpret_cast<char*>(&file_height), sizeof(file_height));
    file.read(reinterpret_cast<char*>(&frame_count), sizeof(frame_count));

    if (file_width != width || file_height != height) {
        std::cerr << "Recording dimensions (" << file_width << "x" << file_height 
                  << ") don't match current dimensions (" 
                  << width << "x" << height << ")" << std::endl;
        return false;
    }

    frames.clear();

    for (uint32_t i = 0; i < frame_count; i++) {
        std::vector<uint8_t> frame(static_cast<size_t>(width * height));
        file.read(reinterpret_cast<char*>(frame.data()), 
                  static_cast<std::streamsize>(frame.size()));
        frames.push_back(frame);
    }

    current_frame = 0;
    std::cout << "Loaded " << frames.size() << " frames from " << filename << std::endl;
    return true;
}