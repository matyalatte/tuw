#include "Component.h"

//Drop target for path picker
template <typename T>
DropFilePath<T>::DropFilePath(T* frame) : wxFileDropTarget() {
    this->frame = frame;
}

template <typename T>
DropFilePath<T>::~DropFilePath() {
}

template <typename T>
bool DropFilePath<T>::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) {
    this->frame->SetPath(filenames[0]);
    return 1;
}

//Component for GUI
Component::Component(nlohmann::json j, int height, bool hasString) {
    widget = nullptr;
    this->height = height;
    this->hasString = hasString;
    label = j["label"];
    addQuotes = false;
    if (jsonUtils::hasKey(j, "add_quotes")) {
        addQuotes= j["add_quotes"];
    }
}

Component::~Component() {
};

void Component::SetValues(std::vector<std::string> vals) {
	values = vals;
}

wxString Component::GetRawString() {
    return "";
}

wxString Quote(wxString str) {
    return "\"" + str + "\"";
}

wxString Component::GetString() {
    wxString str = GetRawString();
    if (addQuotes){
        return Quote(str);
    }
	return str;
}

#ifdef _WIN32
//utf-16 to utf-8 for Windows
std::string wstring_to_utf8(const std::wstring& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}
std::wstring utf8_to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}
#endif

nlohmann::json Component::GetConfig() {
    return {};
}

void Component::SetConfig(nlohmann::json config) {
}

void Component::SetHeight(int h) {
    height = h;
}
int Component::GetHeight() {
    return height;
}

std::string Component::GetLabel() {
    return label;
}

bool Component::HasString() {
    return hasString;
}

Component* Component::PutComponent(wxPanel* panel, nlohmann::json j, int y) {
    Component* comp=nullptr;
    if (j["type"] == "text") {//text
        comp = new Text(panel, j, y);
    }
    else if (j["type"] == "file") {//file picker
        comp = new FilePicker(panel, j, y);
    }
    else if (j["type"] == "folder") {//dir picker
        comp = new DirPicker(panel, j, y);
    }
    else if (j["type"] == "choice") {//choice
        comp = new Choice(panel, j, y);
    }
    else if (j["type"] == "check") {//checkbox
        comp = new CheckBox(panel, j, y);
    }
    else if (j["type"] == "checks") {//checkboxes
        comp = new CheckBoxes(panel, j, y);
    }
    else {
        std::cout << "[UpdatePanel] unknown component type detected. (" << j["type"] << ")" << std::endl;
    }
    return comp;
}

const bool HAS_STRING = true;
const bool NOT_STRING = false;

Text::Text(wxPanel* panel, nlohmann::json j, int y): Component(j, 25, NOT_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
}

//File Picker
FilePicker::FilePicker(wxPanel* panel, nlohmann::json j, int y) : Component(j, 50, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    wxString ext;
    if (jsonUtils::hasKey(j, "extension")) {
        ext = wxString::FromUTF8(j["extension"]);
    }
    else {
        ext = "any files | *";
    }
    wxFilePickerCtrl* picker = new wxFilePickerCtrl(panel, wxID_ANY, "", "", ext, wxPoint(20, y + 15), wxSize(350, 25), wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxFilePickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    widget = picker;
}

wxString FilePicker::GetRawString(){
    return ((wxFilePickerCtrl*)widget)->GetPath();
}

void FilePicker::SetConfig(nlohmann::json config){
    if (jsonUtils::hasKey(config, "str") && config["str"].is_string()) {
        ((wxFilePickerCtrl*)widget)->SetPath(wxString::FromUTF8(config["str"]));
        ((wxFilePickerCtrl*)widget)->SetInitialDirectory(wxPathOnly(wxString::FromUTF8(config["str"])));
    }
}

nlohmann::json FilePicker::GetConfig() {
    nlohmann::json config = {};
#ifdef _WIN32
    //utf-16 to utf-8 for Windows
    config["str"] = wstring_to_utf8(std::wstring(GetRawString()));
#else
    config["str"] = GetRawString();
#endif
    return config;
}

//Dir Picker
DirPicker::DirPicker(wxPanel* panel, nlohmann::json j, int y) : Component(j, 50, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    wxDirPickerCtrl* picker = new wxDirPickerCtrl(panel, wxID_ANY, "", "", wxPoint(20, y + 15), wxSize(350, 25), wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxDirPickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    widget = picker;
}

wxString DirPicker::GetRawString() {
    return ((wxDirPickerCtrl*)widget)->GetPath();
}

void DirPicker::SetConfig(nlohmann::json config) {
    if (jsonUtils::hasKey(config, "str") && config["str"].is_string()) {
        ((wxDirPickerCtrl*)widget)->SetPath(wxString::FromUTF8(config["str"]));
        ((wxDirPickerCtrl*)widget)->SetInitialDirectory(wxString::FromUTF8(config["str"]));
    }
}

nlohmann::json DirPicker::GetConfig() {
    nlohmann::json config = {};
#ifdef _WIN32
    //utf-16 to utf-8 for Windows
    config["str"] = wstring_to_utf8(std::wstring(GetRawString()));
#else
    config["str"] = GetRawString();
#endif
    return config;
}

//Choice
Choice::Choice(wxPanel* panel, nlohmann::json j, int y) : Component(j, 55, HAS_STRING) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["items"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(wxString::FromUTF8(i));
        });
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    int width = 95;
    if (jsonUtils::hasKey(j, "width")) {
        width = j["width"];
    }
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(20, y + 20), wxSize(width, 30), wxitems);
    if (jsonUtils::hasKey(j, "default") && j["items"].size() > j["default"]) {
        choice->SetSelection(j["default"]);
    }
    else {
        choice->SetSelection(0);
    }
    if (jsonUtils::hasKey(j, "values") && j["values"].size() == j["items"].size()) {
        SetValues(j["values"]);
    }
    else {
        SetValues(j["items"]);
    }
    widget = choice;
}

wxString Choice::GetRawString() {
    int sel = ((wxChoice*)widget)->GetSelection();
    return wxString::FromUTF8(values[sel]);
}

void Choice::SetConfig(nlohmann::json config) {
    if (jsonUtils::hasKey(config, "int") && config["int"].is_number() && config["int"] < values.size()) {
        ((wxChoice*)widget)->SetSelection(config["int"]);
    }
}

nlohmann::json Choice::GetConfig() {
    nlohmann::json config = {};
    config["int"] = ((wxChoice*)widget)->GetSelection();
    return config;
}

//CheckBox
CheckBox::CheckBox(wxPanel* panel, nlohmann::json j, int y) : Component(j, 35, HAS_STRING) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y), wxSize(350, 25));
    if (jsonUtils::hasKey(j, "value")) {
        value = j["value"];
    }
    else {
        value = j["label"];
    }
    widget = check;
}

wxString CheckBox::GetRawString() {
    if (((wxCheckBox*)widget)->GetValue()) {
        return wxString::FromUTF8(value);
    }
    return "";
}

void CheckBox::SetConfig(nlohmann::json config) {
    if (jsonUtils::hasKey(config, "int") && config["int"].is_number()) {
        ((wxCheckBox*)widget)->SetValue(config["int"] != 0);
    }
}

nlohmann::json CheckBox::GetConfig() {
    nlohmann::json config = {};
    config["int"] = (int)((wxCheckBox*)widget)->IsChecked();
    return config;
}

//CheckBoxes
CheckBoxes::CheckBoxes(wxPanel* panel, nlohmann::json j, int y) : Component(j, 20 + j["items"].size() * 20 + 10, HAS_STRING) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, wxString::FromUTF8(j["label"]), wxPoint(20, y));
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    wxCheckBox* check;
    for (int i = 0; i < j["items"].size(); i++) {
        check = new wxCheckBox(panel, wxID_ANY, wxString::FromUTF8(j["items"][i]), wxPoint(20, y + 20 + i * 20), wxSize(350, 15));
        checks->push_back(check);
    }
    if (jsonUtils::hasKey(j, "values")) {
        SetValues(j["values"]);
    }
    else {
        SetValues(j["items"]);
    }
    widget = checks;
}

wxString CheckBoxes::GetRawString() {
    wxString str = "";
    std::vector<wxCheckBox*> checks;
    checks = *(std::vector<wxCheckBox*>*)widget;
    for (int i = 0; i < checks.size(); i++) {
        if (checks[i]->GetValue()) {
            str += wxString::FromUTF8(values[i]);
        }
    }
    return str;
}

void CheckBoxes::SetConfig(nlohmann::json config) {
    std::vector<wxCheckBox*> checks;
    if (jsonUtils::hasKey(config, "ints") && config["int"].is_array()) {
        checks = *(std::vector<wxCheckBox*>*)widget;
        for (int i = 0; i < config["ints"].size() && i < checks.size(); i++) {
            checks[i]->SetValue(config["ints"][i] != 0);
        }
    }
}

nlohmann::json CheckBoxes::GetConfig() {
    nlohmann::json config = {};
    std::vector<int> ints;
    for (wxCheckBox* check : *(std::vector<wxCheckBox*>*)widget) {
        ints.push_back(check->GetValue());
    }
    config["ints"] = ints;
    return config;
}