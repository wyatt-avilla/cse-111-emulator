// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3
// and then further modified with this thread of chat
// https://chatgpt.com/share/67c667e9-5dc0-8013-a471-56d7cadf7081


// This is the chats used for the code used in changing color of pixel
// https://claude.ai/share/e2202135-6906-4180-a017-fe71cb2a1a5a
// https://chatgpt.com/share/67cfe91c-19d4-8011-930b-dc9923808d79

#ifndef HEADLESS_BUILD

#include "gui.h"

#include "console.h"
#include "gpu.h"
#include "vr.h"

#include <iostream>
#include <thread>
#include <wx/colordlg.h> // For the color dialog
#include <wx/colour.h>   // For wxColour dialog
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/stdpaths.h>

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
      ), gpu(new GPU()) {
    
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

void MyFrame::onFileSelect(wxCommandEvent& /*unused*/) {
    // Open the file dialog
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

    // Check if the file is in the games directory
    wxFileName const file_name(file_path);
    wxString parent_dir = "";
    if (file_name.GetDirCount() > 0) {
        parent_dir = file_name.GetDirs().Last().Lower();
    }

    if (parent_dir != "games") {
        wxMessageBox(
            "Only .slug files from the 'games' directory are allowed.\n"
            "Please select a file from the games directory.",
            "Invalid Directory",
            wxOK | wxICON_ERROR
        );
        return;
    }

    // Ask user to confirm file selection with Yes/No buttons
    int const confirm_result = wxMessageBox(
        "You selected: " + file_path + "\n\nDo you want to use this file?",
        "Confirm File Selection",
        wxYES_NO | wxICON_QUESTION
    );

    if (confirm_result != wxYES) {
        // User didn't confirm, go back to file selection
        return;
    }

    // Enable execute button and disable playback when a new file is selected
    execute_button->Enable();
    playback_button->Disable();
    has_recording = false; // Reset recording state
}

void MyFrame::onExecute(wxCommandEvent& /*unused*/) {
    if (!file_path.IsEmpty()) {
        bool color_chosen = false;

        // Set default gray color first
        uint8_t const default_gray_value = 128;
        if (gpu != nullptr) {
            gpu->setSelectedColor(
                default_gray_value,
                default_gray_value,
                default_gray_value
            );
            std::cout << "Setting default GRAY: R=128, G=128, B=128"
                      << std::endl;
        } else {
            std::cerr << "Error: GPU instance is null" << std::endl;
            return;
        }

        while (!color_chosen) {
            // Create color data for each iteration
            wxColourData color_data;
            wxColour const default_gray(128, 128, 128); // Medium gray
            color_data.SetColour(default_gray);

            // Create a new dialog each time
            auto* color_dialog = new wxColourDialog(this, &color_data);

            if (color_dialog == nullptr) {
                std::cerr << "Failed to create color dialog" << std::endl;
                return;
            }

            int const result = color_dialog->ShowModal();

            // If user cancels/closes the color dialog, don't proceed
            if (result != wxID_OK) {
                color_dialog->Destroy();
                return;
            }

            // Get the color data safely
            wxColour const color = color_dialog->GetColourData().GetColour();
            uint8_t const selected_red = color.Red();
            uint8_t const selected_green = color.Green();
            uint8_t const selected_blue = color.Blue();

            // Clean up the dialog before continuing
            color_dialog->Destroy();

            // Check if the color is black
            if (selected_red == 0 && selected_green == 0 && selected_blue == 0) {
                wxMessageBox(
                    "Pure black (0,0,0) is not allowed. Please select a "
                    "different color.",
                    "Invalid Color",
                    wxOK | wxICON_WARNING
                );
                continue;
            }

            // Display the selected color information as a confirmation
            wxString const color_message = wxString::Format(
                "Selected Color: RGB(%d, %d, %d)\n\nDo you want to use this "
                "color?",
                selected_red,
                selected_green,
                selected_blue
            );

            // Use a simple message dialog for confirmation
            int const confirm_result = wxMessageBox(
                color_message,
                "Confirm Color Selection",
                wxYES_NO | wxICON_QUESTION,
                this
            );

            if (confirm_result != wxYES) {
                // User didn't confirm, go back to color selection
                continue;
            }

            // User confirmed - set the color
            color_chosen = true;
            std::cout << "GUI COLOR SELECTION: R=" << (int) selected_red
                      << ", G=" << (int) selected_green
                      << ", B=" << (int) selected_blue << std::endl;

            if (gpu != nullptr) {
                gpu->setSelectedColor(selected_red, selected_green, selected_blue);
            } else {
                std::cerr << "Error: GPU instance is null when setting color"
                          << std::endl;
                return;
            }
        }

        try {
            video_recorder = std::make_unique<VideoRecorder>(
                DEFAULT_VIDEO_RECORDER_WIDTH,
                DEFAULT_VIDEO_RECORDER_HEIGHT
            );

            if (!video_recorder) {
                throw std::runtime_error("Failed to create video recorder");
            }

            video_recorder->startRecording();

            Console banana(true);

            // Important: Transfer the color from the GUI's GPU to the console's
            // GPU
            banana.gpu.setSelectedColor(
                gpu->getSelectedColorR(),
                gpu->getSelectedColorG(),
                gpu->getSelectedColorB()
            );

            // Pass the selected color to the video recorder
            if (video_recorder) {
                video_recorder->setColorTint(
                    gpu->getSelectedColorR(),
                    gpu->getSelectedColorG(),
                    gpu->getSelectedColorB()
                );
            }

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

            // Reset the file path and disable only the execute button
            file_path = "";
            execute_button->Disable();
            // Note: We don't disable the playback button here to allow replays

        } catch (const std::exception& e) {
            wxMessageBox(
                "Couldn't run file:\n" + file_path + "\nError: " + e.what(),
                "Execution Error",
                wxOK | wxICON_ERROR
            );

            // Reset file path and disable execute button on error
            file_path = "";
            execute_button->Disable();
            // Don't change playback button state on error
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
