// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3
// and then further modified with this thread of chat
// https://chatgpt.com/share/67c667e9-5dc0-8013-a471-56d7cadf7081

#ifndef HEADLESS_BUILD

#include "gui.h"
#include "filter.h"

#include "console.h"
#include "vr.h"
#include "gpu.h"

#include <iostream>
#include <thread>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/stdpaths.h>
#include <wx/colour.h>  // For wxColour dialog
#include <wx/colordlg.h> // For the color dialog

const int DEFAULT_VIDEO_RECORDER_WIDTH = 128;
const int DEFAULT_VIDEO_RECORDER_HEIGHT = 128;

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

MyFrame::MyFrame() // NOLINT(readability-function-size)
    : wxFrame(
          nullptr,
          wxID_ANY,
          "Banana Emulator",
          wxDefaultPosition,
          wxSize(WX_FRAME_X_POSTION, WX_FRAME_Y_POSTION)
      ) {
    gpu = new GPU();
    auto* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(GREY_COLOR, GREY_COLOR, GREY_COLOR)
    ); // Dark grayish black background

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Title
    auto* title = new wxStaticText(
        panel,
        wxID_ANY,
        "Welcome to Banana Emulator",
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_CENTRE
    );
    title->SetForegroundColour(
        wxColour(FOREGROUND_COLOUR_1, FOREGROUND_COLOUR_2, FOREGROUND_COLOUR_3)
    ); // Yellow title text
    title->SetFont(wxFont(
        FONT_SIZE,
        wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD
    ));

    // Get dynamic image path
    wxFileName const exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exe_path);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString const image_path =
        file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png";

    // Load Image
    wxInitAllImageHandlers();
    wxImage image(image_path, wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxMessageBox(
            "Failed to load image: " + image_path,
            "Error",
            wxOK | wxICON_ERROR
        );
    }

    image.Rescale(RESCALE_X, RESCALE_Y); // Initial size
    image_bitmap = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(image));

    // Buttons
    auto* button_sizer = new wxBoxSizer(wxVERTICAL);
    select_button = new wxButton(
        panel,
        wxID_ANY,
        "Select File",
        wxDefaultPosition,
        wxSize(SELECT_BUTTON_X, SELECT_BUTTON_Y)
    );
    execute_button = new wxButton(
        panel,
        wxID_ANY,
        "Execute",
        wxDefaultPosition,
        wxSize(EXECUTE_BUTTON_X, EXECUTE_BUTTON_Y)
    );
    execute_button->Disable(); // Initially disabled

    // Add playback button
    playback_button = new wxButton(
        panel,
        wxID_ANY,
        "View Recording",
        wxDefaultPosition,
        wxSize(EXECUTE_BUTTON_X, EXECUTE_BUTTON_Y)
    );
    playback_button->Disable(); // Initially disabled

    auto* color_button = new wxButton(
        panel,
        wxID_ANY,
        "Background Color",
        wxDefaultPosition,
        wxSize(SELECT_BUTTON_X, SELECT_BUTTON_Y)
    );

    // Button Styling
    wxColour const button_color(30, 30, 30);   // Darker black
    wxColour const outline_color(255, 215, 0); // Yellow outline

    select_button->SetBackgroundColour(button_color);
    select_button->SetForegroundColour(outline_color);
    execute_button->SetBackgroundColour(button_color);
    execute_button->SetForegroundColour(outline_color);
    playback_button->SetBackgroundColour(button_color);
    playback_button->SetForegroundColour(outline_color);

    button_sizer->Add(
        select_button,
        0,
        wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL,
        BUTTON_SIZE
    );
    button_sizer->Add(
        execute_button,
        0,
        wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL,
        BUTTON_SIZE
    );
    button_sizer->Add(
        playback_button,
        0,
        wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL,
        BUTTON_SIZE
    );
    

    // Layout
    main_sizer->Add(title, 0, wxALIGN_CENTER | wxALIGN_TOP, TITLE_SIZE);
    main_sizer->Add(
        image_bitmap,
        0,
        wxALIGN_CENTER | wxALIGN_CENTER_HORIZONTAL,
        IMAGE_SIZE
    );
    main_sizer->AddStretchSpacer();
    main_sizer->Add(button_sizer, 0, wxALIGN_CENTER);
    main_sizer->AddStretchSpacer();

    panel->SetSizer(main_sizer);

    // Bind Events
    select_button->Bind(wxEVT_BUTTON, &MyFrame::onFileSelect, this);
    execute_button->Bind(wxEVT_BUTTON, &MyFrame::onExecute, this);
    playback_button->Bind(wxEVT_BUTTON, &MyFrame::onPlayback, this);
    Bind(wxEVT_SIZE, &MyFrame::onResize, this); // Resize event

    video_recorder = std::make_unique<VideoRecorder>(
        DEFAULT_VIDEO_RECORDER_WIDTH,
        DEFAULT_VIDEO_RECORDER_HEIGHT
    );
    has_recording = false;
}

// Handle window resizing
void MyFrame::onResize(wxSizeEvent& event) {
    wxSize const new_size = GetClientSize();
    double const new_width =
        new_size.GetWidth() * 0.6;             // Scale to 60% of window width
    double const new_height = new_width * 0.5; // Maintain aspect ratio

    wxFileName const exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exe_path);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString const image_path =
        file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png";

    // NOLINTBEGIN(clang-analyzer-optin.cplusplus.VirtualCall)
    wxImage image(image_path, wxBITMAP_TYPE_PNG);
    // NOLINTEND(clang-analyzer-optin.cplusplus.VirtualCall)

    if (image.IsOk()) {
        image.Rescale(
            static_cast<int32_t>(new_width),
            static_cast<int32_t>(new_height)
        );
        image_bitmap->SetBitmap(wxBitmap(image));
        Layout(); // Refresh layout
    }

    event.Skip(); // Allow normal event processing
}

// Handle file selection
void MyFrame::onFileSelect(wxCommandEvent& /*unused*/) {
    wxFileDialog open_file_dialog(
        this,
        "Open .slug File",
        "",
        "",
        "SLUG files (*.slug)|*.slug",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (open_file_dialog.ShowModal() == wxID_CANCEL) {
        return; // User cancelled the dialog
    }

    file_path = open_file_dialog.GetPath();

    // Ensure the file has a ".slug" extension
    if (!file_path.Lower().EndsWith(".slug")) {
        wxMessageBox(
            "Invalid file type! Please select a .slug file.",
            "Error",
            wxOK | wxICON_ERROR
        );
        return;
    }

    wxMessageBox(
        "You selected: " + file_path,
        "File Selected",
        wxOK | wxICON_INFORMATION
    );
    execute_button->Enable(
    ); // Enable execute button after selecting a valid file
}

void MyFrame::onExecute(wxCommandEvent& /*unused*/) {
    if (!file_path.IsEmpty()) {
        // Show the color dialog to let the user pick a color
        wxColourDialog colorDialog(this);
        if (colorDialog.ShowModal() == wxID_OK) {
            wxColour color = colorDialog.GetColourData().GetColour();  // Get selected color
            uint8_t selectedred = color.Red();   // Get the RGB values
            uint8_t selectedgreen = color.Green();
            uint8_t selectedblue = color.Blue();

            // Optionally, display the color that was selected in a message box or log it
            wxString colorMessage = wxString::Format("Selected Color: RGB(%d, %d, %d)", selectedred, selectedgreen, selectedblue);
            wxMessageBox(colorMessage, "Color Selected", wxOK | wxICON_INFORMATION);
            //SDL_SetTextureColorMod(texture, selectedred, selectedgreen, selectedblue);
             // Assuming GPU is a member of MyFrame, you would call the function to set the color
             if (gpu) {  // Make sure gpu is properly initialized
                gpu->setSelectedColor(selectedred, selectedgreen, selectedblue);
            }
            SDL_SetRenderDrawColor(renderer, selectedred, selectedgreen, selectedblue, 255);
            
        }

        try {
            video_recorder = std::make_unique<VideoRecorder>(
                DEFAULT_VIDEO_RECORDER_WIDTH,
                DEFAULT_VIDEO_RECORDER_HEIGHT
            );
            video_recorder->startRecording();

            Console banana(true);

            banana.gpu.setVideoRecorder(video_recorder.get());

            banana.run(std::string(file_path.ToStdString()));

            video_recorder->stopRecording();
            has_recording = (video_recorder->getFrameCount() > 0);

            if (has_recording) {
                playback_button->Enable();
                wxMessageBox(
                    "Gameplay recorded successfully. Click 'View Recording' to "
                    "replay.",
                    "Recording Complete",
                    wxOK | wxICON_INFORMATION
                );
            }
        } catch (const std::exception& e) {
            wxMessageBox(
                "Couldn't run file:\n" + file_path + "\nError: " + e.what(),
                "Execution Error",
                wxOK | wxICON_ERROR
            );
        }
    }
}
void MyFrame::onPlayback(wxCommandEvent& /*unused*/) {
    if (!has_recording || !video_recorder) {
        wxMessageBox(
            "No recording available to play back.",
            "Playback Error",
            wxOK | wxICON_ERROR
        );
        return;
    }

    if (video_recorder->initPlaybackWindow()) {
        video_recorder->play();

        bool running = true;
        while (running) {
            running = video_recorder->handleEvents();
            if (!running) {
                break;
            }
            video_recorder->updateDisplay();
            SDL_Delay(1); // Small delay to avoid consuming 100% CPU
        }

        video_recorder->closePlaybackWindow();
    } else {
        wxMessageBox(
            "Failed to initialize playback window.",
            "Playback Error",
            wxOK | wxICON_ERROR
        );
    }
}

#endif
