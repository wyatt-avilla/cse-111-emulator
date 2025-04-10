// got a large put of this code from chat gpt
// https://chatgpt.com/share/67c212e0-dae4-8013-b5b9-2400c824b5e3
// and then further modified with this thread of chat
// https://chatgpt.com/share/67c667e9-5dc0-8013-a471-56d7cadf7081


// These are the chats used for fixing the pixel color issue
// https://claude.ai/share/e2202135-6906-4180-a017-fe71cb2a1a5a
// https://chatgpt.com/share/67cfe91c-19d4-8011-930b-dc9923808d79

#ifndef HEADLESS_BUILD

#include "gui.h"

#include "console.h"
#include "disassembler.h"
#include "gpu.h"
#include "vr.h"

#include <iostream>
#include <thread>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/stdpaths.h>

const int32_t WX_FRAME_X_POSTION = 500;
const int32_t WX_FRAME_Y_POSTION = 400;
const int32_t GREY_COLOR = 40;
const int32_t FOREGROUND_COLOUR_1 = 255;
const int32_t FOREGROUND_COLOUR_2 = 215;
const int32_t FOREGROUND_COLOUR_3 = 0;
const int32_t FONT_SIZE = 14;
const int32_t RESCALE_X = 300;
const int32_t RESCALE_Y = 150;
const int32_t SELECT_BUTTON_X = 150;
const int32_t SELECT_BUTTON_Y = 40;
const int32_t EXECUTE_BUTTON_X = 150;
const int32_t EXECUTE_BUTTON_Y = 40;
const int32_t BUTTON_SIZE = 10;
const int32_t IMAGE_SIZE = 10;
const int32_t TITLE_SIZE = 20;

bool MyApp::OnInit() {
    auto* console = new Console(true);
    auto* frame = new MyFrame(console);
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(Console* console) // NOLINT(readability-function-size)
    : wxFrame(
          nullptr,
          wxID_ANY,
          "Banana Emulator",
          wxDefaultPosition,
          wxSize(WX_FRAME_X_POSTION, WX_FRAME_Y_POSTION)
      ),
      console(console) {

    auto* panel = new wxPanel(this, wxID_ANY);
    panel->SetBackgroundColour(wxColour(GREY_COLOR, GREY_COLOR, GREY_COLOR));

    auto* main_sizer = new wxBoxSizer(wxVERTICAL);


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
    );
    title->SetFont(wxFont(
        FONT_SIZE,
        wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL,
        wxFONTWEIGHT_BOLD
    ));


    wxFileName const exe_path(wxStandardPaths::Get().GetExecutablePath());
    wxFileName file(exe_path);
    file.Normalize(wxPATH_NORM_ABSOLUTE);
    wxString const image_path =
        file.GetPath() + wxFILE_SEP_PATH + "../src/banana.png";


    wxInitAllImageHandlers();
    wxImage image(image_path, wxBITMAP_TYPE_PNG);
    if (!image.IsOk()) {
        wxMessageBox(
            "Failed to load image: " + image_path,
            "Error",
            wxOK | wxICON_ERROR
        );
    }

    image.Rescale(RESCALE_X, RESCALE_Y);
    image_bitmap = new wxStaticBitmap(panel, wxID_ANY, wxBitmap(image));


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
    execute_button->Disable();

    disassemble_button = new wxButton(
        panel,
        wxID_ANY,
        "Disassemble",
        wxDefaultPosition,
        wxSize(EXECUTE_BUTTON_X, EXECUTE_BUTTON_Y)
    );
    disassemble_button->Disable(); // Initially disabled

    // Add playback button
    playback_button = new wxButton(
        panel,
        wxID_ANY,
        "View Recording",
        wxDefaultPosition,
        wxSize(EXECUTE_BUTTON_X, EXECUTE_BUTTON_Y)
    );
    playback_button->Disable();


    wxColour const button_color(30, 30, 30);
    wxColour const outline_color(255, 215, 0);

    select_button->SetBackgroundColour(button_color);
    select_button->SetForegroundColour(outline_color);
    execute_button->SetBackgroundColour(button_color);
    execute_button->SetForegroundColour(outline_color);
    disassemble_button->SetBackgroundColour(button_color);
    disassemble_button->SetForegroundColour(outline_color);
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
        disassemble_button,
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


    select_button->Bind(wxEVT_BUTTON, &MyFrame::onFileSelect, this);
    execute_button->Bind(wxEVT_BUTTON, &MyFrame::onExecute, this);
    disassemble_button->Bind(wxEVT_BUTTON, &MyFrame::onDisassemble, this);
    playback_button->Bind(wxEVT_BUTTON, &MyFrame::onPlayback, this);
    Bind(wxEVT_SIZE, &MyFrame::onResize, this);

    has_recording = false;
}


void MyFrame::onResize(wxSizeEvent& event) {
    wxSize const new_size = GetClientSize();
    double const new_width = new_size.GetWidth() * 0.6;
    double const new_height = new_width * 0.5;

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
        Layout();
    }

    event.Skip();
}

void MyFrame::onFileSelect( // NOLINT(readability-function-size)
    wxCommandEvent&
    /*unused*/
) {
    wxFileDialog open_file_dialog(
        this,
        "Open .slug File",
        "",
        "",
        "SLUG files (*.slug)|*.slug",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (open_file_dialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    file_path = open_file_dialog.GetPath();

    if (!file_path.Lower().EndsWith(".slug")) {
        wxMessageBox(
            "Invalid file type! Please select a .slug file.",
            "Error",
            wxOK | wxICON_ERROR
        );
        return;
    }

    int32_t const confirm_result = wxMessageBox(
        "You selected: " + file_path + "\n\nDo you want to use this file?",
        "Confirm File Selection",
        wxYES_NO | wxICON_QUESTION
    );

    if (confirm_result != wxYES) {
        return;
    }

    execute_button->Enable();
    disassemble_button->Enable();
    playback_button->Disable();
    has_recording = false;
}

void MyFrame::onExecute( // NOLINT(readability-function-cognitive-complexity
                         // readability-function-size)
    wxCommandEvent&      /*unused*/
) {
    if (!file_path.IsEmpty()) {
        bool color_chosen = false;


        uint8_t const default_gray_value = 128;
        console->filter.setColor(
            default_gray_value,
            default_gray_value,
            default_gray_value
        );

        while (!color_chosen) {

            wxColourData color_data;
            wxColour const default_gray(128, 128, 128);
            color_data.SetColour(default_gray);


            auto* color_dialog = new wxColourDialog(this, &color_data);

            if (color_dialog == nullptr) {
                std::cerr << "Failed to create color dialog" << std::endl;
                return;
            }

            int32_t const result = color_dialog->ShowModal();


            if (result != wxID_OK) {
                color_dialog->Destroy();
                return;
            }


            wxColour const color = color_dialog->GetColourData().GetColour();
            uint8_t const selected_red = color.Red();
            uint8_t const selected_green = color.Green();
            uint8_t const selected_blue = color.Blue();


            color_dialog->Destroy();


            if (selected_red == 0 && selected_green == 0 &&
                selected_blue == 0) {
                wxMessageBox(
                    "Pure black (0,0,0) is not allowed. Please select a "
                    "different color.",
                    "Invalid Color",
                    wxOK | wxICON_WARNING
                );
                continue;
            }


            wxString const color_message = wxString::Format(
                "Selected Color: RGB(%d, %d, %d)\n\nDo you want to use this "
                "color?",
                selected_red,
                selected_green,
                selected_blue
            );


            int32_t const confirm_result = wxMessageBox(
                color_message,
                "Confirm Color Selection",
                wxYES_NO | wxICON_QUESTION,
                this
            );

            if (confirm_result != wxYES) {

                continue;
            }


            color_chosen = true;
            console->filter
                .setColor(selected_red, selected_green, selected_blue);
        }

        try {
            console->video_recorder.startRecording();

            console->run(std::string(file_path.ToStdString()));

            console->video_recorder.stopRecording();

            has_recording = (console->video_recorder.getFrameCount() > 0);

            if (has_recording) {
                playback_button->Enable();
                wxMessageBox(
                    "Gameplay recorded successfully. Click 'View Recording' to "
                    "replay.",
                    "Recording Complete",
                    wxOK | wxICON_INFORMATION
                );
            }


            file_path = "";
            execute_button->Disable();


        } catch (const std::exception& e) {
            wxMessageBox(
                "Couldn't run file:\n" + file_path + "\nError: " + e.what(),
                "Execution Error",
                wxOK | wxICON_ERROR
            );


            file_path = "";
            execute_button->Disable();
        }
    }
}

void MyFrame::onDisassemble(wxCommandEvent& /*unused*/) {
    if (!file_path.IsEmpty()) {
        try {
            Disassembler disassembler(file_path.ToStdString());
            const std::string disassemble_path = disassembler.disassemble();
            wxMessageBox(
                "Disassembled file stored in " + disassemble_path,
                "Disassembly Complete",
                wxOK | wxICON_INFORMATION
            );
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
    if (!has_recording) {
        wxMessageBox(
            "No recording available to play back.",
            "Playback Error",
            wxOK | wxICON_ERROR
        );
        return;
    }

    if (console->video_recorder.initPlaybackWindow()) {
        console->video_recorder.play();

        bool running = true;
        while (running) {
            running = console->video_recorder.handleEvents();
            if (!running) {
                break;
            }
            console->video_recorder.updateDisplay();
            SDL_Delay(1);
        }

        console->video_recorder.closePlaybackWindow();
    } else {
        wxMessageBox(
            "Failed to initialize playback window.",
            "Playback Error",
            wxOK | wxICON_ERROR
        );
    }
}

#endif
