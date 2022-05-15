#include <iostream>
#include <filesystem>
#include <fstream>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/richtext/richtextctrl.h>

#include "colorer.h"

class color_app : public wxApp
{
public:
    virtual bool OnInit();
};

class main_frame : public wxFrame
{
public:
    main_frame();

    void text_changed(wxCommandEvent& event);
    void fuzz_changed(wxCommandEvent& event);
    void offset_changed(wxCommandEvent& event);

    void select_script(wxCommandEvent& event);

private:
    void output_exception(const lua_exception& e);

    void load_lua_names();
    void load_lua(const std::string filename);

    void check_current_choice();
    std::string current_selected();
    static std::filesystem::path filename_path(const std::string filename);

    void process_text();

    float _current_fuzz = 0;
    float _prev_fuzz = 0;

    float _current_offset = 0;
    float _prev_offset = 0;

    std::wstring _current_text = L"";
    std::wstring _prev_text = L"";

    wxTextCtrl* _output_text = nullptr;
    wxRichTextCtrl* _preview_text = nullptr;

    wxChoice* _script_choice = nullptr;
    std::filesystem::file_time_type _folder_timestamp;

    std::filesystem::file_time_type _current_timestamp;

    colorer _main_colorer;
};

const int update_delay = 250;
const std::filesystem::path gradients_folder = "gradients";


wxIMPLEMENT_APP(color_app);

bool color_app::OnInit()
{
    main_frame* frame = new main_frame();

    frame->Show(true);

    return true;
}

main_frame::main_frame()
: wxFrame(NULL, wxID_ANY, "colorererererer", wxDefaultPosition)
{
    wxBoxSizer* bottom_sizer = new wxBoxSizer(wxVERTICAL);

    const std::wstring default_text = L"test тест";
    _current_text = default_text;

    wxTextCtrl* user_text = new wxTextCtrl(this, wxID_ANY, default_text, wxDefaultPosition, wxSize(400,30));
    user_text->Bind(wxEVT_TEXT, &main_frame::text_changed, this);

    bottom_sizer->Add(user_text, wxSizerFlags(1).Expand());

    wxBoxSizer* sliders_sizer = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* fuzz_slider_sizer = new wxBoxSizer(wxVERTICAL);

    wxSlider* fuzz_slider = new wxSlider(this, wxID_ANY, 75, 0, 100);
    _current_fuzz = 0.75f;
    fuzz_slider->Bind(wxEVT_SLIDER, &main_frame::fuzz_changed, this);

    fuzz_slider_sizer->Add(new wxStaticText(this, wxID_ANY, "fuzz"));
    fuzz_slider_sizer->Add(fuzz_slider, wxSizerFlags(1).Expand());
    sliders_sizer->Add(fuzz_slider_sizer, wxSizerFlags(1).Expand());


    wxBoxSizer* offset_slider_sizer = new wxBoxSizer(wxVERTICAL);

    wxSlider* offset_slider = new wxSlider(this, wxID_ANY, 50, 0, 100);
    _current_offset = 0.5f;
    offset_slider->Bind(wxEVT_SLIDER, &main_frame::offset_changed, this);

    offset_slider_sizer->Add(new wxStaticText(this, wxID_ANY, "offset"));
    offset_slider_sizer->Add(offset_slider, wxSizerFlags(1).Expand());
    sliders_sizer->Add(offset_slider_sizer, wxSizerFlags(1).Expand());

    _script_choice = new wxChoice(this, wxID_ANY);
    _script_choice->Bind(wxEVT_CHOICE, &main_frame::select_script, this);
    sliders_sizer->Add(_script_choice, wxSizerFlags(1).Expand());


    bottom_sizer->Add(sliders_sizer, wxSizerFlags(1).Expand());


    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

    _output_text = new wxTextCtrl(this, wxID_ANY, "undefined");
    _output_text->SetEditable(false);
    top_sizer->Add(_output_text, wxSizerFlags(1).Expand());

    _preview_text = new wxRichTextCtrl(this, wxID_ANY, "undefined", wxDefaultPosition, wxDefaultSize,
        wxRE_READONLY);
    _preview_text->EnableVerticalScrollbar(false);
    top_sizer->Add(_preview_text, wxSizerFlags(1).Expand());

    wxBoxSizer* full_sizer = new wxBoxSizer(wxVERTICAL);


    full_sizer->Add(top_sizer, wxSizerFlags(1).Expand().Border(wxBOTTOM, 15));
    full_sizer->Add(bottom_sizer, wxSizerFlags(1).Expand().Border(wxRIGHT | wxLEFT, 10));

    SetSizerAndFit(full_sizer);

    wxTimer* process_timer = new wxTimer(this, wxID_ANY);
    Bind(wxEVT_TIMER, [this](wxTimerEvent evt){check_current_choice(); load_lua_names(); process_text();});

    load_lua_names();
    const std::string c_name = current_selected();
    if(c_name!="")
    {
        load_lua(c_name);
    }

    process_timer->Start(update_delay);

    process_text();
}

void main_frame::text_changed(wxCommandEvent& event)
{
    _current_text = event.GetString().wc_str();
}

void main_frame::fuzz_changed(wxCommandEvent& event)
{
    _current_fuzz = event.GetInt()/100.0f;
}

void main_frame::offset_changed(wxCommandEvent& event)
{
    _current_offset = event.GetInt()/100.0f;
}

void main_frame::select_script(wxCommandEvent& event)
{
    load_lua(event.GetString().ToStdString());
    process_text();
}

void main_frame::output_exception(const lua_exception& e)
{
    wxMessageBox(e.what(), "lua error", wxOK, this);
}

void main_frame::load_lua_names()
{
    const auto c_time = std::filesystem::last_write_time(gradients_folder);
    if(_folder_timestamp!=c_time)
    {
        _folder_timestamp = c_time;

        _script_choice->Clear();

        for(auto const& c_entry : std::filesystem::directory_iterator{gradients_folder})
        {
            if(c_entry.path().extension()==".lua")
            {
                _script_choice->Append(wxString(c_entry.path().stem()));
            }
        }

        if(_script_choice->GetCount()>0)
            _script_choice->SetSelection(0);
    }
}

void main_frame::load_lua(const std::string filename)
{
    const auto c_path = filename_path(filename);
    _current_timestamp = std::filesystem::last_write_time(c_path);

    try
    {
        _main_colorer.update_lua(c_path.string());
    } catch(lua_exception& e)
    {
        output_exception(e);
    }
}

void main_frame::check_current_choice()
{
    const std::string c_name = current_selected();

    if(c_name!="")
    {
        const auto c_path = filename_path(c_name);
        if(std::filesystem::exists(c_path))
        {
            const auto c_time = std::filesystem::last_write_time(c_path);
            if(_current_timestamp!=c_time)
            {
                load_lua(c_name);
            }
        }
    }
}

std::string main_frame::current_selected()
{
    const int c_index = _script_choice->GetSelection();
    if(c_index<_script_choice->GetCount())
    {
        wxString selected_string = _script_choice->GetString(c_index);
        return selected_string.ToStdString();
    }

    return "";
}

std::filesystem::path main_frame::filename_path(const std::string filename)
{
    return gradients_folder/(filename+std::string(".lua"));
}

void main_frame::process_text()
{
    if(_main_colorer.updated && _current_text==_prev_text && _current_fuzz==_prev_fuzz && _current_offset==_prev_offset)
    {
        return;
    } else
    {
        _prev_text = _current_text;
        _prev_fuzz = _current_fuzz;
        _prev_offset = _current_offset;
        _main_colorer.updated = true;
    }

    std::vector<colorer::color> output_colors;
    try
    {
         output_colors = _main_colorer.generate_colors(_current_text, {_current_fuzz, _current_offset});
    } catch(lua_exception& e)
    {
        output_exception(e);
        return;
    }

    const std::wstring generated_text = colorer::generate_codes(_current_text, output_colors);

    if(_output_text!=nullptr)
        _output_text->SetValue(generated_text);

    if(_preview_text!=nullptr)
    {
        wxRichTextCtrl& c_text = *_preview_text;

        c_text.BeginSuppressUndo();
        c_text.Clear();

        for(int i = 0; i < output_colors.size(); ++i)
        {
            const colorer::color c_color = output_colors[i];
            c_text.BeginTextColour(wxColour(
                static_cast<unsigned char>(c_color.r*255),
                static_cast<unsigned char>(c_color.g*255),
                static_cast<unsigned char>(c_color.b*255)));

            c_text.WriteText(_current_text[i]);

            c_text.EndTextColour();
        }

        c_text.EndSuppressUndo();
    }
}
