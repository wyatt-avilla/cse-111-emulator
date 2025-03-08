#pragma once

#include "vr.h"

#include <memory>
#include <wx/wx.h>
#include <SDL2/SDL.h>


class MyFrame : public wxFrame {
  public:
    MyFrame();

  private:
    wxButton* select_button;
    wxButton* execute_button;
    wxButton* playback_button;
    wxButton* color_button;
    wxStaticBitmap* image_bitmap;
    wxString file_path;

    std::unique_ptr<VideoRecorder> video_recorder;
    bool has_recording = false;

    // Function to handle changing background color
    void onChangeColor(wxCommandEvent& event);

    void onFileSelect(wxCommandEvent& event);
    void onExecute(wxCommandEvent& event);
    void onPlayback(wxCommandEvent& event);
    void onResize(wxSizeEvent& event);
    //void onInitUI();  // Initialize UI components including the color button
};

class MyApp : public wxApp {
  public:
    bool OnInit() override;
};
