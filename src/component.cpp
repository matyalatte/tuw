#include "component.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(const nlohmann::json& j, bool has_string) {
    m_widget = nullptr;
    m_has_string = has_string;
    m_label = wxString::FromUTF8(j["label"].get<std::string>());
    m_id = j.value("id", "");
    if (m_id == "") {
        size_t hash = std::hash<std::string>()(j["label"].get<std::string>());
        m_id = "_" + std::to_string(hash);
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

Component* Component::PutComponent(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j) {
    Component* comp = nullptr;
    int type = j["type"].get<int>();
    switch (type) {
        case COMP_STATIC_TEXT:
            comp = new StaticText(panel, sizer, j);
            break;
        case COMP_FILE:
            comp = new FilePicker(panel, sizer, j);
            break;
        case COMP_FOLDER:
            comp = new DirPicker(panel, sizer, j);
            break;
        case COMP_CHOICE:
            comp = new Choice(panel, sizer, j);
            break;
        case COMP_CHECK:
            comp = new CheckBox(panel, sizer, j);
            break;
        case COMP_CHECK_ARRAY:
            comp = new CheckArray(panel, sizer, j);
            break;
        case COMP_TEXT:
            comp = new TextBox(panel, sizer, j);
            break;
        case COMP_INT:
            comp = new IntPicker(panel, sizer, j);
            break;
        case COMP_FLOAT:
            comp = new FloatPicker(panel, sizer, j);
            break;
        default:
            std::string msg = "Unknown component type detected. (" + std::to_string(type) + ")";
            throw std::runtime_error(msg);
            break;
    }
    return comp;
}

static const bool HAS_STRING = true;
static const bool NOT_STRING = false;
static const int DEFAULT_SIZER_FLAG = wxFIXED_MINSIZE | wxALIGN_LEFT | wxBOTTOM;

// Static Text
StaticText::StaticText(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : Component(j, NOT_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    text->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG , 13);
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : Component(j, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    if (j.contains("tooltip") && !j["tooltip"].is_array()) {
        text->SetToolTip(wxString::FromUTF8(j["tooltip"].get<std::string>()));
    }
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG, 3);
}

nlohmann::json StringComponentBase::GetConfig() {
    nlohmann::json config = {};
    config["str"] = GetRawString().ToUTF8();
    return config;
}

// File Picker
FilePicker::FilePicker(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
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
    return static_cast<CustomFilePicker*>(m_widget)->GetTextCtrlValue();
}

void FilePicker::SetConfig(const nlohmann::json& config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"].get<std::string>());
        static_cast<CustomFilePicker*>(m_widget)->SetPath(str);
        static_cast<CustomFilePicker*>(m_widget)->SetInitialDirectory(wxPathOnly(str));
    }
}

// Dir Picker
DirPicker::DirPicker(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
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
    return static_cast<CustomDirPicker*>(m_widget)->GetTextCtrlValue();
}

void DirPicker::SetConfig(const nlohmann::json& config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"].get<std::string>());
        static_cast<CustomDirPicker*>(m_widget)->SetPath(str);
        static_cast<CustomDirPicker*>(m_widget)->SetInitialDirectory(str);
    }
}

// Choice
Choice::Choice(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : StringComponentBase(panel, sizer, j) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["item"].get<std::vector<std::string>>();
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(wxString::FromUTF8(i));
        });
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxitems);
    sizer->Add(choice, 0, DEFAULT_SIZER_FLAG, 13);
    choice->SetSelection(j.value("default", 0) % items.size());

    if (j.contains("value") && j["value"].size() == j["item"].size()) {
        SetValues(j["value"].get<std::vector<std::string>>());
    } else {
        SetValues(j["item"].get<std::vector<std::string>>());
    }
    choice->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
    m_widget = choice;
}

wxString Choice::GetRawString() {
    int sel = static_cast<wxChoice*>(m_widget)->GetSelection();
    return wxString::FromUTF8(m_values[sel]);
}

void Choice::SetConfig(const nlohmann::json& config) {
    if (config.contains("int") && config["int"].is_number() && config["int"] < m_values.size()) {
        static_cast<wxChoice*>(m_widget)->SetSelection(config["int"].get<int>());
    }
}

nlohmann::json Choice::GetConfig() {
    nlohmann::json config = {};
    config["int"] = static_cast<wxChoice*>(m_widget)->GetSelection();
    return config;
}

// CheckBox
CheckBox::CheckBox(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : Component(j, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, m_label);
    sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 13);
    m_value = j.value("value", m_label.ToUTF8());
    check->SetValue(j.value("default", false));
    check->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));
    m_widget = check;
}

wxString CheckBox::GetRawString() {
    if (static_cast<wxCheckBox*>(m_widget)->GetValue()) {
        return wxString::FromUTF8(m_value);
    }
    return "";
}

void CheckBox::SetConfig(const nlohmann::json& config) {
    if (config.contains("int") && config["int"].is_number()) {
        static_cast<wxCheckBox*>(m_widget)->SetValue(config["int"] != 0);
    }
}

nlohmann::json CheckBox::GetConfig() {
    nlohmann::json config = {};
    config["int"] = static_cast<int>(static_cast<wxCheckBox*>(m_widget)->IsChecked());
    return config;
}

// CheckArray
CheckArray::CheckArray(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : StringComponentBase(panel, sizer, j) {
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    for (int i = 0; i < j["item"].size(); i++) {
        wxCheckBox* check = new wxCheckBox(panel, wxID_ANY,
                           wxString::FromUTF8(j["item"][i].get<std::string>()));
        if (j.contains("default")) {
            check->SetValue(j["default"][i].get<bool>());
        }
        if (j.contains("tooltip")) {
            check->SetToolTip(wxString::FromUTF8(j["tooltip"][i].get<std::string>()));
        }
        checks->push_back(check);
        sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 3 + 10 * (i + 1 == j["item"].size()));
    }
    if (j.contains("value")) {
        SetValues(j["value"].get<std::vector<std::string>>());
    } else {
        SetValues(j["item"].get<std::vector<std::string>>());
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

void CheckArray::SetConfig(const nlohmann::json& config) {
    if (config.contains("ints") && config["ints"].is_array()) {
        std::vector<wxCheckBox*> checks = *(std::vector<wxCheckBox*>*)m_widget;
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
TextBox::TextBox(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
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
    return static_cast<CustomTextCtrl*>(m_widget)->GetActualValue();
}

void TextBox::SetConfig(const nlohmann::json& config) {
    if (config.contains("str") && config["str"].is_string()) {
        wxString str = wxString::FromUTF8(config["str"].get<std::string>());
        static_cast<CustomTextCtrl*>(m_widget)->UpdateText(str);
    }
}

// IntPicker and FloatPicker
NumPickerBase::NumPickerBase(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : StringComponentBase(panel, sizer, j) {
    long style = wxSP_ARROW_KEYS;
    if (j.contains("wrap") && j["wrap"]) {
        style |= wxSP_WRAP;
    }

    m_picker = new wxSpinCtrlDouble(panel, wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxSize(150, 23), style);
    double min = j.value("min", 0.0);
    double max = j.value("max", 100.0);
    m_picker->SetRange(min, max);
    m_picker->SetToolTip(wxString::FromUTF8(j.value("tooltip", "")));

    sizer->Add(m_picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    m_widget = m_picker;
}

wxString NumPickerBase::GetRawString() {
    return m_picker->GetTextValue();
}

nlohmann::json NumPickerBase::GetConfig() {
    nlohmann::json config = {};
    config["float"] = m_picker->GetValue();
    return config;
}

void NumPickerBase::SetConfig(const nlohmann::json& config) {
    if (config.contains("float")
        && (config["float"].is_number_integer() || config["float"].is_number_float())) {
        double val = static_cast<double>(config["float"]);
        m_picker->SetValue(val);
    }
}

IntPicker::IntPicker(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : NumPickerBase(panel, sizer, j) {
    double inc = j.value("inc", 1);
    m_picker->SetIncrement(inc);
    m_picker->SetDigits(0);
    double val = j.value("default", 0.0);
    m_picker->SetValue(val);
}

FloatPicker::FloatPicker(wxWindow* panel, wxBoxSizer* sizer, const nlohmann::json& j)
    : NumPickerBase(panel, sizer, j) {
    double inc = j.value("inc", 0.1);
    m_picker->SetIncrement(inc);
    double digits = j.value("digits", 1);
    m_picker->SetDigits(digits);
    double val = j.value("default", 0.0);
    m_picker->SetValue(val);
}
