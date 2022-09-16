#include "CustomWxObj.h"


BEGIN_EVENT_TABLE(CustomTextCtrl, wxTextCtrl)
EVT_SET_FOCUS(CustomTextCtrl::OnSetFocusEmptyMessage)
EVT_KILL_FOCUS(CustomTextCtrl::OnKillFocusEmptyMessage)
END_EVENT_TABLE()

CustomTextCtrl::CustomTextCtrl(
    wxWindow* parent,
    wxWindowID id,
    const wxString& value,
    const wxString& emptyMessage,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) :
    wxTextCtrl(parent, id, value, pos, size, style, validator, name)
{
    this->emptyMessage = emptyMessage;
    actualValue = value;
    ChangeValue(value);
    SetEmptyMessage();
}

const wxColour wxGREY(150, 150, 150);

void CustomTextCtrl::SetEmptyMessage() {
    if (actualValue == "" && emptyMessage != "") {
        ChangeValue(emptyMessage);
        SetForegroundColour(wxGREY);
    }
    else {
        ChangeValue(actualValue);
        SetForegroundColour(*wxBLACK);
    }
}


void CustomTextCtrl::OnKillFocusEmptyMessage(wxFocusEvent& event) {
    if (GetForegroundColour() != wxGREY) {
        actualValue = GetValue();
    }
    SetEmptyMessage();
    event.Skip();
}

void CustomTextCtrl::OnSetFocusEmptyMessage(wxFocusEvent& event) {
    wxString value = GetValue();
    if (GetForegroundColour() != wxGREY) {
        actualValue = value;
    }
    else if (value != emptyMessage && value.Left(emptyMessage.Len()) == emptyMessage){
        // Somehow, droppped paths can be injected to empty message on MacOS.
        // This will fix the issue.
        actualValue = value.Mid(emptyMessage.Len());
    }
    ChangeValue(actualValue);
    SetForegroundColour(*wxBLACK);
    event.Skip();
}

void CustomTextCtrl::UpdateText(const wxString string) {
    actualValue = string;
    SetEmptyMessage();
}

bool CustomTextCtrl::IsEmpty() {
    return actualValue == "";
}

wxString CustomTextCtrl::GetActualValue() {
    if (GetForegroundColour() != wxGREY) {
        return GetValue();
    }
    return actualValue;
}

//Drop target for path picker
template <typename T>
DropFilePath<T>::DropFilePath(T* frame, CustomTextCtrl* textCtrl) : wxFileDropTarget() {
    this->frame = frame;
    this->textCtrl = textCtrl;
}

template <typename T>
DropFilePath<T>::~DropFilePath() {
}

template <typename T>
bool DropFilePath<T>::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    this->textCtrl->UpdateText(filenames[0]);
    return 1;
}

CustomPickerBase::CustomPickerBase(): wxFileDirPickerCtrlBase()
{
    customTextCtrl = nullptr;
}

bool CustomPickerBase::CustomCreatePickerBase(wxWindow* parent,
    wxWindowID id,
    const wxString& text,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    style &= ~wxBORDER_MASK;

    if (!wxControl::Create(parent, id, pos, size, style | wxNO_BORDER | wxTAB_TRAVERSAL,
        validator, name))
        return false;

    SetMinSize(size);

    m_sizer = new wxBoxSizer(wxHORIZONTAL);

    if (HasFlag(wxPB_USE_TEXTCTRL))
    {
        customTextCtrl = new CustomTextCtrl(this, wxID_ANY, wxEmptyString,
            empty_message,
            wxDefaultPosition, wxDefaultSize,
            GetTextCtrlStyle(style));
        m_text = (wxTextCtrl*)customTextCtrl;
        if (!m_text)
        {
            wxFAIL_MSG(wxT("wxPickerBase's textctrl creation failed"));
            return false;
        }

        m_text->SetMaxLength(32);

        customTextCtrl->UpdateText(text);

        customTextCtrl->Bind(wxEVT_TEXT, &CustomPickerBase::OnTextCtrlUpdate, this);
        customTextCtrl->Bind(wxEVT_KILL_FOCUS, &CustomPickerBase::OnTextCtrlKillFocus, this);
        customTextCtrl->Bind(wxEVT_DESTROY, &CustomPickerBase::OnTextCtrlDelete, this);

        m_sizer->Add(m_text,
            wxSizerFlags(1).CentreVertical().Border(wxRIGHT));
    }
    return true;
}

bool CustomPickerBase::CustomCreateBase(wxWindow * parent,
    wxWindowID id,
    const wxString & path,
    const wxString & message,
    const wxString & wildcard,
    const wxString & empty_message,
    const wxPoint & pos,
    const wxSize & size,
    long style,
    const wxValidator & validator,
    const wxString & name)
{

    if (!CustomCreatePickerBase(parent, id, path, empty_message, pos, size,
        style, validator, name))
        return false;

    if (!HasFlag(wxFLP_OPEN) && !HasFlag(wxFLP_SAVE))
        m_windowStyle |= wxFLP_OPEN;     // wxFD_OPEN is the default

    // check that the styles are not contradictory
    wxASSERT_MSG(!(HasFlag(wxFLP_SAVE) && HasFlag(wxFLP_OPEN)),
        wxT("can't specify both wxFLP_SAVE and wxFLP_OPEN at once"));

    wxASSERT_MSG(!HasFlag(wxFLP_SAVE) || !HasFlag(wxFLP_FILE_MUST_EXIST),
        wxT("wxFLP_FILE_MUST_EXIST can't be used with wxFLP_SAVE"));

    wxASSERT_MSG(!HasFlag(wxFLP_OPEN) || !HasFlag(wxFLP_OVERWRITE_PROMPT),
        wxT("wxFLP_OVERWRITE_PROMPT can't be used with wxFLP_OPEN"));

    m_pickerIface = CreatePicker(this, path, message, wildcard);
    if (!m_pickerIface)
        return false;
    m_picker = m_pickerIface->AsControl();

    wxPickerBase::PostCreation();

    DoConnect(m_picker, this);

    if (m_text) m_text->SetMaxLength(512);

    return true;
}

void CustomPickerBase::UpdateTextCtrlFromPicker()
{
    if (!m_text)
        return;
    if (m_pickerIface->GetPath() == m_text->GetValue()) {
        return;
    }
    m_text->ChangeValue(m_pickerIface->GetPath());
    customTextCtrl->UpdateText(m_pickerIface->GetPath());
}

void CustomPickerBase::UpdatePickerFromTextCtrl()
{
    wxString newpath(GetTextCtrlValue());
    if (m_pickerIface->GetPath() != newpath)
    {
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
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    CustomFilePicker::Create(parent, id, path, message, wildcard, empty_message, pos, size, style, validator, name);
}

bool CustomFilePicker::Create(wxWindow* parent,
    wxWindowID id,
    const wxString& path,
    const wxString& message,
    const wxString& wildcard,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    if (!CustomCreateBase
    (
        parent, id, path, message, wildcard, empty_message,
        pos, size, style, validator, name
    ))
        return false;

    if (HasTextCtrl())
        GetTextCtrl()->AutoCompleteFileNames();
    m_text->SetDropTarget(new DropFilePath<CustomFilePicker>(this, customTextCtrl));

    return true;
}

wxString CustomFilePicker::GetTextCtrlValue() const
{
    return wxFileName(customTextCtrl->GetActualValue()).GetFullPath();
}

CustomDirPicker::CustomDirPicker(
    wxWindow* parent,
    wxWindowID id,
    const wxString& path,
    const wxString& message,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    CustomDirPicker::Create(parent, id, path, message, empty_message, pos, size, style, validator, name);
}

bool CustomDirPicker::Create(wxWindow* parent,
    wxWindowID id,
    const wxString& path,
    const wxString& message,
    const wxString& empty_message,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    if (!CustomCreateBase
    (
        parent, id, path, message, wxEmptyString, empty_message,
        pos, size, style, validator, name
    ))
        return false;

    if (HasTextCtrl())
        GetTextCtrl()->AutoCompleteDirectories();

    m_text->SetDropTarget(new DropFilePath<CustomDirPicker>(this, customTextCtrl));

    return true;
}

wxString CustomDirPicker::GetTextCtrlValue() const
{
    return wxFileName::DirName(customTextCtrl->GetActualValue()).GetFullPath();
}
