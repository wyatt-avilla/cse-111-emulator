#include "gui.h"
#include "console.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "wxWidgets Window", wxDefaultPosition, wxSize(400, 300)) {
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxButton* button = new wxButton(panel, wxID_ANY, "Select File", wxPoint(150, 100), wxSize(100, 50));
    
    button->Bind(wxEVT_BUTTON, &MyFrame::OnFileSelect, this);
}

void MyFrame::OnFileSelect(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Open File", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }
    
    wxString filePath = openFileDialog.GetPath();
    wxMessageBox("You selected: " + filePath, "File Selected", wxOK | wxICON_INFORMATION);
}