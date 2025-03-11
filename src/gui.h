#pragma once

#include "gpu.h"
#include "vr.h"

#include <SDL2/SDL.h>
#include <memory>
#include <wx/wx.h>

class MyFrame : public wxFrame {
  public:
    MyFrame();
    GPU* gpu;
    SDL_Texture* texture;
    SDL_Renderer* renderer;

  private:
    wxButton* select_button;
    wxButton* execute_button;
    wxButton* playback_button;

    wxStaticBitmap* image_bitmap;
    wxString file_path;

    std::unique_ptr<VideoRecorder> video_recorder;
    bool has_recording = false;


    void onFileSelect(wxCommandEvent& event);
    void onExecute(wxCommandEvent& event);
    void onPlayback(wxCommandEvent& event);
    void onResize(wxSizeEvent& event);
    // void onInitUI();  // Initialize UI components including the color button
};

class MyApp : public wxApp {
  public:
    bool OnInit() override;
};
