#pragma once
#include <nlohmann/json.hpp>
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/dnd.h>
#include <string>

//Drop target for path picker
template <typename T>
class DropFilePath : public wxFileDropTarget
{
private:
	T* frame;

public:

	DropFilePath(T* frame);
	~DropFilePath();
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override;
};

enum comp_type {
	TYPE_TEXT,
	TYPE_FILE,
	TYPE_FOLDER,
	TYPE_CHOICE,
	TYPE_CHECK
};

class Component {
private:
	void* widget;
	int type;
	int height;
	std::vector<std::string> values;
	std::string value;
	bool hasString;

	static int PutText(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
	static int PutFilePicker(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
	static int PutDirPicker(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
	static int PutChoice(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
	static int PutCheckBox(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
public:
	Component(void* wid, int t);
	~Component();
	wxString GetString();
	int GetHeight();
	void SetHeight(int h);
	void SetValues(std::vector<std::string> vals);
	void SetValue(std::string val);
	bool HasString();

	static int PutComponent(std::vector<Component>* components, wxPanel* panel, nlohmann::json j, int y);
};

bool hasKey(nlohmann::json json, std::string key);