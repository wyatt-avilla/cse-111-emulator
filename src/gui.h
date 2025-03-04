#pragma once

#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
    MyFrame();
    
private:
    wxButton* select_button;
    wxButton* execute_button;
    wxStaticBitmap* image_bitmap; // Declare imageBitmap here
    wxString file_path;

    void onFileSelect(wxCommandEvent& event);
    void onExecute(wxCommandEvent& event);
    void onResize(wxSizeEvent& event); // Declare OnResize here
};

class MyApp : public wxApp {
public:
    bool OnInit() override;
};

