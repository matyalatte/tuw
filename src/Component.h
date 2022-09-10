#pragma once
#include <nlohmann/json.hpp>
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/dnd.h>
#include <string>
#include "JsonUtils.h"

#ifdef _WIN32
#include <codecvt> //char code converter for Windows system
#endif

//TextCtrl that can show a message when its text box is empty.
class FocusedTextCtrl : public wxTextCtrl {
private:
	wxString emptyMessage;
	wxString actualValue;
public:
	FocusedTextCtrl(wxWindow* parent, wxWindowID id,
		const wxString& value = wxEmptyString,
		const wxString& empty_message = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxTextCtrlNameStr);
	virtual ~FocusedTextCtrl() {}

	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void SetEmptyMessage();
	DECLARE_EVENT_TABLE();
};

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

//component for GUI
class Component {
protected:
	void* widget;
	std::vector<std::string> values;
	std::string value;


private:
	int height;
	bool hasString;
	bool addQuotes;
	std::string label;

public:
	Component(nlohmann::json j, int height, bool hasString);
	~Component();
	virtual wxString GetRawString();
	wxString GetString();
	std::string GetLabel();
	int GetHeight();
	virtual nlohmann::json GetConfig();
	virtual void SetConfig(nlohmann::json config);

	void SetHeight(int h);
	void SetValues(std::vector<std::string> vals);
	bool HasString();

	static Component* PutComponent(wxPanel* panel, nlohmann::json j, int y);
};

#ifdef _WIN32
std::string wstring_to_utf8(const std::wstring& str);
std::wstring utf8_to_wstring(const std::string& str);
#endif

class StaticText : public Component {
public:
	StaticText(wxPanel* panel, nlohmann::json j, int y);
};

class FilePicker : public Component {
public:
	wxString GetRawString() override;
	FilePicker(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};

class DirPicker : public Component {
public:
	wxString GetRawString() override;
	DirPicker(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};

class Choice : public Component {
public:
	wxString GetRawString() override;
	Choice(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};

class CheckBox : public Component {
public:
	wxString GetRawString() override;
	CheckBox(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};

class CheckArray : public Component {
public:
	wxString GetRawString() override;
	CheckArray(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};

class TextBox : public Component {
public:
	wxString GetRawString() override;
	TextBox(wxPanel* panel, nlohmann::json j, int y);
	nlohmann::json GetConfig() override;
	void SetConfig(nlohmann::json config) override;
};