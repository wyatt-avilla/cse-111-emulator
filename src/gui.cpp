// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3

#include "gui.h"
#include "console.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include <wx/image.h>
#include <wx/sizer.h>
#include <thread> // Added this

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

    // Load Image
    wxString imagePath = "/home/mike/Documents/college/cse-111-emulator/src/banna.png"; 
    wxInitAllImageHandlers(); // Ensure wxWidgets supports images
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

    wxImage image("/home/mike/Documents/college/cse-111-emulator/src/banna.png", wxBITMAP_TYPE_PNG);
    if (image.IsOk()) {
        image.Rescale(newWidth, newHeight);
        imageBitmap->SetBitmap(wxBitmap(image));
        Layout(); // Refresh layout
    }

    event.Skip(); // Allow normal event processing
}

// Handle file selection
void MyFrame::OnFileSelect(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Open File", "", "", "All files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }
    
    filePath = openFileDialog.GetPath();
    wxMessageBox("You selected: " + filePath, "File Selected", wxOK | wxICON_INFORMATION);
    executeButton->Enable(); // Enable execute button after selecting a file
}

// Handle execution
void MyFrame::OnExecute(wxCommandEvent& event) {
    if (!filePath.IsEmpty()) {
        std::thread([this]() {
            Console banana;
            try {
                banana.run(std::string(filePath.ToStdString()));
            } catch (const std::exception& e) {
                wxMessageBox("Couldn't run file:\n" + filePath + "\nError: " + e.what(), "Execution Error", wxOK | wxICON_ERROR);
            }
        }).detach();
    }
}