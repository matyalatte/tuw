// Custom classes for wx objects

#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "wx/wx.h"
#include "wx/filepicker.h"
#include "wx/dnd.h"
#include "json_utils.h"

// TextCtrl that can show a message when its text box is empty.
class CustomTextCtrl : public wxTextCtrl {
 private:
    wxString m_empty_message;
    wxString m_actual_value;
 public:
    CustomTextCtrl(wxWindow* parent, wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxString& empty_message = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxTextCtrlNameStr);

    void OnSetFocusEmptyMessage(wxFocusEvent& event);
    void OnKillFocusEmptyMessage(wxFocusEvent& event);
    void SetEmptyMessage();
    void UpdateText(const wxString string);
    wxString GetActualValue();
    DECLARE_EVENT_TABLE();
};

// Drop target for path picker
// It can input file and folder paths to text box.
template <typename T>
class DropFilePath : public wxFileDropTarget{
 private:
    T* m_frame;
    CustomTextCtrl* m_text_ctrl;

 public:
    DropFilePath(T* frame, CustomTextCtrl* text_ctrl): wxFileDropTarget() {
        m_frame = frame;
        m_text_ctrl = text_ctrl;
    }
    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames){
        m_text_ctrl->UpdateText(filenames[0]);
        return 1;
    }
};

// Customized wxFileDirPickerCtrlBase
// It has drop target for text box, and can show a message when its text box is empty.
class CustomPickerBase : public wxFileDirPickerCtrlBase {
 protected:
    CustomTextCtrl* m_custom_text_ctrl;

 public:
    CustomPickerBase() : wxFileDirPickerCtrlBase() {
        m_custom_text_ctrl = nullptr;
    }

    // Customized wxPickerBase::CreateBase
    bool CustomCreatePickerBase(wxWindow* parent,
        wxWindowID id,
        const wxString& text = wxEmptyString,
        const wxString& empty_message = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxASCII_STR(wxButtonNameStr));

    // Customized wxFileDirPickerCtrlBase::CreateBase
    bool CustomCreateBase(wxWindow* parent,
        wxWindowID id,
        const wxString& path,
        const wxString& message,
        const wxString& wildcard,
        const wxString& empty_message,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxValidator& validator,
        const wxString& name);
    void UpdateTextCtrlFromPicker();
    void UpdatePickerFromTextCtrl();
    virtual wxString GetFullPath() {return "";}
};

// Customized wxFilePicker
// It has a drop target for text box, and can show a message when its text box is empty.
class CustomFilePicker : public CustomPickerBase {
 public:
    CustomFilePicker(wxWindow* parent, wxWindowID id,
        const wxString& path = wxEmptyString,
        const wxString& message = wxFileSelectorPromptStr,
        const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
        const wxString& empty_message = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxFLP_DEFAULT_STYLE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxFilePickerCtrlNameStr);

    wxString GetTextCtrlValue() const wxOVERRIDE;
    wxString GetFullPath() wxOVERRIDE;

    bool IsCwdToUpdate() const wxOVERRIDE {
        return HasFlag(wxFLP_CHANGE_DIR);
    }

    wxEventType GetEventType() const wxOVERRIDE {
        return wxEVT_FILEPICKER_CHANGED;
    }

    virtual void DoConnect(wxControl* sender, wxFileDirPickerCtrlBase* eventSink) wxOVERRIDE {
        sender->Bind(wxEVT_FILEPICKER_CHANGED,
            &wxFileDirPickerCtrlBase::OnFileDirChange, eventSink);
    }

 protected:
    virtual
        wxFileDirPickerWidgetBase* CreatePicker(wxWindow* parent,
            const wxString& path,
            const wxString& message,
            const wxString& wildcard) wxOVERRIDE {
        return new wxFilePickerWidget(parent, wxID_ANY,
            wxGetTranslation(wxFilePickerWidgetLabel),
            path, message, wildcard,
            wxDefaultPosition, wxDefaultSize,
            GetPickerStyle(GetWindowStyle()));
    }

    long GetPickerStyle(long style) const wxOVERRIDE {
        return style & (wxFLP_OPEN |
            wxFLP_SAVE |
            wxFLP_OVERWRITE_PROMPT |
            wxFLP_FILE_MUST_EXIST |
            wxFLP_CHANGE_DIR |
            wxFLP_USE_TEXTCTRL |
            wxFLP_SMALL);
    }
};

// Customized wxDirPicker
// It has a drop target for text box, and can show a message when its text box is empty.
class CustomDirPicker : public CustomPickerBase {
 public:
    CustomDirPicker(wxWindow* parent, wxWindowID id,
        const wxString& path = wxEmptyString,
        const wxString& message = wxFileSelectorPromptStr,
        const wxString& empty_message = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxFLP_DEFAULT_STYLE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxFilePickerCtrlNameStr);

    wxString GetTextCtrlValue() const wxOVERRIDE;
    wxString GetFullPath() wxOVERRIDE;

    bool IsCwdToUpdate() const wxOVERRIDE {
        return HasFlag(wxDIRP_CHANGE_DIR);
    }

    wxEventType GetEventType() const wxOVERRIDE {
        return wxEVT_DIRPICKER_CHANGED;
    }

    virtual void DoConnect(wxControl* sender, wxFileDirPickerCtrlBase* eventSink) wxOVERRIDE {
        sender->Bind(wxEVT_DIRPICKER_CHANGED,
            &wxFileDirPickerCtrlBase::OnFileDirChange, eventSink);
    }


 protected:
    virtual
        wxFileDirPickerWidgetBase* CreatePicker(wxWindow* parent,
            const wxString& path,
            const wxString& message,
            const wxString& WXUNUSED(wildcard)) wxOVERRIDE {
        return new wxDirPickerWidget(parent, wxID_ANY,
            wxGetTranslation(wxDirPickerWidgetLabel),
            path, message,
            wxDefaultPosition, wxDefaultSize,
            GetPickerStyle(GetWindowStyle()));
    }

    long GetPickerStyle(long style) const wxOVERRIDE {
        return style & (wxDIRP_DIR_MUST_EXIST |
            wxDIRP_CHANGE_DIR |
            wxDIRP_USE_TEXTCTRL |
            wxDIRP_SMALL);
    }
};
