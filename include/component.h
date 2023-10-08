#pragma once
#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "ui.h"

// Base class for GUI components (file picker, combo box, etc.)
class Component {
 protected:
    void* m_widget;
    std::string m_label;
    std::string m_id;
    bool m_has_string;
    bool m_is_wide;
    uintptr_t m_tooltip;  // handler for tooltip window

 private:
    bool m_add_quotes;

 public:
    explicit Component(const rapidjson::Value& j);
    ~Component();
    virtual std::string GetRawString() { return "";}
    std::string GetString();
    std::string const GetID();

    virtual void SetConfig(const rapidjson::Value& config) {}
    virtual void GetConfig(rapidjson::Document& config) {}

    bool HasString() { return m_has_string; }
    bool IsWide() { return m_is_wide; }

    static Component* PutComponent(uiBox* box, const rapidjson::Value& j);
};

// containers for Choice and CheckArray
class MultipleValuesContainer {
 protected:
    std::vector<std::string> m_values;

 public:
    void SetValues(std::vector<std::string> values){
        m_values = values;
    }
};

class EmptyComponent : public Component {
 public:
    EmptyComponent(uiBox* box, const rapidjson::Value& j)
        : Component(j) {}
};

class StaticText : public Component {
 public:
    StaticText(uiBox* box, const rapidjson::Value& j);
};

class StringComponentBase : public Component {
 public:
    StringComponentBase(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
};

class FilePicker : public StringComponentBase {
 private:
    std::string m_ext;
 public:
    std::string GetRawString() override;
    FilePicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
    void OpenFile();
};

class DirPicker : public StringComponentBase {
 public:
    std::string GetRawString() override;
    DirPicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
    void OpenFolder();
};

class Choice : public StringComponentBase, MultipleValuesContainer {
 public:
    std::string GetRawString() override;
    Choice(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckBox : public Component {
 private:
    std::string m_value;
 public:
    std::string GetRawString() override;
    CheckBox(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class CheckArray : public StringComponentBase, MultipleValuesContainer {
 public:
    std::string GetRawString() override;
    CheckArray(uiBox* box, const rapidjson::Value& j);
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

class TextBox : public StringComponentBase {
 public:
    std::string GetRawString() override;
    TextBox(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class IntPicker : public StringComponentBase {
 public:
    IntPicker(uiBox* box, const rapidjson::Value& j);
    std::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

// This is the same as IntPicker cause uiSpinboxDouble is not supported yet.
class FloatPicker : public StringComponentBase {
 public:
    FloatPicker(uiBox* box, const rapidjson::Value& j);
    std::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
};

