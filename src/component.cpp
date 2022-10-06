#include "component.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(nlohmann::json j, bool has_string) {
    m_widget = nullptr;
    m_has_string = has_string;
    m_label = j["label"];
    m_add_quotes = j.value("add_quotes", false);
}

wxString Component::GetString() {
    wxString str = GetRawString();
    if (m_add_quotes) {
        return "\"" + str + "\"";
    }
    return str;
}

#ifdef _WIN32
// utf-16 to utf-8 for Windows
std::string WStringToUTF8(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

std::wstring UTF8ToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}
#endif

nlohmann::json Component::GetConfig() {
    return {};
}

std::string Component::GetLabel() {
    return m_label;
}

bool Component::HasString() {
    return m_has_string;
}

Component* Component::PutComponent(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j) {
    Component* comp = nullptr;
    std::string type = j["type"];
    if (type == "static_text") {  // statc text
        comp = new StaticText(panel, sizer, j);
    } else if (type == "file") {  // file picker
        comp = new FilePicker(panel, sizer, j);
    } else if (type == "folder") {  // dir picker
        comp = new DirPicker(panel, sizer, j);
    } else if (type == "choice") {  // choice
        comp = new Choice(panel, sizer, j);
    } else if (type == "check") {  // checkbox
        comp = new CheckBox(panel, sizer, j);
    } else if (type == "checks" || type == "check_array") {  // checkArray
        comp = new CheckArray(panel, sizer, j);
    } else if (type == "text" || type == "text_box") {  // text box
        comp = new TextBox(panel, sizer, j);
    } else {
        std::cout << "[UpdatePanel] Unknown component type detected. (" << type << ")" << std::endl;
    }
    return comp;
}

const bool HAS_STRING = true;
const bool NOT_STRING = false;
const int DEFAULT_SIZER_FLAG = wxFIXED_MINSIZE | wxALIGN_LEFT | wxBOTTOM;

StaticText::StaticText(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, NOT_STRING) {
    sizer->Add(
        new wxStaticText(panel, wxID_ANY,
            wxString::FromUTF8(j["label"])),
            0, DEFAULT_SIZER_FLAG , 13);
}

FilePicker::FilePicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    wxString ext;
    if (j.contains("extension")) {
        ext = wxString::FromUTF8(j["extension"]);
    } else {
        ext = "any files (*)|*";
    }
    std::string value = j.value("default", "");
    std::string empty_message = wxString::FromUTF8(j.value("empty_message", ""));
    CustomFilePicker* picker = new CustomFilePicker(panel, wxID_ANY,
                                                    value, "", ext, empty_message,
                                                    wxDefaultPosition, wxSize(350, 25),
                                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    sizer->Add(
        new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"])),
        0, DEFAULT_SIZER_FLAG, 3);
    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    m_widget = picker;
}

wxString FilePicker::GetRawString() {
    return reinterpret_cast<CustomFilePicker*>(m_widget)->GetTextCtrlValue();
}

void FilePicker::SetConfig(nlohmann::json config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"]);
        reinterpret_cast<CustomFilePicker*>(m_widget)->SetPath(str);
        reinterpret_cast<CustomFilePicker*>(m_widget)->SetInitialDirectory(wxPathOnly(str));
    }
}

nlohmann::json FilePicker::GetConfig() {
    nlohmann::json config = {};
#ifdef _WIN32
    // utf-16 to utf-8 for Windows
    config["str"] = WStringToUTF8(std::wstring(GetRawString()));
#else
    config["str"] = GetRawString();
#endif
    return config;
}

// Dir Picker
DirPicker::DirPicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    std::string value = j.value("default", "");
    std::string empty_message = wxString::FromUTF8(j.value("empty_message", ""));
    CustomDirPicker* picker = new CustomDirPicker(panel, wxID_ANY,
                                                  value, "", empty_message,
                                                  wxDefaultPosition, wxSize(350, 25),
                                                  wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);
    sizer->Add(
        new wxStaticText(panel, wxID_ANY,
            wxString::FromUTF8(j["label"])),
            0, DEFAULT_SIZER_FLAG, 3);
    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    m_widget = picker;
}

wxString DirPicker::GetRawString() {
    return reinterpret_cast<CustomDirPicker*>(m_widget)->GetTextCtrlValue();
}

void DirPicker::SetConfig(nlohmann::json config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"]);
        reinterpret_cast<CustomDirPicker*>(m_widget)->SetPath(str);
        reinterpret_cast<CustomDirPicker*>(m_widget)->SetInitialDirectory(str);
    }
}

nlohmann::json DirPicker::GetConfig() {
    nlohmann::json config = {};
#ifdef _WIN32
    // utf-16 to utf-8 for Windows
    config["str"] = WStringToUTF8(std::wstring(GetRawString()));
#else
    config["str"] = GetRawString();
#endif
    return config;
}

// Choice
Choice::Choice(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["items"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(wxString::FromUTF8(i));
        });
    sizer->Add(
        new wxStaticText(panel, wxID_ANY,
            wxString::FromUTF8(j["label"])),
            0, DEFAULT_SIZER_FLAG, 3);
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxitems);
    sizer->Add(choice, 0, DEFAULT_SIZER_FLAG, 13);
    if (j.contains("default") && j["items"].size() > j["default"]) {
        choice->SetSelection(j["default"]);
    } else {
        choice->SetSelection(0);
    }
    if (j.contains("values") && j["values"].size() == j["items"].size()) {
        SetValues(j["values"]);
    } else {
        SetValues(j["items"]);
    }
    m_widget = choice;
}

wxString Choice::GetRawString() {
    int sel = reinterpret_cast<wxChoice*>(m_widget)->GetSelection();
    return wxString::FromUTF8(m_values[sel]);
}

void Choice::SetConfig(nlohmann::json config) {
    if (config.contains("int") && config["int"].is_number() && config["int"] < m_values.size()) {
        reinterpret_cast<wxChoice*>(m_widget)->SetSelection(config["int"]);
    }
}

nlohmann::json Choice::GetConfig() {
    nlohmann::json config = {};
    config["int"] = reinterpret_cast<wxChoice*>(m_widget)->GetSelection();
    return config;
}

void SetDefaultForCheckBox(wxCheckBox* check, nlohmann::json j) {
    if (j.is_boolean()) {
        check->SetValue(j);
    } else {
        check->SetValue(j["default"] != 0);
    }
}

// CheckBox
CheckBox::CheckBox(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY,
                                       wxString::FromUTF8(j["label"]));
    sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 13);
    m_value = j.value("value", j["label"]);
    if (j.contains("default")) {
        SetDefaultForCheckBox(check, j["default"]);
    }
    m_widget = check;
}

wxString CheckBox::GetRawString() {
    if (reinterpret_cast<wxCheckBox*>(m_widget)->GetValue()) {
        return wxString::FromUTF8(m_value);
    }
    return "";
}

void CheckBox::SetConfig(nlohmann::json config) {
    if (config.contains("int") && config["int"].is_number()) {
        reinterpret_cast<wxCheckBox*>(m_widget)->SetValue(config["int"] != 0);
    }
}

nlohmann::json CheckBox::GetConfig() {
    nlohmann::json config = {};
    config["int"] = static_cast<int>(reinterpret_cast<wxCheckBox*>(m_widget)->IsChecked());
    return config;
}

// CheckArray
CheckArray::CheckArray(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    sizer->Add(
        new wxStaticText(panel, wxID_ANY,
            wxString::FromUTF8(j["label"])),
            0, DEFAULT_SIZER_FLAG, 3);
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    wxCheckBox* check;
    for (int i = 0; i < j["items"].size(); i++) {
        check = new wxCheckBox(panel, wxID_ANY,
                           wxString::FromUTF8(j["items"][i]));
        checks->push_back(check);
        sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 3 + 10 * (i + 1 == j["items"].size()));
    }
    if (j.contains("values")) {
        SetValues(j["values"]);
    } else {
        SetValues(j["items"]);
    }
    if (j.contains("default")) {
        for (int i = 0; i < checks->size(); i++) {
            SetDefaultForCheckBox((*checks)[i], j["default"][i]);
        }
    }
    m_widget = checks;
}

wxString CheckArray::GetRawString() {
    wxString str = "";
    std::vector<wxCheckBox*> checks;
    checks = *(std::vector<wxCheckBox*>*)m_widget;
    for (int i = 0; i < checks.size(); i++) {
        if (checks[i]->GetValue()) {
            str += wxString::FromUTF8(m_values[i]);
        }
    }
    return str;
}

void CheckArray::SetConfig(nlohmann::json config) {
    std::vector<wxCheckBox*> checks;
    if (config.contains("ints") && config["ints"].is_array()) {
        checks = *(std::vector<wxCheckBox*>*)m_widget;
        for (int i = 0; i < config["ints"].size() && i < checks.size(); i++) {
            checks[i]->SetValue(config["ints"][i] != 0);
        }
    }
}

nlohmann::json CheckArray::GetConfig() {
    nlohmann::json config = {};
    std::vector<int> ints;
    for (wxCheckBox* check : *(std::vector<wxCheckBox*>*)m_widget) {
        ints.push_back(check->GetValue());
    }
    config["ints"] = ints;
    return config;
}

// TextBox
TextBox::TextBox(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    sizer->Add(
        new wxStaticText(panel, wxID_ANY,
            wxString::FromUTF8(j["label"])),
            0, DEFAULT_SIZER_FLAG, 3);
    std::string value = j.value("default", "");
    std::string emptyMessage = wxString::FromUTF8(j.value("empty_message", ""));
    CustomTextCtrl* textbox = new CustomTextCtrl(panel, wxID_ANY,
        value, emptyMessage,
        wxDefaultPosition, wxSize(350, 23));
    sizer->Add(textbox, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    m_widget = textbox;
}

wxString TextBox::GetRawString() {
    return reinterpret_cast<CustomTextCtrl*>(m_widget)->GetActualValue();
}

void TextBox::SetConfig(nlohmann::json config) {
    if (config.contains("str") && config["str"].is_string()) {
        reinterpret_cast<CustomTextCtrl*>(m_widget)->UpdateText(config["str"]);
    }
}

nlohmann::json TextBox::GetConfig() {
    nlohmann::json config = {};
    config["str"] = (std::string)GetRawString();
    return config;
}
