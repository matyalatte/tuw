#pragma once
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
Component::Component(void* wid, int t) {
	widget = wid;
	type = t;
    height = 0;
    switch (t){
    case comp_type::TYPE_TEXT:
        hasString = false;
        break;
    default:
        hasString = true;
    }
}

Component::~Component() {
	//delete this->widget;
};

void Component::SetValues(std::vector<std::string> vals) {
	values = vals;
}

void Component::SetValue(std::string val) {
    value = val;
}

int Component::GetInt() {
    int i=0;
    switch (type) {
    case comp_type::TYPE_FILE:
    case comp_type::TYPE_FOLDER:
        i = 0;
        break;
    case comp_type::TYPE_CHOICE:
        i = ((wxChoice*)widget)->GetSelection();
        break;
    case comp_type::TYPE_CHECK:
        i = (int)((wxCheckBox*)widget)->IsChecked();
        break;
    default:
        i = 0;
    }
    return i;
}

std::vector<int> Component::GetInts() {
    std::vector<int> ints;
    switch (type) {
    case comp_type::TYPE_FILE:
    case comp_type::TYPE_FOLDER:
        break;
    case comp_type::TYPE_CHOICE:
        ints.push_back(((wxChoice*)widget)->GetSelection());
        break;
    case comp_type::TYPE_CHECK:
        ints.push_back((int)((wxCheckBox*)widget)->IsChecked());
        break;
    case comp_type::TYPE_CHECKS:
        for (wxCheckBox* check: *(std::vector<wxCheckBox*>*)widget) {
            ints.push_back(check->GetValue());
        }
        break;        
    default:
        break;
    }
    return ints;
}

wxString Component::GetString() {
	wxString str = "";
    std::vector<wxCheckBox*> checks;
	int sel;
	switch (type) {
	case comp_type::TYPE_FILE:
		str = ((wxFilePickerCtrl*)widget)->GetPath();
		break;
	case comp_type::TYPE_FOLDER:
		str = ((wxDirPickerCtrl*)widget)->GetPath();
		break;
	case comp_type::TYPE_CHOICE:
		sel = ((wxChoice*)widget)->GetSelection();
		str = values[sel];
		break;
    case comp_type::TYPE_CHECK:
        if (((wxCheckBox*)widget)->GetValue()) {
            str = value;
        }
        break;
    case comp_type::TYPE_CHECKS:
        str = "";
        checks = *(std::vector<wxCheckBox*>*)widget;
        for (int i=0; i<checks.size();i++){
            if (checks[i]->GetValue()) {
                str += values[i];
            }
        }
        break;
	default:
        str = "";
	}
	return str;
}

nlohmann::json Component::GetConfig() {
    nlohmann::json config = {};
    switch (type) {
    case comp_type::TYPE_FILE:
    case comp_type::TYPE_FOLDER:
        config["str"] = GetString();
        break;
    case comp_type::TYPE_CHOICE:
    case comp_type::TYPE_CHECK:
        config["int"] = GetInt();
        break;
    case comp_type::TYPE_CHECKS:
        config["ints"] = GetInts();
        break;
    default:
        break;
    }
    return config;
}

void Component::SetConfig(nlohmann::json config) {
    std::vector<wxCheckBox*> checks;
    switch (type) {
    case comp_type::TYPE_FILE:
        if (hasKey(config, "str")) {
            ((wxFilePickerCtrl*)widget)->SetPath(config["str"]);
            ((wxFilePickerCtrl*)widget)->SetInitialDirectory(wxPathOnly(config["str"]));
        }
        break;
    case comp_type::TYPE_FOLDER:
        if (hasKey(config, "str")) {
            ((wxDirPickerCtrl*)widget)->SetPath(config["str"]);
            ((wxDirPickerCtrl*)widget)->SetInitialDirectory(config["str"]);
        }
        break;
    case comp_type::TYPE_CHOICE:
        if (hasKey(config, "int") && config["int"] < values.size()) {
            ((wxChoice*)widget)->SetSelection(config["int"]);
        }
        break;
    case comp_type::TYPE_CHECK:
        if (hasKey(config, "int")) {
            ((wxCheckBox*)widget)->SetValue(config["int"]!=0);
        }
        break;
    case comp_type::TYPE_CHECKS:
        if (hasKey(config, "ints")) {
            checks = *(std::vector<wxCheckBox*>*)widget;
            for (int i = 0; i < config["ints"].size() && i<checks.size(); i++) {
                checks[i]->SetValue(config["ints"][i] != 0);
            }
        }
        break;
    default:
        break;
    }
}

void Component::SetHeight(int h) {
    height = h;
}
int Component::GetHeight() {
    return height;
}
int Component::GetType() {
    return type;
}

void Component::SetLabel(std::string str) {
    label = str;
}

std::string Component::GetLabel() {
    return label;
}

bool Component::HasString() {
    return hasString;
}

Component* Component::PutText(wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    Component* comp = new Component(nullptr, comp_type::TYPE_TEXT);
    comp->SetHeight(25);
    return comp;
}

Component* Component::PutFilePicker(wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    std::string ext;
    if (hasKey(j, "extension")) {
        ext = j["extension"];
    }
    else {
        ext = "any files | *";
    }
    wxFilePickerCtrl* picker = new wxFilePickerCtrl(panel, wxID_ANY, "", "", j["extension"], wxPoint(20, y + 15), wxSize(350, 25), wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxFilePickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    Component* comp = new Component(picker, comp_type::TYPE_FILE);
    comp->SetHeight(50);
    return comp;
}

Component* Component::PutDirPicker(wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    wxDirPickerCtrl* picker = new wxDirPickerCtrl(panel, wxID_ANY, "", "", wxPoint(20, y + 15), wxSize(350, 25), wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxDirPickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    Component* comp = new Component(picker, comp_type::TYPE_FOLDER);
    comp->SetHeight(50);
    return comp;
}
Component* Component::PutChoice(wxPanel* panel, nlohmann::json j, int y) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["items"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(i);
        });
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    int width = 95;
    if (hasKey(j, "width")) {
        width = j["width"];
    }
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(20, y + 20), wxSize(width, 30), wxitems);
    if (hasKey(j, "default")) {
        choice->SetSelection(j["default"]);
    }
    Component* comp = new Component(choice, comp_type::TYPE_CHOICE);
    if (hasKey(j, "values") && j["values"].size() == j["items"].size()) {
        comp->SetValues(j["values"]);
    }
    else {
        comp->SetValues(j["items"]);
    }
    comp->SetHeight(55);
    return comp;
}

Component* Component::PutCheckBox(wxPanel* panel, nlohmann::json j, int y) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, j["label"], wxPoint(20, y), wxSize(350, 25));
    Component* comp = new Component(check, comp_type::TYPE_CHECK);
    if (hasKey(j, "value")) {
        comp->SetValue(j["value"]);
    }
    else {
        comp->SetValue(j["label"]);
    }
    //components->push_back(*comp);
    comp->SetHeight(35);
    return comp;
}

Component* Component::PutCheckBoxes(wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    std::vector<wxCheckBox*>* checks = new std::vector<wxCheckBox*>();
    wxCheckBox* check;
    for (int i = 0; i < j["items"].size(); i++) {
        check = new wxCheckBox(panel, wxID_ANY, j["items"][i], wxPoint(20, y + 20 + i * 20), wxSize(350, 15));
        checks->push_back(check);
    }
    Component* comp = new Component(checks, comp_type::TYPE_CHECKS);
    if (hasKey(j, "values")) {
        comp->SetValues(j["values"]);
    }
    else {
        comp->SetValues(j["items"]);
    }
    //components->push_back(*comp);
    comp->SetHeight(20 + j["items"].size() * 20 + 10);
    return comp;
}


Component* Component::PutComponent(wxPanel* panel, nlohmann::json j, int y) {
    Component* comp=nullptr;
    if (j["type"] == "text") {//text
        comp = Component::PutText(panel, j, y);
    }
    else if (j["type"] == "file") {//file picker
        comp = Component::PutFilePicker(panel, j, y);
    }
    else if (j["type"] == "folder") {//dir picker
        comp = Component::PutDirPicker(panel, j, y);
    }
    else if (j["type"] == "choice") {//choice
        comp = Component::PutChoice(panel, j, y);
    }
    else if (j["type"] == "check") {//checkbox
        comp = Component::PutCheckBox(panel, j, y);
    }
    else if (j["type"] == "checks") {//checkboxes
        comp = Component::PutCheckBoxes(panel, j, y);
    }
    else {
        std::cout << "[UpdatePanel] unknown component type detected. (" << j["type"] << ")" << std::endl;
    }
    comp->SetLabel(j["label"]);
    return comp;
}

bool hasKey(nlohmann::json json, std::string key) {
    auto subjectIdIter = json.find(key);
    return subjectIdIter != json.end();
}