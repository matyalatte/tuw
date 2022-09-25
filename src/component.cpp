#include "component.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(nlohmann::json j, int height, bool has_string) {
    m_widget = nullptr;
    m_height = height;
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

void Component::SetHeight(int h) {
    m_height = h;
}

int Component::GetHeight() {
    return m_height;
}

std::string Component::GetLabel() {
    return m_label;
}

bool Component::HasString() {
    return m_has_string;
}

Component* Component::PutComponent(wxPanel* panel, nlohmann::json j, int y) {
    Component* comp = nullptr;
    std::string type = j["type"];
    if (type == "static_text") {  // statc text
        comp = new StaticText(panel, j, y);
    } else if (type == "file") {  // file picker
        comp = new FilePicker(panel, j, y);
    } else if (type == "folder") {  // dir picker
        comp = new DirPicker(panel, j, y);
    } else if (type == "choice") {  // choice
        comp = new Choice(panel, j, y);
    } else if (type == "check") {  // checkbox
        comp = new CheckBox(panel, j, y);
    } else if (type == "checks" || type == "check_array") {  // checkArray
        comp = new CheckArray(panel, j, y);
    } else if (type == "text" || type == "text_box") {  // text box
        comp = new TextBox(panel, j, y);
    } else {
        std::cout << "[UpdatePanel] Unknown component type detected. (" << type << ")" << std::endl;
    }
    return comp;
}

const bool HAS_STRING = true;
const bool NOT_STRING = false;

StaticText::StaticText(wxPanel* panel, nlohmann::json j, int y): Component(j, 25, NOT_STRING) {
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
}

FilePicker::FilePicker(wxPanel* panel, nlohmann::json j, int y) : Component(j, 53, HAS_STRING) {
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    wxString ext;
    if (j.contains("extension")) {
        ext = wxString::FromUTF8(j["extension"]);
    } else {
        ext = "any files (*)|*";
    }
    std::string value = "";
    std::string emptyMessage = j.value("empty_message", "");
    CustomFilePicker* picker = new CustomFilePicker(panel, wxID_ANY,
                                                    value, "", ext, emptyMessage,
                                                    wxPoint(20, y + 18), wxSize(350, 25),
                                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
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
DirPicker::DirPicker(wxPanel* panel, nlohmann::json j, int y) : Component(j, 53, HAS_STRING) {
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    std::string value = "";
    std::string empty_message = j.value("empty_message", "");
    CustomDirPicker* picker = new CustomDirPicker(panel, wxID_ANY,
                                                  value, "", empty_message,
                                                  wxPoint(20, y + 18), wxSize(350, 25),
                                                  wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);
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
Choice::Choice(wxPanel* panel, nlohmann::json j, int y) : Component(j, 55, HAS_STRING) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["items"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(wxString::FromUTF8(i));
        });
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    int width = j.value("width", 95);

    wxChoice* choice = new wxChoice(panel, wxID_ANY,
        wxPoint(20, y + 20), wxSize(width, 30), wxitems);
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
CheckBox::CheckBox(wxPanel* panel, nlohmann::json j, int y) : Component(j, 35, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY,
                                       wxString::FromUTF8(j["label"]),
                                       wxPoint(20, y), wxSize(350, 25));
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
CheckArray::CheckArray(wxPanel* panel, nlohmann::json j, int y) :
    Component(j, 20 + j["items"].size() * 20 + 10, HAS_STRING) {
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    for (int i = 0; i < j["items"].size(); i++) {
        checks->push_back(
            new wxCheckBox(panel, wxID_ANY,
                           wxString::FromUTF8(j["items"][i]),
                           wxPoint(20, y + 20 + i * 20), wxSize(350, 15)));
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
TextBox::TextBox(wxPanel* panel, nlohmann::json j, int y) : Component(j, 53, HAS_STRING) {
    new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    std::string value = j.value("default", "");
    std::string emptyMessage = j.value("empty_message", "");
    CustomTextCtrl* textbox = new CustomTextCtrl(panel, wxID_ANY,
        value, emptyMessage,
        wxPoint(20, y + 20), wxSize(350, 23));
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
