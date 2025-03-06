#pragma once

#include <wx/wx.h>
#include <memory>

// Forward declaration
class VideoRecorder;

class MyFrame : public wxFrame {
  public:
    MyFrame();

  private:
    wxButton* select_button;
    wxButton* execute_button;
    wxButton* playback_button; // New button for video playback
    wxStaticBitmap* image_bitmap;
    wxString file_path;
    
    // Video recorder instance
    std::unique_ptr<VideoRecorder> video_recorder;
    bool has_recording = false;

    void onFileSelect(wxCommandEvent& event);
    void onExecute(wxCommandEvent& event);
    void onPlayback(wxCommandEvent& event); // New event handler
    void onResize(wxSizeEvent& event);
};

class MyApp : public wxApp {
  public:
    bool OnInit() override;
};