// MyApp.cpp
#include "gui.h"
#include "console.h"

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "wxWidgets Window", wxDefaultPosition, wxSize(400, 300)) {
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxButton* button = new wxButton(panel, wxID_ANY, "Click Me", wxPoint(150, 100), wxSize(100, 50));
}