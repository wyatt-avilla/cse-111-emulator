#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    void OnFileSelect(wxCommandEvent& event); // Declare the function
    void OnExecute(wxCommandEvent& event); // Declare the execute function
    
    wxButton* selectButton; // Declare selectButton
    wxButton* executeButton; // Declare executeButton
    wxString filePath; // Declare filePath to store selected file
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

#endif // GUI_H
