#include <wx/wx.h>

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame();
private:
    void OnButtonClick(wxCommandEvent& event);
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, "Simple wxWidgets Window", wxDefaultPosition, wxSize(400, 300)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxButton* button = new wxButton(panel, wxID_ANY, "Click Me", wxPoint(150, 100), wxSize(100, 40));
    button->Bind(wxEVT_BUTTON, &MyFrame::OnButtonClick, this);
}

void MyFrame::OnButtonClick(wxCommandEvent& event) {
    wxMessageBox("Hello! You clicked the button!", "Info", wxOK | wxICON_INFORMATION);
}