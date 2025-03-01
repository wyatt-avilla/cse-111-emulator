#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
    MyFrame();
    
private:
    wxButton* selectButton;
    wxButton* executeButton;
    wxStaticBitmap* imageBitmap; // Declare imageBitmap here
    wxString filePath;

    void OnFileSelect(wxCommandEvent& event);
    void OnExecute(wxCommandEvent& event);
    void OnResize(wxSizeEvent& event); // Declare OnResize here
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

#endif // GUI_H

