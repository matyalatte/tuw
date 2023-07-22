#include "custom_wx_obj.h"

BEGIN_EVENT_TABLE(CustomTextCtrl, wxTextCtrl)
EVT_SET_FOCUS(CustomTextCtrl::OnSetFocusEmptyMessage)
EVT_KILL_FOCUS(CustomTextCtrl::OnKillFocusEmptyMessage)
END_EVENT_TABLE()

CustomTextCtrl::CustomTextCtrl(
    wxWindow* parent,
    wxWindowID id,
    const wxString& value,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) : wxTextCtrl(parent, id, value, pos, size, style, validator, name) {
    m_empty_message = empty_message;
    m_actual_value = value;
    ChangeValue(value);
    SetEmptyMessage();
}

const wxColour wxGREY(150, 150, 150);

void CustomTextCtrl::SetEmptyMessage() {
    if (m_actual_value == "" && m_empty_message != "") {
        ChangeValue(m_empty_message);
        SetForegroundColour(wxGREY);
    } else {
        ChangeValue(m_actual_value);
        SetForegroundColour(*wxBLACK);
    }
}

void CustomTextCtrl::OnKillFocusEmptyMessage(wxFocusEvent& event) {
    if (GetForegroundColour() != wxGREY) {
        m_actual_value = GetValue();
    }
    SetEmptyMessage();
    event.Skip();
}

void CustomTextCtrl::OnSetFocusEmptyMessage(wxFocusEvent& event) {
    wxString value = GetValue();
    if (GetForegroundColour() != wxGREY) {
        m_actual_value = value;
    } else if (value != m_empty_message && value.Left(m_empty_message.Len()) == m_empty_message) {
        // Somehow, droppped paths can be injected to empty message on MacOS.
        // This will fix the issue.
        m_actual_value = value.Mid(m_empty_message.Len());
    }
    ChangeValue(m_actual_value);
    SetForegroundColour(*wxBLACK);
    event.Skip();
}

void CustomTextCtrl::UpdateText(const wxString string) {
    m_actual_value = string;
    SetEmptyMessage();
}

wxString CustomTextCtrl::GetActualValue() {
    if (GetForegroundColour() != wxGREY) {
        return GetValue();
    }
    return m_actual_value;
}

bool CustomPickerBase::CustomCreatePickerBase(wxWindow* parent,
    wxWindowID id,
    const wxString& text,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) {
    style &= ~wxBORDER_MASK;

    if (!wxControl::Create(parent, id, pos, size, style | wxNO_BORDER | wxTAB_TRAVERSAL,
        validator, name))
        return false;

    SetMinSize(size);

    m_sizer = new wxBoxSizer(wxHORIZONTAL);

    m_custom_text_ctrl = new CustomTextCtrl(this, wxID_ANY, text,
        empty_message,
        wxDefaultPosition, wxDefaultSize,
        GetTextCtrlStyle(style));
    m_text = static_cast<wxTextCtrl*>(m_custom_text_ctrl);
    if (!m_text) {
        wxFAIL_MSG(wxT("wxPickerBase's textctrl creation failed"));
        return false;
    }

    m_text->SetMaxLength(32);

    m_custom_text_ctrl->UpdateText(text);

    m_custom_text_ctrl->Bind(wxEVT_TEXT, &CustomPickerBase::OnTextCtrlUpdate, this);
    m_custom_text_ctrl->Bind(wxEVT_KILL_FOCUS, &CustomPickerBase::OnTextCtrlKillFocus, this);
    m_custom_text_ctrl->Bind(wxEVT_DESTROY, &CustomPickerBase::OnTextCtrlDelete, this);

    m_sizer->Add(m_text,
        wxSizerFlags(1).CentreVertical().Border(wxRIGHT));
    return true;
}

bool CustomPickerBase::CustomCreateBase(wxWindow * parent,
    wxWindowID id,
    const wxString & path,
    const wxString & message,
    const wxString & wildcard,
    const wxString & empty_message,
    const wxString & button_label,
    const wxPoint & pos,
    const wxSize & size,
    long style,
    const wxValidator & validator,
    const wxString & name) {

    if (!CustomCreatePickerBase(parent, id, path, empty_message, pos, size,
        style, validator, name))
        return false;

    if (!HasFlag(wxFLP_OPEN) && !HasFlag(wxFLP_SAVE))
        m_windowStyle |= wxFLP_OPEN;  // wxFD_OPEN is the default

    // check that the styles are not contradictory
    wxASSERT_MSG(!(HasFlag(wxFLP_SAVE) && HasFlag(wxFLP_OPEN)),
        wxT("can't specify both wxFLP_SAVE and wxFLP_OPEN at once"));

    wxASSERT_MSG(!HasFlag(wxFLP_SAVE) || !HasFlag(wxFLP_FILE_MUST_EXIST),
        wxT("wxFLP_FILE_MUST_EXIST can't be used with wxFLP_SAVE"));

    wxASSERT_MSG(!HasFlag(wxFLP_OPEN) || !HasFlag(wxFLP_OVERWRITE_PROMPT),
        wxT("wxFLP_OVERWRITE_PROMPT can't be used with wxFLP_OPEN"));

    m_pickerIface = CreatePicker(this, path, message, button_label, wildcard);
    if (!m_pickerIface)
        return false;
    m_picker = m_pickerIface->AsControl();

    wxPickerBase::PostCreation();

    DoConnect(m_picker, this);

    if (m_text) m_text->SetMaxLength(512);

    return true;
}

void CustomPickerBase::UpdateTextCtrlFromPicker() {
    if (!m_text)
        return;
    if (m_pickerIface->GetPath() == m_text->GetValue()) {
        return;
    }
    m_text->ChangeValue(m_pickerIface->GetPath());
    m_custom_text_ctrl->UpdateText(m_pickerIface->GetPath());
}

void CustomPickerBase::UpdatePickerFromTextCtrl() {
    wxString newpath(GetFullPath());
    if (m_pickerIface->GetPath() != newpath) {
        m_pickerIface->SetPath(newpath);

        if (IsCwdToUpdate())
            wxSetWorkingDirectory(newpath);


        wxFileDirPickerEvent event(GetEventType(), this, GetId(), newpath);
        GetEventHandler()->ProcessEvent(event);
    }
}

CustomFilePicker::CustomFilePicker(
    wxWindow* parent,
    wxWindowID id,
    const wxString& path,
    const wxString& message,
    const wxString& wildcard,
    const wxString& empty_message,
    const wxString& button_label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) {
    if (!CustomCreateBase(
        parent, id, path, message, wildcard, empty_message, button_label,
        pos, size, style, validator, name))
        return;

    if (HasTextCtrl())
        GetTextCtrl()->AutoCompleteFileNames();
    m_text->SetDropTarget(new DropFilePath<CustomFilePicker>(this, m_custom_text_ctrl));
}

wxString CustomFilePicker::GetTextCtrlValue() const {
    return m_custom_text_ctrl->GetActualValue();
}

wxString CustomFilePicker::GetFullPath() {
    return wxFileName(m_custom_text_ctrl->GetActualValue()).GetFullPath();
}

CustomDirPicker::CustomDirPicker(
    wxWindow* parent,
    wxWindowID id,
    const wxString& path,
    const wxString& message,
    const wxString& empty_message,
    const wxString& button_label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) {
    if (!CustomCreateBase(
        parent, id, path, message, wxEmptyString, empty_message, button_label,
        pos, size, style, validator, name))
        return;

    if (HasTextCtrl())
        GetTextCtrl()->AutoCompleteDirectories();

    m_text->SetDropTarget(new DropFilePath<CustomDirPicker>(this, m_custom_text_ctrl));
}

wxString CustomDirPicker::GetTextCtrlValue() const {
    return m_custom_text_ctrl->GetActualValue();
}

wxString CustomDirPicker::GetFullPath() {
    return wxFileName::DirName(m_custom_text_ctrl->GetActualValue()).GetFullPath();
}
