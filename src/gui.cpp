// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3

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
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    
    selectButton = new wxButton(panel, wxID_ANY, "Select File");
    executeButton = new wxButton(panel, wxID_ANY, "Execute");
    executeButton->Disable(); // Initially disabled
    
    sizer->Add(selectButton, 0, wxALIGN_CENTER | wxALL, 10);
    sizer->Add(executeButton, 0, wxALIGN_CENTER | wxALL, 10);
    
    panel->SetSizer(sizer);
    
    selectButton->Bind(wxEVT_BUTTON, &MyFrame::OnFileSelect, this);
    executeButton->Bind(wxEVT_BUTTON, &MyFrame::OnExecute, this);
}

void MyFrame::OnFileSelect(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Open File", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }
    
    filePath = openFileDialog.GetPath();
    wxMessageBox("You selected: " + filePath, "File Selected", wxOK | wxICON_INFORMATION);
    executeButton->Enable(); // Enable execute button after selecting a file
}

void MyFrame::OnExecute(wxCommandEvent& event) {
    if (!filePath.IsEmpty()) {
        Console banana;

        try {
                banana.run(std::string(filePath.ToStdString()));
            } catch (const std::exception& e) {
                std::cerr << "Couldn't run " << "\"" << std::string(filePath.ToStdString())
                            << "\":" << std::endl
                            << "    " << e.what() << std::endl;
            }
        
            exit(EXIT_SUCCESS);
    }
}
