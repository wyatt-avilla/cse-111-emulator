// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3

#include "gui.h"
#include "console.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/statbox.h>

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Banana Emulator", wxDefaultPosition, wxSize(500, 400)) {
    
    wxPanel* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(40, 40, 40)); // Dark grayish black background
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "Welcome to Banana Emulator", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    title->SetForegroundColour(wxColour(255, 215, 0)); // Yellow title text
    title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
    
    selectButton = new wxButton(panel, wxID_ANY, "Select File", wxDefaultPosition, wxSize(150, 40));
    executeButton = new wxButton(panel, wxID_ANY, "Execute", wxDefaultPosition, wxSize(150, 40));
    executeButton->Disable(); // Initially disabled
    
    // Button Styling
    wxColour buttonColor(30, 30, 30);  // Darker black
    wxColour outlineColor(255, 215, 0); // Yellow outline
    
    selectButton->SetBackgroundColour(buttonColor);
    selectButton->SetForegroundColour(outlineColor);
    executeButton->SetBackgroundColour(buttonColor);
    executeButton->SetForegroundColour(outlineColor);
    
    buttonSizer->Add(selectButton, 0, wxALIGN_CENTER | wxALL, 10);
    buttonSizer->Add(executeButton, 0, wxALIGN_CENTER | wxALL, 10);
    
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxTOP, 20);
    mainSizer->AddStretchSpacer();
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);
    mainSizer->AddStretchSpacer();
    
    panel->SetSizer(mainSizer);
    
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
