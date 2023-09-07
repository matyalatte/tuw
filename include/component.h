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
 public:
    std::string GetRawString() override;
    FilePicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
};

class DirPicker : public StringComponentBase {
 public:
    std::string GetRawString() override;
    DirPicker(uiBox* box, const rapidjson::Value& j);
    void SetConfig(const rapidjson::Value& config) override;
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
 private:
    bool m_wrap;
    int m_inc;
    int m_min;
    int m_max;
    int m_old_val;
 public:
    IntPicker(uiBox* box, const rapidjson::Value& j);
    std::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
    bool GetWrap() { return m_wrap; }
    int GetInc() { return m_inc; }
    int GetMin() { return m_min; }
    int GetMax() { return m_max; }
    int GetOldVal() { return m_old_val; }
    void SetOldVal(int val) { m_old_val = val; }
};

// This is the same as IntPicker cause uiSpinboxDouble is not supported yet.
class FloatPicker : public StringComponentBase {
 private:
    bool m_wrap;
    double m_inc;
    double m_min;
    double m_max;
    double m_old_val;
    int m_digits;
 public:
    FloatPicker(uiBox* box, const rapidjson::Value& j);
    std::string GetRawString() override;
    void GetConfig(rapidjson::Document& config) override;
    void SetConfig(const rapidjson::Value& config) override;
    bool GetWrap() { return m_wrap; }
    double GetInc() { return m_inc; }
    double GetMin() { return m_min; }
    double GetMax() { return m_max; }
    double GetOldVal() { return m_old_val; }
    void SetOldVal(double val) { m_old_val = val; }
    int GetDigits() { return m_digits; }
};

