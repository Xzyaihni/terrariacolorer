#include <iostream>
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

private:
    void process_text();

    float _current_fuzz = 0;
    float _current_offset = 0;
    std::wstring _current_text = L"";

    wxTextCtrl* _output_text = nullptr;
    wxRichTextCtrl* _preview_text = nullptr;
};

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


    bottom_sizer->Add(sliders_sizer, wxSizerFlags(1).Expand());


    wxBoxSizer* top_sizer = new wxBoxSizer(wxVERTICAL);

    _output_text = new wxTextCtrl(this, wxID_ANY, "undefined");
    _output_text->SetEditable(false);
    top_sizer->Add(_output_text, wxSizerFlags(1).Expand());

    _preview_text = new wxRichTextCtrl(this, wxID_ANY, "undefined", wxDefaultPosition, wxDefaultSize,
        wxRE_READONLY);
    _preview_text->EnableVerticalScrollbar(false);
    top_sizer->Add(_preview_text, wxSizerFlags(1).Expand());

    process_text(); //set the initial text in the output thingy

    wxBoxSizer* full_sizer = new wxBoxSizer(wxVERTICAL);


    full_sizer->Add(top_sizer, wxSizerFlags(1).Expand().Border(wxBOTTOM, 15));
    full_sizer->Add(bottom_sizer, wxSizerFlags(1).Expand().Border(wxRIGHT | wxLEFT, 10));

    SetSizerAndFit(full_sizer);
}

void main_frame::text_changed(wxCommandEvent& event)
{
    _current_text = event.GetString().wc_str();
    process_text();
}

void main_frame::fuzz_changed(wxCommandEvent& event)
{
    _current_fuzz = event.GetInt()/100.0f;
    process_text();
}

void main_frame::offset_changed(wxCommandEvent& event)
{
    _current_offset = event.GetInt()/100.0f;
    process_text();
}

void main_frame::process_text()
{
    using namespace colorer;

    const std::vector<colorer::color> output_colors =
        generate_colors(_current_text, {gradient::rainbow, _current_fuzz, _current_offset});

    const std::wstring generated_text = generate_codes(_current_text, output_colors);

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
