#pragma once
#include <nlohmann/json.hpp>
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/dnd.h>
#include <string>

#ifdef _WIN32
#include <codecvt> //char code converter for Windows system
#endif

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
	TYPE_CHECK,
	TYPE_CHECKS
};

//component for GUI
class Component {
private:
	void* widget;
	int type;
	int height;
	std::vector<std::string> values;
	std::string value;
	bool hasString;
	bool addQuotes;
	std::string label;
	wxString GetRawString();
	void SetLabel(std::string str);
	void SetAddQuotes(bool add);

	static Component* PutText(wxPanel* panel, nlohmann::json j, int y);
	static Component* PutFilePicker(wxPanel* panel, nlohmann::json j, int y);
	static Component* PutDirPicker(wxPanel* panel, nlohmann::json j, int y);
	static Component* PutChoice(wxPanel* panel, nlohmann::json j, int y);
	static Component* PutCheckBox(wxPanel* panel, nlohmann::json j, int y);
	static Component* PutCheckBoxes(wxPanel* panel, nlohmann::json j, int y);

public:
	Component(void* wid, int t);
	~Component();
	wxString GetString();
	std::string GetLabel();
	int GetInt();
	std::vector<int> GetInts();
	int GetHeight();
	int GetType();
	nlohmann::json GetConfig();
	void SetConfig(nlohmann::json config);

	void SetHeight(int h);
	void SetValues(std::vector<std::string> vals);
	void SetValue(std::string val);
	bool HasString();

	static Component* PutComponent(wxPanel* panel, nlohmann::json j, int y);
};

#ifdef _WIN32
std::string wstring_to_utf8(const std::wstring& str);
std::wstring utf8_to_wstring(const std::string& str);
#endif

bool hasKey(nlohmann::json json, std::string key);