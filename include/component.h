#pragma once
#include "json.h"
#include "ui.h"
#include "string_utils.h"
#include "noex/vector.hpp"
#include "validator.h"

#define UNUSED(x) (void)(x)

// Base class for GUI components (file picker, combo box, etc.)
class Component {
 protected:
    void* m_widget;
    const char* m_label;
    const char* m_id;
    bool m_has_string;
    bool m_is_wide;
    Validator m_validator;
    uiLabel* m_error_widget;
    bool m_optional;
    const char* m_prefix;
    const char* m_suffix;

 private:
    bool m_add_quotes;

 public:
    explicit Component(const tuwjson::Value& j) noexcept;
    virtual ~Component() noexcept {}
    virtual noex::string GetRawString() noexcept { return "";}
    noex::string GetString() noexcept;
    const char* GetID() const noexcept { return m_id; }

    virtual void SetConfig(const tuwjson::Value& config) noexcept { UNUSED(config); }
    virtual void GetConfig(tuwjson::Value& config) noexcept { UNUSED(config); }

    bool HasString() const noexcept { return m_has_string; }
    bool IsWide() const noexcept { return m_is_wide; }

    bool Validate(bool* redraw_flag) noexcept;
    const noex::string& GetValidationError() const noexcept;
    void PutErrorWidget(uiBox* box) noexcept;

    static Component* PutComponent(uiBox* box, const tuwjson::Value& j) noexcept;
};

class EmptyComponent : public Component {
 public:
    EmptyComponent(uiBox* box, const tuwjson::Value& j) noexcept
        : Component(j) { UNUSED(box); }
};

class StaticText : public Component {
 public:
    StaticText(uiBox* box, const tuwjson::Value& j) noexcept;
};

class StringComponentBase : public Component {
 public:
    StringComponentBase(uiBox* box, const tuwjson::Value& j) noexcept;
    void GetConfig(tuwjson::Value& config) noexcept override;
};

class FilterList {
 private:
    noex::string filter_buf_str;
    noex::vector<const char*> patterns;
    noex::vector<uiFileDialogParamsFilter> ui_filters;

 public:
    FilterList() noexcept: filter_buf_str(), patterns(), ui_filters() {}

    void MakeFilters(const char* ext) noexcept;

    size_t GetSize() const noexcept {
        return ui_filters.size();
    }

    uiFileDialogParamsFilter* ToLibuiFilterList() const noexcept {
        return ui_filters.data();
    }
};

class FilePicker : public StringComponentBase {
 private:
    const char* m_ext;

 public:
    noex::string GetRawString() noexcept override;
    FilePicker(uiBox* box, const tuwjson::Value& j) noexcept;
    void SetConfig(const tuwjson::Value& config) noexcept override;
    void OpenFile() noexcept;
};

class DirPicker : public StringComponentBase {
 public:
    noex::string GetRawString() noexcept override;
    DirPicker(uiBox* box, const tuwjson::Value& j) noexcept;
    void SetConfig(const tuwjson::Value& config) noexcept override;
    void OpenFolder() noexcept;
};

class ComboBox : public StringComponentBase {
 private:
    noex::vector<const char*> m_values;

 public:
    noex::string GetRawString() noexcept override;
    ComboBox(uiBox* box, const tuwjson::Value& j) noexcept;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

class RadioButtons : public StringComponentBase {
 private:
    noex::vector<const char*> m_values;

 public:
    noex::string GetRawString() noexcept override;
    RadioButtons(uiBox* box, const tuwjson::Value& j) noexcept;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

class CheckBox : public Component {
 private:
    const char* m_value;

 public:
    noex::string GetRawString() noexcept override;
    CheckBox(uiBox* box, const tuwjson::Value& j) noexcept;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

class CheckArray : public StringComponentBase {
 private:
    noex::vector<const char*> m_values;
    noex::vector<uiCheckbox*> m_checks;

 public:
    noex::string GetRawString() noexcept override;
    CheckArray(uiBox* box, const tuwjson::Value& j) noexcept;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

class TextBox : public StringComponentBase {
 public:
    noex::string GetRawString() noexcept override;
    TextBox(uiBox* box, const tuwjson::Value& j) noexcept;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

class IntPicker : public StringComponentBase {
 public:
    IntPicker(uiBox* box, const tuwjson::Value& j) noexcept;
    noex::string GetRawString() noexcept override;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};

// This is the same as IntPicker cause uiSpinboxDouble is not supported yet.
class FloatPicker : public StringComponentBase {
 public:
    FloatPicker(uiBox* box, const tuwjson::Value& j) noexcept;
    noex::string GetRawString() noexcept override;
    void GetConfig(tuwjson::Value& config) noexcept override;
    void SetConfig(const tuwjson::Value& config) noexcept override;
};
