#ifndef GUI_H
#define GUI_H

#include <wx/wx.h>

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    void OnFileSelect(wxCommandEvent& event); // Declare the function
};

class MyApp : public wxApp {
    public:
        virtual bool OnInit();
};

#endif // GUI_H
