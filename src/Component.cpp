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
    std::cout << filenames[0].c_str();
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

wxString Component::GetString() {
	wxString str = "";
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
	default:
        str = "";
	}
	return str;
}
void Component::SetHeight(int h) {
    height = h;
}
int Component::GetHeight() {
    return height;
}

bool Component::HasString() {
    return hasString;
}

int Component::PutText(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    Component* comp = new Component(nullptr, comp_type::TYPE_TEXT);
    components->push_back(*comp);
    comp->SetHeight(25);
    return comp->GetHeight();
}

int Component::PutFilePicker(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    wxFilePickerCtrl* picker = new wxFilePickerCtrl(panel, wxID_ANY, "", "", j["extension"], wxPoint(20, y + 15), wxSize(350, 25), wxFLP_DEFAULT_STYLE | wxFLP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxFilePickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    Component* comp = new Component(picker, comp_type::TYPE_FILE);
    components->push_back(*comp);
    comp->SetHeight(50);
    return comp->GetHeight();
}

int Component::PutDirPicker(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    wxDirPickerCtrl* picker = new wxDirPickerCtrl(panel, wxID_ANY, "", "", wxPoint(20, y + 15), wxSize(350, 25), wxDIRP_DEFAULT_STYLE | wxDIRP_USE_TEXTCTRL);
    picker->GetTextCtrl()->SetDropTarget(new DropFilePath<wxDirPickerCtrl>(picker));
    picker->DragAcceptFiles(true);
    Component* comp = new Component(picker, comp_type::TYPE_FOLDER);
    components->push_back(*comp);
    comp->SetHeight(50);
    return comp->GetHeight();
}
int Component::PutChoice(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    wxArrayString wxitems;
    std::vector<std::string> items = j["items"];
    std::for_each(items.begin(), items.end(), [&](std::string i) {
        wxitems.Add(i);
        });
    wxStaticText* text = new wxStaticText(panel, wxID_ANY, j["label"], wxPoint(20, y));
    wxChoice* choice = new wxChoice(panel, wxID_ANY, wxPoint(20, y + 20), wxSize(95, 30), wxitems);
    choice->SetSelection(j["default"]);
    Component* comp = new Component(choice, comp_type::TYPE_CHOICE);
    if (hasKey(j, "values")) {
        comp->SetValues(j["values"]);
    }
    else {
        comp->SetValues(j["items"]);
    }
    comp->SetHeight(60);
    components->push_back(*comp);
    return comp->GetHeight();
}

int Component::PutCheckBox(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    wxCheckBox* check = new wxCheckBox(panel, wxID_ANY, "label", wxPoint(20, y), wxSize(350, 25));
    Component* comp = new Component(check, comp_type::TYPE_CHECK);
    if (hasKey(j, "value")) {
        comp->SetValue(j["value"]);
    }
    else {
        comp->SetValue(j["label"]);
    }
    components->push_back(*comp);
    comp->SetHeight(35);
    return comp->GetHeight();
}

int Component::PutComponent(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y) {
    int height = 0;
    if (j["type"] == "text") {//text
        height = Component::PutText(components, panel, j, y);
    }
    else if (j["type"] == "file") {//file picker
        height = Component::PutFilePicker(components, panel, j, y);
    }
    else if (j["type"] == "folder") {//dir picker
        height = Component::PutDirPicker(components, panel, j, y);
    }
    else if (j["type"] == "choice") {//choice
        height = Component::PutChoice(components, panel, j, y);
    }
    else if (j["type"] == "check") {//choice
        height = Component::PutCheckBox(components, panel, j, y);
    }
    else {
        std::cout << "[UpdatePanel] unknown component type detected. (" << j["type"] << ")" << std::endl;
    }
    return height;
}

bool hasKey(nlohmann::json json, std::string key) {
    auto subjectIdIter = json.find(key);
    return subjectIdIter != json.end();
}