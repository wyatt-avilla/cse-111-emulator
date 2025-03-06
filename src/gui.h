#pragma once

#include "vr.h"

#include <memory>
#include <wx/wx.h>


class MyFrame : public wxFrame {
  public:
    MyFrame();

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
};

class MyApp : public wxApp {
  public:
    bool OnInit() override;
};
