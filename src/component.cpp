#include "component.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(const rapidjson::Value& j, bool has_string) {
    m_widget = nullptr;
    m_has_string = has_string;
    m_label = wxString::FromUTF8(j["label"].GetString());
    m_id = json_utils::GetString(j, "id", "");
    if (m_id == "") {
        size_t hash = std::hash<std::string>()(j["label"].GetString());
        m_id = "_" + std::to_string(hash);
    }
    m_add_quotes = json_utils::GetBool(j, "add_quotes", false);
}

wxString Component::GetString() {
    wxString str = GetRawString();
    if (m_add_quotes) {
        return "\"" + str + "\"";
    }
    return str;
}

std::string const Component::GetID() {
    return m_id;
}

Component* Component::PutComponent(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j) {
    Component* comp = nullptr;
    int type = j["type_int"].GetInt();
    switch (type) {
        case COMP_EMPTY:
            comp = new EmptyComponent(panel, sizer, j);
            break;
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
StaticText::StaticText(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : Component(j, NOT_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    text->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG , 13);
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : Component(j, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, m_label);
    if (j.HasMember("tooltip") && !j["tooltip"].IsArray()) {
        text->SetToolTip(wxString::FromUTF8(j["tooltip"].GetString()));
    }
    sizer->Add(text, 0, DEFAULT_SIZER_FLAG, 3);
}

void StringComponentBase::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    rapidjson::Value val(GetRawString().ToUTF8(), config.GetAllocator());
    config.AddMember(n, val, config.GetAllocator());
}

// File Picker
FilePicker::FilePicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    wxString ext = wxString::FromUTF8(json_utils::GetString(j, "extension", "any files (*)|*").c_str());
    wxString value = wxString::FromUTF8(json_utils::GetString(j, "default", "").c_str());
    wxString empty_message = wxString::FromUTF8(json_utils::GetString(j, "empty_message", "").c_str());
    CustomFilePicker* picker = new CustomFilePicker(panel, wxID_ANY,
                                                    value, "", ext, empty_message,
                                                    wxDefaultPosition, wxSize(350, 25),
                                                    wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);

    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    picker->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    m_widget = picker;
}

wxString FilePicker::GetRawString() {
    return static_cast<CustomFilePicker*>(m_widget)->GetTextCtrlValue();
}

void FilePicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        wxString str = wxString::FromUTF8(config[m_id].GetString());
        static_cast<CustomFilePicker*>(m_widget)->SetPath(str);
        static_cast<CustomFilePicker*>(m_widget)->SetInitialDirectory(wxPathOnly(str));
    }
}

// Dir Picker
DirPicker::DirPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    wxString value = wxString::FromUTF8(json_utils::GetString(j, "default", "").c_str());
    wxString empty_message = wxString::FromUTF8(json_utils::GetString(j, "empty_message", "").c_str());
    CustomDirPicker* picker = new CustomDirPicker(panel, wxID_ANY,
                                                  value, "", empty_message,
                                                  wxDefaultPosition, wxSize(350, 25),
                                                  wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);

    sizer->Add(picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    picker->DragAcceptFiles(true);
    picker->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    m_widget = picker;
}

wxString DirPicker::GetRawString() {
    return static_cast<CustomDirPicker*>(m_widget)->GetTextCtrlValue();
}

void DirPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        wxString str = wxString::FromUTF8(config[m_id].GetString());
        static_cast<CustomDirPicker*>(m_widget)->SetPath(str);
        static_cast<CustomDirPicker*>(m_widget)->SetInitialDirectory(str);
    }
}

// Choice
Choice::Choice(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    wxArrayString wxitems;
    wxArrayString values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        wxString label = wxString::FromUTF8(i["label"].GetString());
        wxitems.Add(label);
        wxString value;
        if (i.HasMember("value"))
            value = wxString::FromUTF8(i["value"].GetString());
        else
            value = label;
        values.Add(value);
    }
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxitems);
    sizer->Add(choice, 0, DEFAULT_SIZER_FLAG, 13);
    choice->SetSelection(json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    choice->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    m_widget = choice;
}

wxString Choice::GetRawString() {
    int sel = static_cast<wxChoice*>(m_widget)->GetSelection();
    return m_values[sel];
}

void Choice::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int  i = config[m_id].GetInt();
        if (i < m_values.size())
            static_cast<wxChoice*>(m_widget)->SetSelection(i);
    }
}

void Choice::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    int sel = static_cast<wxChoice*>(m_widget)->GetSelection();
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, sel, config.GetAllocator());
}

// CheckBox
CheckBox::CheckBox(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : Component(j, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, m_label);
    sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 13);
    if (j.HasMember("value"))
        m_value = wxString::FromUTF8(j["value"].GetString());
    else
        m_value = m_label;
    check->SetValue(json_utils::GetBool(j, "default", false));
    check->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    m_widget = check;
}

wxString CheckBox::GetRawString() {
    if (static_cast<wxCheckBox*>(m_widget)->IsChecked())
        return m_value;
    return "";
}

void CheckBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsBool())
        static_cast<wxCheckBox*>(m_widget)->SetValue(config[m_id].GetBool());
}

void CheckBox::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    bool checked = static_cast<wxCheckBox*>(m_widget)->IsChecked();
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, checked, config.GetAllocator());
}

// CheckArray
CheckArray::CheckArray(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    wxArrayString values;
    size_t id = 0;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        wxString label = wxString::FromUTF8(i["label"].GetString());
        wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, label);
        if (i.HasMember("default"))
            check->SetValue(i["default"].GetBool());
        if (i.HasMember("tooltip"))
            check->SetToolTip(wxString::FromUTF8(i["tooltip"].GetString()));
        checks->push_back(check);
        sizer->Add(check, 0, DEFAULT_SIZER_FLAG, 3 + 10 * (id + 1 == j["items"].Size()));
        wxString value;
        if (i.HasMember("value"))
            value = wxString::FromUTF8(i["value"].GetString());
        else
            value = label;
        values.Add(value);
        id++;
    }
    SetValues(values);
    m_widget = checks;
}

wxString CheckArray::GetRawString() {
    wxString str = "";
    std::vector<wxCheckBox*> checks;
    checks = *(std::vector<wxCheckBox*>*)m_widget;
    for (int i = 0; i < checks.size(); i++) {
        if (checks[i]->GetValue()) {
            str += m_values[i];
        }
    }
    return str;
}

void CheckArray::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsArray()) {
        std::vector<wxCheckBox*> checks = *(std::vector<wxCheckBox*>*)m_widget;
        for (int i = 0; i < config[m_id].Size() && i < checks.size(); i++) {
            checks[i]->SetValue(config[m_id][i].GetBool());
        }
    }
}

void CheckArray::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);

    rapidjson::Value ints;
    ints.SetArray();
    for (wxCheckBox* check : *(std::vector<wxCheckBox*>*)m_widget) {
        ints.PushBack(check->GetValue(), config.GetAllocator());
    }
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, ints, config.GetAllocator());
}

// TextBox
TextBox::TextBox(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    wxString value = wxString::FromUTF8(json_utils::GetString(j, "default", "").c_str());
    wxString empty_message = wxString::FromUTF8(json_utils::GetString(j, "empty_message", "").c_str());
    CustomTextCtrl* textbox = new CustomTextCtrl(panel, wxID_ANY,
        value, empty_message,
        wxDefaultPosition, wxSize(350, 23));

    sizer->Add(textbox, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    textbox->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));
    m_widget = textbox;
}

wxString TextBox::GetRawString() {
    return static_cast<CustomTextCtrl*>(m_widget)->GetActualValue();
}

void TextBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        wxString str = wxString::FromUTF8(config[m_id].GetString());
        static_cast<CustomTextCtrl*>(m_widget)->UpdateText(str);
    }
}

// IntPicker and FloatPicker
NumPickerBase::NumPickerBase(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : StringComponentBase(panel, sizer, j) {
    long style = wxSP_ARROW_KEYS;
    if (j.HasMember("wrap") && j["wrap"].GetBool())
        style |= wxSP_WRAP;

    m_picker = new wxSpinCtrlDouble(panel, wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxSize(150, 23), style);
    double min = json_utils::GetDouble(j, "min", 0.0);
    double max = json_utils::GetDouble(j, "max", 100.0);
    m_picker->SetRange(min, max);
    m_picker->SetToolTip(wxString::FromUTF8(json_utils::GetString(j, "tooltip", "").c_str()));

    sizer->Add(m_picker, 0, wxALIGN_LEFT | wxBOTTOM, 13);
    m_widget = m_picker;
}

wxString NumPickerBase::GetRawString() {
    return m_picker->GetTextValue();
}

void NumPickerBase::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, m_picker->GetValue(), config.GetAllocator());
}

void NumPickerBase::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && (config[m_id].IsInt() || config[m_id].IsDouble())) {
        double val = config[m_id].GetDouble();
        m_picker->SetValue(val);
    }
}

IntPicker::IntPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : NumPickerBase(panel, sizer, j) {
    double inc = json_utils::GetDouble(j, "inc", 1);
    m_picker->SetIncrement(inc);
    m_picker->SetDigits(0);
    double val = json_utils::GetDouble(j, "default", 0.0);
    m_picker->SetValue(val);
}

FloatPicker::FloatPicker(wxWindow* panel, wxBoxSizer* sizer, const rapidjson::Value& j)
    : NumPickerBase(panel, sizer, j) {
    double inc = json_utils::GetDouble(j, "inc", 0.1);
    m_picker->SetIncrement(inc);
    int digits = json_utils::GetInt(j, "digits", 1);
    m_picker->SetDigits(digits);
    double val = json_utils::GetDouble(j, "default", 0.0);
    m_picker->SetValue(val);
}
