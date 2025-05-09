#include "component.h"
#include "json_utils.h"
#include "env_utils.h"
#include "string_utils.h"
#include "tuw_constants.h"

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(const tuwjson::Value& j) noexcept {
    m_widget = nullptr;
    m_has_string = false;
    m_is_wide = false;
    m_label = j["label"].GetString();
    m_id = json_utils::GetString(j, "id", "");
    m_add_quotes = json_utils::GetBool(j, "add_quotes", false);
    tuwjson::Value* ptr = j.GetMemberPtr("validator");
    if (ptr)
        m_validator.Initialize(*ptr);
    m_optional = json_utils::GetBool(j, "optional", false);
    m_prefix = json_utils::GetString(j, "prefix", "");
    m_suffix = json_utils::GetString(j, "suffix", "");
}

noex::string Component::GetString() noexcept {
    noex::string str = GetRawString();
    if (m_optional && str.empty())
        return "";
    if (m_add_quotes)
        str = noex::concat_cstr("\"", str.c_str(), "\"");
    return noex::concat_cstr(m_prefix, str.c_str(), m_suffix);
}

bool Component::Validate(bool* redraw_flag) noexcept {
    // Main frame should run Fit() after this function.
    noex::string str = GetRawString();
    if (m_optional && str.empty())
        return true;

    bool validate = m_validator.Validate(str);
    uiControl *c = uiControl(m_error_widget);
    bool old_validate = !static_cast<bool>(uiControlVisible(c));
    bool updated = old_validate != validate;
    *redraw_flag |= updated;

    if (updated == validate) {
        uiControlHide(c);
    }
    if (!validate) {
        uiControlShow(c);
        uiLabelSetText(m_error_widget,
                       GetValidationError().c_str());
    }
    return validate;
}

const noex::string& Component::GetValidationError() const noexcept {
    return m_validator.GetError();
}

void Component::PutErrorWidget(uiBox* box) noexcept {
    m_error_widget = uiNewLabel("");
    uiLabelSetTextColor(m_error_widget, 1.0, 0.0, 0.0);
    uiControlHide(uiControl(m_error_widget));
    uiBoxAppend(box, uiControl(m_error_widget), 0);
}

template <typename CompT>
Component* NewComp(uiBox* box, const tuwjson::Value& j) noexcept {
    CompT* comp = reinterpret_cast<CompT*>(calloc(1, sizeof(CompT)));
    if (comp) {
        new (comp) CompT(box, j);
    } else {
        noex::set_error_no(noex::EXTERNAL_ALLOCATION_ERROR);
    }
    return comp;
}

using NewCompFunc = Component*(*)(uiBox*, const tuwjson::Value&);

Component* Component::PutComponent(uiBox* box, const tuwjson::Value& j) noexcept {
    // Note: We need EmptyComponent for components which should be ignored.
    static const NewCompFunc new_funcs[COMP_MAX - COMP_EMPTY] = {
        &NewComp<EmptyComponent>,
        &NewComp<StaticText>,
        &NewComp<FilePicker>,
        &NewComp<DirPicker>,
        &NewComp<ComboBox>,
        &NewComp<RadioButtons>,
        &NewComp<CheckBox>,
        &NewComp<CheckArray>,
        &NewComp<TextBox>,
        &NewComp<IntPicker>,
        &NewComp<FloatPicker>
    };

    int type = j["type_int"].GetInt();
    if (type < COMP_EMPTY || COMP_MAX <= type)
        return nullptr;

    Component* comp = nullptr;
    comp = new_funcs[type - COMP_EMPTY](box, j);
    if (comp)
        comp->PutErrorWidget(box);
    return comp;
}

// Static Text
StaticText::StaticText(uiBox* box, const tuwjson::Value& j) noexcept
    : Component(j) {
    uiLabel* text = uiNewLabel(m_label);
    uiBoxAppend(box, uiControl(text), 0);
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    uiBox* box, const tuwjson::Value& j) noexcept
    : Component(j) {
    m_has_string = false;
    uiLabel* text = uiNewLabel(m_label);
    uiBoxAppend(box, uiControl(text), 0);
}

void StringComponentBase::GetConfig(tuwjson::Value& config) noexcept {
    config[m_id].SetString(GetRawString());
}

static uiWindow* GetToplevel(uiControl* c) noexcept {
    if (uiControlToplevel(c)) return uiWindow(c);
    return GetToplevel(uiControlParent(c));
}

static void onOpenFileClicked(uiButton *b, void *data) noexcept {
    FilePicker* picker = static_cast<FilePicker*>(data);
    picker->OpenFile();
    UNUSED(b);
}

static void onFilesDropped(uiEntry *e, int count, char** names, void *data) noexcept {
    if (count < 1) return;
    uiEntrySetText(e, names[0]);
    UNUSED(data);
}

static void SetTooltip(uiControl* c, const tuwjson::Value& j) {
    tuwjson::Value* ptr = j.GetMemberPtr("tooltip");
    if (ptr)
        uiControlSetTooltip(c, ptr->GetString());
}

static uiEntry *putPathPicker(void* component, uiBox* box, const tuwjson::Value& j,
                              void (*click_func)(uiButton *sender, void *senderData)) noexcept {
    const char* value = json_utils::GetString(j, "default", "");
    const char* placeholder = json_utils::GetString(j, "placeholder", "");
    const char* button_label = json_utils::GetString(j, "button", "Browse");

    uiEntry* entry = uiNewEntry();
    uiEntryOnFilesDropped(entry, onFilesDropped, NULL);
    uiEntrySetAcceptDrops(entry, 1);
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, placeholder);

    uiButton* button = uiNewButton(button_label);
    uiButtonOnClicked(button, click_func, component);

    uiGrid* grid = uiNewGrid();
    uiGridAppend(grid, uiControl(entry),
        0, 0, 1, 1,
        1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(button),
        1, 0, 1, 1,
        0, uiAlignFill, 0, uiAlignFill);
    uiGridSetSpacing(grid, tuw_constants::GRID_COMP_XSPACE, 0);

    uiBoxAppend(box, uiControl(grid), 0);
    SetTooltip(uiControl(entry), j);
    return entry;
}

// File Picker
FilePicker::FilePicker(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    m_is_wide = true;
    m_ext = json_utils::GetString(j, "extension", "any files (*.*)|*.*");
    m_use_save_dialog = json_utils::GetBool(j, "use_save_dialog", false);
    m_widget = putPathPicker(this, box, j, onOpenFileClicked);
}

noex::string FilePicker::GetRawString() noexcept {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    noex::string str = text;
    uiFreeText(text);
    return str;
}

static void setConfigForTextBox(const tuwjson::Value& config,
                                const char* id, void *widget) noexcept {
    const char* str = json_utils::GetString(config, id, nullptr);
    if (str) {
        uiEntry* entry = static_cast<uiEntry*>(widget);
        uiEntrySetText(entry, str);
    }
}

void FilePicker::SetConfig(const tuwjson::Value& config) noexcept {
    setConfigForTextBox(config, m_id, m_widget);
}

void FilterList::MakeFilters(const char* ext) noexcept {
    filter_buf_str = ext;
    char* filter_buf = filter_buf_str.data();
    if (!filter_buf) return;

    bool is_reading_pattern = false;
    const char* name = nullptr;
    size_t first_pattern_id = 0;

    const char* p = ext;
    char* buf = filter_buf;
    const char* next_str = filter_buf;
    while (*p) {
        char c = *p;
        if (c == '|') {
            *buf = 0;
            if (is_reading_pattern) {
                patterns.emplace_back(next_str);
                ui_filters.push_back({
                    name,
                    patterns.size() - first_pattern_id,
                    nullptr  // We set pointers later since they can be reallocated.
                });
                first_pattern_id = patterns.size();
            } else {
                name = next_str;
            }
            is_reading_pattern = !is_reading_pattern;
            next_str = buf + 1;
        } else if (is_reading_pattern && (c == ';')) {
            *buf = 0;
            patterns.emplace_back(next_str);
            next_str = buf + 1;
        } else {
            // *buf = c;
        }
        buf++;
        p++;
    }
    *buf = 0;
    if (is_reading_pattern) {
        patterns.emplace_back(next_str);
        ui_filters.push_back({
            name,
            patterns.size() - first_pattern_id,
            nullptr
        });
    }

    // Set string pointers here since patterns.emplace_back() can reallocate them.
    const char** ptr = patterns.data();
    for (uiFileDialogParamsFilter& filter : ui_filters) {
        size_t count = filter.patternCount;
        filter.patterns = ptr;
        ptr += count;
    }
}

void FilePicker::OpenFile() noexcept {
    uiEntry *entry = static_cast<uiEntry*>(m_widget);
    char *filename;

    uiFileDialogParams params;
    params.defaultPath = NULL;
    params.defaultName = NULL;

    FilterList filter_list = FilterList();
    filter_list.MakeFilters(m_ext);

    params.filterCount = filter_list.GetSize();
    params.filters = filter_list.ToLibuiFilterList();

    uiWindow* top = GetToplevel(uiControl(entry));
    if (m_use_save_dialog)
        filename = uiSaveFileWithParams(top, &params);
    else
        filename = uiOpenFileWithParams(top, &params);
    if (filename == NULL)
        return;

    uiEntrySetText(entry, filename);
    uiFreeText(filename);
}

static void onOpenFolderClicked(uiButton *b, void *data) noexcept {
    DirPicker* picker = static_cast<DirPicker*>(data);
    picker->OpenFolder();
    UNUSED(b);
}

// Dir Picker
DirPicker::DirPicker(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    m_is_wide = true;
    m_widget = putPathPicker(this, box, j, onOpenFolderClicked);
}

noex::string DirPicker::GetRawString() noexcept {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    noex::string str = text;
    uiFreeText(text);
    return str;
}

void DirPicker::SetConfig(const tuwjson::Value& config) noexcept {
    setConfigForTextBox(config, m_id, m_widget);
}

void DirPicker::OpenFolder() noexcept {
    uiEntry *entry = uiEntry(m_widget);
    char *filename;

    uiFileDialogParams params;
    params.defaultPath = NULL;
    params.defaultName = NULL;
    params.filterCount = 0;
    params.filters = NULL;

    filename = uiOpenFolderWithParams(GetToplevel(uiControl(entry)), &params);
    if (filename == NULL) {
        return;
    }

    uiEntrySetText(entry, filename);
    uiFreeText(filename);
}

// ComboBox
ComboBox::ComboBox(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j), m_values() {
    uiCombobox* combo = uiNewCombobox();
    for (const tuwjson::Value& i : j["items"]) {
        const char* label = i["label"].GetString();
        uiComboboxAppend(combo, label);
        const char* value = json_utils::GetString(i, "value", label);
        m_values.push_back(value);
    }
    uiBoxAppend(box, uiControl(combo), 0);
    uiComboboxSetSelected(combo, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetTooltip(uiControl(combo), j);
    m_widget = combo;
}

noex::string ComboBox::GetRawString() noexcept {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    return m_values[sel];
}

void ComboBox::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsInt()) {
        int i = ptr->GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiComboboxSetSelected(static_cast<uiCombobox*>(m_widget), i);
    }
}

void ComboBox::GetConfig(tuwjson::Value& config) noexcept {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    config[m_id].SetInt(sel);
}

// RadioButtons
RadioButtons::RadioButtons(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j), m_values() {
    uiRadioButtons* radio = uiNewRadioButtons();
    for (const tuwjson::Value& i : j["items"]) {
        const char* label = i["label"].GetString();
        uiRadioButtonsAppend(radio, label);
        const char* value = json_utils::GetString(i, "value", label);
        m_values.push_back(value);
    }
    uiBoxAppend(box, uiControl(radio), 0);
    uiRadioButtonsSetSelected(radio, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetTooltip(uiControl(radio), j);
    m_widget = radio;
}

noex::string RadioButtons::GetRawString() noexcept {
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    return m_values[sel];
}

void RadioButtons::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsInt()) {
        int i = ptr->GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiRadioButtonsSetSelected(static_cast<uiRadioButtons*>(m_widget), i);
    }
}

void RadioButtons::GetConfig(tuwjson::Value& config) noexcept {
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    config[m_id].SetInt(sel);
}

// CheckBox
CheckBox::CheckBox(uiBox* box, const tuwjson::Value& j) noexcept
    : Component(j) {
    m_has_string = true;
    uiCheckbox* check = uiNewCheckbox(m_label);
    uiCheckboxSetChecked(check, json_utils::GetBool(j, "default", false));
    uiBoxAppend(box, uiControl(check), 0);

    m_value = json_utils::GetString(j, "value", m_label);

    SetTooltip(uiControl(check), j);
    m_widget = check;
}

noex::string CheckBox::GetRawString() noexcept {
    if (uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget)))
        return m_value;
    return "";
}

void CheckBox::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsBool())
        uiCheckboxSetChecked(static_cast<uiCheckbox*>(m_widget), ptr->GetBool());
}

void CheckBox::GetConfig(tuwjson::Value& config) noexcept {
    bool checked = uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget));
    config[m_id].SetBool(checked);
}

// CheckArray
CheckArray::CheckArray(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    uiBox* check_array_box = uiNewVerticalBox();
    uiBoxSetSpacing(check_array_box, tuw_constants::BOX_CHECKS_SPACE);
    size_t id = 0;
    for (const tuwjson::Value& i : j["items"]) {
        const char* label = i["label"].GetString();
        uiCheckbox* check = uiNewCheckbox(label);
        uiCheckboxSetChecked(check, json_utils::GetBool(i, "default", false));
        uiBoxAppend(check_array_box, uiControl(check), 0);
        SetTooltip(uiControl(check), i);
        m_checks.push_back(check);
        const char* value = json_utils::GetString(i, "value", label);
        m_values.push_back(value);
        id++;
    }
    uiBoxAppend(box, uiControl(check_array_box), 0);
}

noex::string CheckArray::GetRawString() noexcept {
    noex::string str;
    for (size_t i = 0; i < m_checks.size(); i++) {
        if (uiCheckboxChecked(m_checks[i])) {
            str += m_values[i];
        }
    }
    return str;
}

void CheckArray::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsArray()) {
        for (size_t i = 0; i < ptr->Size() && i < m_checks.size(); i++) {
            tuwjson::Value& v = ptr->At(i);
            if (v.IsBool())
                uiCheckboxSetChecked(m_checks[i], v.GetBool());
        }
    }
}

void CheckArray::GetConfig(tuwjson::Value& config) noexcept {
    tuwjson::Value ints;
    ints.SetArray();
    for (uiCheckbox* check : m_checks) {
        tuwjson::Value v;
        v.SetBool(static_cast<bool>(uiCheckboxChecked(check)));
        ints.MoveAndPush(v);
    }
    config[m_id].MoveFrom(ints);
}

// TextBox
TextBox::TextBox(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    m_is_wide = true;
    const char* value = json_utils::GetString(j, "default", "");
    const char* placeholder = json_utils::GetString(j, "placeholder", "");
    uiEntry* entry = uiNewEntry();
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, placeholder);
    uiBoxAppend(box, uiControl(entry), 0);
    SetTooltip(uiControl(entry), j);
    m_widget = entry;
}

noex::string TextBox::GetRawString() noexcept {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    noex::string str = text;
    uiFreeText(text);
    return str;
}

void TextBox::SetConfig(const tuwjson::Value& config) noexcept {
    setConfigForTextBox(config, m_id, m_widget);
}

static void initSpinbox(uiSpinbox* picker, uiBox* box, const tuwjson::Value& j) noexcept {
    uiBoxAppend(box, uiControl(picker), 0);
    SetTooltip(uiControl(picker), j);
}

IntPicker::IntPicker(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    int min = json_utils::GetInt(j, "min", 0);
    int max = json_utils::GetInt(j, "max", 100);
    if (min > max) {
        int x = min;
        min = max;
        max = x;
    }
    int inc = json_utils::GetInt(j, "inc", 1);
    int val = json_utils::GetInt(j, "default", min);
    bool wrap = json_utils::GetBool(j, "wrap", false);
    uiSpinbox* picker = uiNewSpinboxDoubleEx(
        static_cast<double>(min),
        static_cast<double>(max),
        0,
        static_cast<double>(inc),
        static_cast<int>(wrap));
    uiSpinboxSetValue(picker, val);
    initSpinbox(picker, box, j);
    m_widget = picker;
}

noex::string IntPicker::GetRawString() noexcept {
    char* text = uiSpinboxValueText(static_cast<uiSpinbox*>(m_widget));
    noex::string str(text);
    uiFreeText(text);
    return str;
}

void IntPicker::GetConfig(tuwjson::Value& config) noexcept {
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    config[m_id].SetInt(val);
}

void IntPicker::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsInt()) {
        int val = ptr->GetInt();
        uiSpinboxSetValue(static_cast<uiSpinbox*>(m_widget), val);
    }
}

FloatPicker::FloatPicker(uiBox* box, const tuwjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    double min = json_utils::GetDouble(j, "min", 0.0);
    double max = json_utils::GetDouble(j, "max", 100.0);
    if (min > max) {
        double x = min;
        min = max;
        max = x;
    }
    double inc = json_utils::GetDouble(j, "inc", 0.1);
    int digits = json_utils::GetInt(j, "digits", 1);
    double val = json_utils::GetDouble(j, "default", min);
    bool wrap = json_utils::GetBool(j, "wrap", false);
    uiSpinbox* picker = uiNewSpinboxDoubleEx(min, max, digits, inc, static_cast<int>(wrap));
    uiSpinboxSetValueDouble(picker, val);
    initSpinbox(picker, box, j);
    m_widget = picker;
}

noex::string FloatPicker::GetRawString() noexcept {
    char* text = uiSpinboxValueText(static_cast<uiSpinbox*>(m_widget));
    noex::string str(text);
    uiFreeText(text);
    return str;
}

void FloatPicker::GetConfig(tuwjson::Value& config) noexcept {
    double val = uiSpinboxValueDouble(static_cast<uiSpinbox*>(m_widget));
    config[m_id].SetDouble(val);
}

void FloatPicker::SetConfig(const tuwjson::Value& config) noexcept {
    tuwjson::Value* ptr = config.GetMemberPtr(m_id);
    if (ptr && ptr->IsDouble()) {
        double val = ptr->GetDouble();
        uiSpinboxSetValueDouble(static_cast<uiSpinbox*>(m_widget), val);
    }
}
