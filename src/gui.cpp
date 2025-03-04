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

const int WX_FRAME_X_POSTION = 500;
const int WX_FRAME_Y_POSTION = 400;
const int GREY_COLOR = 40;
const int FOREGROUND_COLOUR_1 = 255;
const int FOREGROUND_COLOUR_2 = 215;
const int FOREGROUND_COLOUR_3 = 0;
const int FONT_SIZE = 14;
const int RESCALE_X = 300;
const int RESCALE_Y = 150;
const int SELECT_BUTTON_X = 150;
const int SELECT_BUTTON_Y = 40;
const int EXECUTE_BUTTON_X = 150;
const int EXECUTE_BUTTON_Y = 40;
const int BUTTON_SIZE = 10;
const int IMAGE_SIZE = 10;
const int TITLE_SIZE = 20;

bool MyApp::OnInit() {
    auto* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Banana Emulator", wxDefaultPosition, wxSize(WX_FRAME_X_POSTION, WX_FRAME_Y_POSTION)) {
    
    auto* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(GREY_COLOR, GREY_COLOR, GREY_COLOR)); // Dark grayish black background
    
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);
    
    // Title
    auto* title = new wxStaticText(panel, wxID_ANY, "Welcome to Banana Emulator", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE);
    title->SetForegroundColour(wxColour(FOREGROUND_COLOUR_1, FOREGROUND_COLOUR_2, FOREGROUND_COLOUR_3)); // Yellow title text
    title->SetFont(wxFont(FONT_SIZE, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // Get dynamic image path
    wxFileName const exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exe_path);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString const image_path = file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png";
    
    // Load Image
    wxInitAllImageHandlers(); 
    wxImage image(image_path, wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxMessageBox("Failed to load image: " + image_path, "Error", wxOK | wxICON_ERROR);
    }

    image.Rescale(RESCALE_X, RESCALE_Y); // Initial size
    imageBitmap = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(image));

    // Buttons
    auto* button_sizer = new wxBoxSizer(wxVERTICAL);
    selectButton = new wxButton(panel, wxID_ANY, "Select File", wxDefaultPosition, wxSize(SELECT_BUTTON_X, SELECT_BUTTON_Y));
    executeButton = new wxButton(panel, wxID_ANY, "Execute", wxDefaultPosition, wxSize(EXECUTE_BUTTON_X, EXECUTE_BUTTON_Y));
    executeButton->Disable(); // Initially disabled

    // Button Styling
    wxColour const button_color(30, 30, 30);  // Darker black
    wxColour const outline_color(255, 215, 0); // Yellow outline

    selectButton->SetBackgroundColour(button_color);
    selectButton->SetForegroundColour(outline_color);
    executeButton->SetBackgroundColour(button_color);
    executeButton->SetForegroundColour(outline_color);

    button_sizer->Add(selectButton, 0, wxALIGN_CENTER | wxALL, BUTTON_SIZE);
    button_sizer->Add(executeButton, 0, wxALIGN_CENTER | wxALL, BUTTON_SIZE);

    // Layout
    main_sizer->Add(title, 0, wxALIGN_CENTER | wxTOP, TITLE_SIZE);
    main_sizer->Add(imageBitmap, 0, wxALIGN_CENTER | wxALL, IMAGE_SIZE);
    main_sizer->AddStretchSpacer();
    main_sizer->Add(button_sizer, 0, wxALIGN_CENTER);
    main_sizer->AddStretchSpacer();

    panel->SetSizer(main_sizer);

    // Bind Events
    selectButton->Bind(wxEVT_BUTTON, &MyFrame::OnFileSelect, this);
    executeButton->Bind(wxEVT_BUTTON, &MyFrame::OnExecute, this);
    Bind(wxEVT_SIZE, &MyFrame::OnResize, this); // Resize event
}

// Handle window resizing
void MyFrame::OnResize(wxSizeEvent& event) {
    wxSize const new_size = GetClientSize();
    double const new_width = new_size.GetWidth() * 0.6; // Scale to 60% of window width
    double const new_height = new_width * 0.5; // Maintain aspect ratio

    wxFileName const exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exe_path);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString const image_path = file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png"; 

    wxImage image(image_path, wxBITMAP_TYPE_PNG);
    if (image.IsOk()) {
        image.Rescale(new_width, new_height);
        imageBitmap->SetBitmap(wxBitmap(image));
        Layout(); // Refresh layout
    }

    event.Skip(); // Allow normal event processing
}

// Handle file selection
void MyFrame::OnFileSelect(wxCommandEvent& /*unused*/) {
    wxFileDialog open_file_dialog(this, "Open .slug File", "", "", "SLUG files (*.slug)|*.slug", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (open_file_dialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }
    
    filePath = open_file_dialog.GetPath();
    
    // Ensure the file has a ".slug" extension
    if (!filePath.Lower().EndsWith(".slug")) {
        wxMessageBox("Invalid file type! Please select a .slug file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxMessageBox("You selected: " + filePath, "File Selected", wxOK | wxICON_INFORMATION);
    executeButton->Enable(); // Enable execute button after selecting a valid file
}

// Handle execution
void MyFrame::OnExecute(wxCommandEvent& /*unused*/) {
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
