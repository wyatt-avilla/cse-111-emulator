// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3
// and then further modified with this thread of chat
// https://chatgpt.com/share/67c667e9-5dc0-8013-a471-56d7cadf7081

#ifndef HEADLESS_BUILD

#include "gui.h"
#include "console.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/image.h>
#include <wx/sizer.h>
#include <wx/filename.h>
#include <thread> 
#include <iostream>
#include <wx/stdpaths.h>


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
    
    // Title
    wxStaticText* title = new wxStaticText(panel, wxID_ANY, "Welcome to Banana Emulator", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    title->SetForegroundColour(wxColour(255, 215, 0)); // Yellow title text
    title->SetFont(wxFont(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // Get dynamic image path
    wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exePath);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString imagePath = file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png";
    
    // Load Image
    wxInitAllImageHandlers(); 
    wxImage image(imagePath, wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxMessageBox("Failed to load image: " + imagePath, "Error", wxOK | wxICON_ERROR);
    }

    image.Rescale(300, 150); // Initial size
    imageBitmap = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(image));

    // Buttons
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

    // Layout
    mainSizer->Add(title, 0, wxALIGN_CENTER | wxTOP, 20);
    mainSizer->Add(imageBitmap, 0, wxALIGN_CENTER | wxALL, 10);
    mainSizer->AddStretchSpacer();
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);
    mainSizer->AddStretchSpacer();

    panel->SetSizer(mainSizer);

    // Bind Events
    selectButton->Bind(wxEVT_BUTTON, &MyFrame::OnFileSelect, this);
    executeButton->Bind(wxEVT_BUTTON, &MyFrame::OnExecute, this);
    Bind(wxEVT_SIZE, &MyFrame::OnResize, this); // Resize event
}

// Handle window resizing
void MyFrame::OnResize(wxSizeEvent& event) {
    wxSize newSize = GetClientSize();
    int newWidth = newSize.GetWidth() * 0.6; // Scale to 60% of window width
    int newHeight = newWidth * 0.5; // Maintain aspect ratio

    wxFileName exePath(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exePath);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString imagePath = file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png"; 

    wxImage image(imagePath, wxBITMAP_TYPE_PNG);
    if (image.IsOk()) {
        image.Rescale(newWidth, newHeight);
        imageBitmap->SetBitmap(wxBitmap(image));
        Layout(); // Refresh layout
    }

    event.Skip(); // Allow normal event processing
}

// Handle file selection
void MyFrame::OnFileSelect(wxCommandEvent&) {
    wxFileDialog openFileDialog(this, "Open .slug File", "", "", "SLUG files (*.slug)|*.slug", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }
    
    filePath = openFileDialog.GetPath();
    
    // Ensure the file has a ".slug" extension
    if (!filePath.Lower().EndsWith(".slug")) {
        wxMessageBox("Invalid file type! Please select a .slug file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxMessageBox("You selected: " + filePath, "File Selected", wxOK | wxICON_INFORMATION);
    executeButton->Enable(); // Enable execute button after selecting a valid file
}

// Handle execution
void MyFrame::OnExecute(wxCommandEvent&) {
    if (!filePath.IsEmpty()) {
        std::thread([this]() {
            Console banana(true);
            try {
                banana.run(std::string(filePath.ToStdString()));
            } catch (const std::exception& e) {
                wxMessageBox("Couldn't run file:\n" + filePath + "\nError: " + e.what(), "Execution Error", wxOK | wxICON_ERROR);
            }
        }).detach();
    }
}
#endif
