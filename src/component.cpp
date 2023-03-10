#include "component.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(nlohmann::json j, bool has_string) {
    m_widget = nullptr;
    m_has_string = has_string;
    m_label = wxString::FromUTF8(j["label"]);
    m_id = j.value("id", "");
    if (m_id == "") {
        size_t hash = std::hash<std::string>()(j["label"]);
        m_id = std::to_string(hash);
    }
    m_add_quotes = j.value("add_quotes", false);
}

wxString Component::GetString() {
    wxString str = GetRawString();
    if (m_add_quotes) {
        return "\"" + str + "\"";
    }
    return str;
}

nlohmann::json Component::GetConfig() {
    return {};
}

std::string const Component::GetID() {
    return m_id;
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
    } else if (type == "check_array") {  // checkArray
        comp = new CheckArray(panel, sizer, j);
    } else if (type == "text") {  // text box
        comp = new TextBox(panel, sizer, j);
    } else {
        std::cout << "Unknown component type detected. (" + type << ")" << std::endl;
    }
    return comp;
}

const bool HAS_STRING = true;
const bool NOT_STRING = false;
const int DEFAULT_SIZER_FLAG = wxFIXED_MINSIZE | wxALIGN_LEFT | wxBOTTOM;

// Static Text
StaticText::StaticText(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, NOT_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    text->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG , 13);
}

// Base Class for strings
StringComponentBase::StringComponentBase(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    if (j.contains("tooltip") && !j["tooltip"].is_array()) {
        text->SetToolTip(wxString::FromUTF8(j["tooltip"]));
    }
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG, 3);
}

nlohmann::json StringComponentBase::GetConfig() {
    nlohmann::json config = {};
    config["str"] = GetRawString().ToUTF8();
    return config;
}

// File Picker
FilePicker::FilePicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : StringComponentBase(panel, sizer, j) {
    wxString ext = wxString::FromUTF8(j.value("extension", "any files (*)|*"));
    wxString value = wxString::FromUTF8(j.value("default", ""));
    wxString empty_message = wxString::FromUTF8(j.value("empty_message", ""));
    CustomFilePicker* picker = new CustomFilePicker(panel, wxID_ANY,
                                                    value, "", ext, empty_message,
                                                    wxDefaultPosition, wxSize(350, 25),
                                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);

    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    picker->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
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

// Dir Picker
DirPicker::DirPicker(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : StringComponentBase(panel, sizer, j) {
    wxString value = wxString::FromUTF8(j.value("default", ""));
    wxString empty_message = wxString::FromUTF8(j.value("empty_message", ""));
    CustomDirPicker* picker = new CustomDirPicker(panel, wxID_ANY,
                                                  value, "", empty_message,
                                                  wxDefaultPosition, wxSize(350, 25),
                                                  wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);

    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    picker->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
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

// Choice
Choice::Choice(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : StringComponentBase(panel, sizer, j) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["item"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(wxString::FromUTF8(i));
        });
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxitems);
    sizer->Add(choice, 0, DEFAULT_SIZER_FLAG, 13);
    choice->SetSelection(j.value("default", 0) % items.size());

    if (j.contains("value") && j["value"].size() == j["item"].size()) {
        SetValues(j["value"]);
    } else {
        SetValues(j["item"]);
    }
    choice->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
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

// CheckBox
CheckBox::CheckBox(wxWindow* panel, wxBoxSizer* sizer, nlohmann::json j)
    : Component(j, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, m_label);
    sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 13);
    m_value = j.value("value", m_label.ToUTF8());
    check->SetValue(j.value("default", false));
    check->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
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
    : StringComponentBase(panel, sizer, j) {
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    for (int i = 0; i < j["item"].size(); i++) {
        wxCheckBox* check = new wxCheckBox(panel, wxID_ANY,
                           wxString::FromUTF8(j["item"][i]));
        if (j.contains("default")) {
            check->SetValue(j["default"][i]);
        }
        if (j.contains("tooltip")) {
            check->SetToolTip(wxString::FromUTF8(j["tooltip"][i]));
        }
        checks->push_back(check);
        sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 3 + 10 * (i + 1 == j["item"].size()));
    }
    if (j.contains("value")) {
        SetValues(j["value"]);
    } else {
        SetValues(j["item"]);
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
    : StringComponentBase(panel, sizer, j) {
    wxString value = wxString::FromUTF8(j.value("default", ""));
    wxString empty_message = wxString::FromUTF8(j.value("empty_message", ""));
    CustomTextCtrl* textbox = new CustomTextCtrl(panel, wxID_ANY,
        value, empty_message,
        wxDefaultPosition, wxSize(350, 23));

    sizer->Add(textbox, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    textbox->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
    m_widget = textbox;
}

wxString TextBox::GetRawString() {
    return reinterpret_cast<CustomTextCtrl*>(m_widget)->GetActualValue();
}

void TextBox::SetConfig(nlohmann::json config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"]);
        reinterpret_cast<CustomTextCtrl*>(m_widget)->UpdateText(str);
    }
}
