#pragma once

#include "gpu.h"
#include "vr.h"

#include <SDL2/SDL.h>
#include <memory>
#include <wx/wx.h>

class Console;

class MyFrame : public wxFrame {
  public:
    MyFrame(Console* console);
    GPU* gpu;
    SDL_Texture* texture;
    SDL_Renderer* renderer;

  private:
    Console* console;
    wxButton* select_button;
    wxButton* execute_button;
    wxButton* disassemble_button;
    wxButton* playback_button;

    wxStaticBitmap* image_bitmap;
    wxString file_path;

    bool has_recording = false;


    void onFileSelect(wxCommandEvent& event);
    void onExecute(wxCommandEvent& event);
    void onDisassemble(wxCommandEvent& event);
    void onPlayback(wxCommandEvent& event);
    void onResize(wxSizeEvent& event);
};

class MyApp : public wxApp {
  public:
    bool OnInit() override;
    Console* console;
};
