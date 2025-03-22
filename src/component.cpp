#include "component.h"
#include "json_utils.h"
#include "env_utils.h"
#include "string_utils.h"
#include "tuw_constants.h"

enum ComponentType: int {
    COMP_UNKNOWN = 0,
    COMP_EMPTY,
    COMP_STATIC_TEXT,
    COMP_FILE,
    COMP_FOLDER,
    COMP_COMBO,
    COMP_RADIO,
    COMP_CHECK,
    COMP_CHECK_ARRAY,
    COMP_TEXT,
    COMP_INT,
    COMP_FLOAT,
    COMP_MAX
};

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(const rapidjson::Value& j) noexcept {
    m_widget = nullptr;
    m_has_string = false;
    m_is_wide = false;
    m_label = j["label"].GetString();
    m_id = json_utils::GetString(j, "id", "");
    if (m_id.empty()) {
        uint32_t hash = Fnv1Hash32(m_label);
        m_id = noex::string("_") + hash;
    }
    m_add_quotes = json_utils::GetBool(j, "add_quotes", false);
    if (j.HasMember("validator"))
        m_validator.Initialize(j["validator"]);
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
    return noex::concat_cstr(m_prefix.c_str(), str.c_str(), m_suffix.c_str());
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

    if (updated) {
       if (validate) {
            uiControlHide(c);
        } else {
            uiControlShow(c);
            uiLabelSetText(m_error_widget,
                           GetValidationError().c_str());
        }
    } else if (!validate) {
        uiControlHide(c);
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
Component* NewComp(uiBox* box, const rapidjson::Value& j) noexcept {
    CompT* comp = reinterpret_cast<CompT*>(calloc(1, sizeof(CompT)));
    if (comp) {
        new (comp) CompT(box, j);
    } else {
        noex::set_error_no(noex::EXTERNAL_ALLOCATION_ERROR);
    }
    return comp;
}

using NewCompFunc = Component*(*)(uiBox*, const rapidjson::Value&);

Component* Component::PutComponent(uiBox* box, const rapidjson::Value& j) noexcept {
    static const NewCompFunc new_funcs[COMP_MAX - COMP_STATIC_TEXT] = {
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

    Component* comp = nullptr;
    int type = j["type_int"].GetInt();
    if (COMP_STATIC_TEXT <= type || type < COMP_MAX)
        comp = new_funcs[type - COMP_STATIC_TEXT](box, j);
    if (comp)
        comp->PutErrorWidget(box);
    return comp;
}

// Static Text
StaticText::StaticText(uiBox* box, const rapidjson::Value& j) noexcept
    : Component(j) {
    uiLabel* text = uiNewLabel(m_label.c_str());
    uiBoxAppend(box, uiControl(text), 0);
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    uiBox* box, const rapidjson::Value& j) noexcept
    : Component(j) {
    m_has_string = false;
    uiLabel* text = uiNewLabel(m_label.c_str());
    uiBoxAppend(box, uiControl(text), 0);
}

static rapidjson::Value
CreateConfigValue(rapidjson::Document& config, const noex::string& id) noexcept {
    if (config.HasMember(id.c_str()))
        config.RemoveMember(id.c_str());
    rapidjson::Value n(id.c_str(), config.GetAllocator());
    return n;
}

void StringComponentBase::GetConfig(rapidjson::Document& config) noexcept {
    rapidjson::Value val(GetRawString().c_str(), config.GetAllocator());
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, val, config.GetAllocator());
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

static void SetTooltip(uiControl* c, const rapidjson::Value& j) {
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(c, json_utils::GetString(j, "tooltip", ""));
}

static uiEntry *putPathPicker(void* component, uiBox* box, const rapidjson::Value& j,
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
FilePicker::FilePicker(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    m_is_wide = true;
    m_ext = json_utils::GetString(j, "extension", "any files (*.*)|*.*");
    m_widget = putPathPicker(this, box, j, onOpenFileClicked);
}

noex::string FilePicker::GetRawString() noexcept {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    noex::string str = text;
    uiFreeText(text);
    return str;
}

static void setConfigForTextBox(const rapidjson::Value& config,
                                const noex::string& id, void *widget) noexcept {
    const char* str = json_utils::GetString(config, id.c_str(), nullptr);
    if (str) {
        uiEntry* entry = static_cast<uiEntry*>(widget);
        uiEntrySetText(entry, str);
    }
}

void FilePicker::SetConfig(const rapidjson::Value& config) noexcept {
    setConfigForTextBox(config, m_id, m_widget);
}

class Filter {
 private:
    const char* name;
    noex::vector<const char*> patterns;

 public:
    Filter() noexcept : name(nullptr), patterns() {}

    Filter(const Filter& filter) :
        name(filter.name), patterns(filter.patterns) {}

    Filter& operator=(const Filter& filter) {
        if (this == &filter) return *this;
        name = filter.name;
        patterns = filter.patterns;
        return *this;
    }

    void SetName(const char* n) noexcept {
        name = n;
    }

    void AddPattern(const char* pattern) noexcept {
        patterns.emplace_back(pattern);
    }

    uiFileDialogParamsFilter ToLibuiFilter() const noexcept {
        return {
            name,
            patterns.size(),
            &patterns[0]
        };
    }
};

class FilterList {
 private:
    noex::string filter_buf_str;
    noex::vector<Filter> filters;
    noex::vector<uiFileDialogParamsFilter> ui_filters;

 public:
    FilterList() noexcept: filter_buf_str(), filters(), ui_filters() {}

    void MakeFilters(const noex::string& ext) noexcept {
        filter_buf_str = ext;
        char* filter_buf = filter_buf_str.data();
        if (!filter_buf) return;

        size_t i = 0;
        size_t start = 0;
        bool is_reading_pattern = false;
        Filter filter = Filter();

        for (const char c : ext) {
            if (c == '|') {
                filter_buf[i] = 0;
                if (is_reading_pattern) {
                    filter.AddPattern(&filter_buf[start]);
                    AddFilter(filter);
                    filter = Filter();
                } else {
                    filter.SetName(&filter_buf[start]);
                }
                is_reading_pattern = !is_reading_pattern;
                start = i + 1;
            } else if (is_reading_pattern && (c == ';')) {
                filter_buf[i] = 0;
                filter.AddPattern(&filter_buf[start]);
                start = i + 1;
            } else {
                // filter_buf[i] = c;
            }
            i++;
        }
        filter_buf[i] = 0;
        if (is_reading_pattern) {
            filter.AddPattern(&filter_buf[start]);
            AddFilter(filter);
        }

        ui_filters.reserve(filters.size());
        if (ui_filters.capacity() != filters.size()) {
            // Failed to reserve the buffer.
            filters.clear();
            return;
        }
        for (size_t j = 0; j < filters.size(); j++) {
            ui_filters.push_back(filters[j].ToLibuiFilter());
        }
    }

    ~FilterList() noexcept {}

    void AddFilter(const Filter& f) noexcept {
        filters.push_back(f);
    }

    size_t GetSize() const noexcept {
        return filters.size();
    }

    uiFileDialogParamsFilter* ToLibuiFilterList() const noexcept {
        return ui_filters.data();
    }
};

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

    filename = uiOpenFileWithParams(GetToplevel(uiControl(entry)), &params);
    if (filename == NULL) {
        return;
    }

    uiEntrySetText(entry, filename);
    uiFreeText(filename);
}

static void onOpenFolderClicked(uiButton *b, void *data) noexcept {
    DirPicker* picker = static_cast<DirPicker*>(data);
    picker->OpenFolder();
    UNUSED(b);
}

// Dir Picker
DirPicker::DirPicker(uiBox* box, const rapidjson::Value& j) noexcept
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

void DirPicker::SetConfig(const rapidjson::Value& config) noexcept {
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
ComboBox::ComboBox(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    uiCombobox* combo = uiNewCombobox();
    noex::vector<noex::string> values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiComboboxAppend(combo, label);
        const char* value = json_utils::GetString(i, "value", label);
        values.emplace_back(value);
    }
    uiBoxAppend(box, uiControl(combo), 0);
    uiComboboxSetSelected(combo, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    SetTooltip(uiControl(combo), j);
    m_widget = combo;
}

noex::string ComboBox::GetRawString() noexcept {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    return m_values[sel];
}

void ComboBox::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsInt()) {
        int i = config[m_id.c_str()].GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiComboboxSetSelected(static_cast<uiCombobox*>(m_widget), i);
    }
}

void ComboBox::GetConfig(rapidjson::Document& config) noexcept {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, sel, config.GetAllocator());
}

// RadioButtons
RadioButtons::RadioButtons(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    uiRadioButtons* radio = uiNewRadioButtons();
    noex::vector<noex::string> values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiRadioButtonsAppend(radio, label);
        const char* value = json_utils::GetString(i, "value", label);
        values.emplace_back(value);
    }
    uiBoxAppend(box, uiControl(radio), 0);
    uiRadioButtonsSetSelected(radio, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    SetTooltip(uiControl(radio), j);
    m_widget = radio;
}

noex::string RadioButtons::GetRawString() noexcept {
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    return m_values[sel];
}

void RadioButtons::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsInt()) {
        int i = config[m_id.c_str()].GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiRadioButtonsSetSelected(static_cast<uiRadioButtons*>(m_widget), i);
    }
}

void RadioButtons::GetConfig(rapidjson::Document& config) noexcept {
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, sel, config.GetAllocator());
}

// CheckBox
CheckBox::CheckBox(uiBox* box, const rapidjson::Value& j) noexcept
    : Component(j) {
    m_has_string = true;
    uiCheckbox* check = uiNewCheckbox(m_label.c_str());
    uiCheckboxSetChecked(check, json_utils::GetBool(j, "default", false));
    uiBoxAppend(box, uiControl(check), 0);

    m_value = json_utils::GetString(j, "value", m_label.c_str());

    SetTooltip(uiControl(check), j);
    m_widget = check;
}

noex::string CheckBox::GetRawString() noexcept {
    if (uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget)))
        return m_value;
    return "";
}

void CheckBox::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsBool())
        uiCheckboxSetChecked(static_cast<uiCheckbox*>(m_widget), config[m_id.c_str()].GetBool());
}

void CheckBox::GetConfig(rapidjson::Document& config) noexcept {
    bool checked = uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget));
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, checked, config.GetAllocator());
}

// CheckArray
CheckArray::CheckArray(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    noex::vector<noex::string> values;
    uiBox* check_array_box = uiNewVerticalBox();
    uiBoxSetSpacing(check_array_box, tuw_constants::BOX_CHECKS_SPACE);
    size_t id = 0;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiCheckbox* check = uiNewCheckbox(label);
        uiCheckboxSetChecked(check, json_utils::GetBool(i, "default", false));
        uiBoxAppend(check_array_box, uiControl(check), 0);
        SetTooltip(uiControl(check), i);
        m_checks.push_back(check);
        const char* value = json_utils::GetString(i, "value", label);
        values.emplace_back(value);
        id++;
    }
    uiBoxAppend(box, uiControl(check_array_box), 0);
    SetValues(values);
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

void CheckArray::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsArray()) {
        for (unsigned i = 0; i < config[m_id.c_str()].Size() && i < m_checks.size(); i++) {
            if (config[m_id.c_str()][i].IsBool())
                uiCheckboxSetChecked(m_checks[i], config[m_id.c_str()][i].GetBool());
        }
    }
}

void CheckArray::GetConfig(rapidjson::Document& config) noexcept {
    rapidjson::Value ints;
    ints.SetArray();
    for (uiCheckbox* check : m_checks) {
        ints.PushBack(static_cast<bool>(uiCheckboxChecked(check)),
                      config.GetAllocator());
    }
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, ints, config.GetAllocator());
}

// TextBox
TextBox::TextBox(uiBox* box, const rapidjson::Value& j) noexcept
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

void TextBox::SetConfig(const rapidjson::Value& config) noexcept {
    setConfigForTextBox(config, m_id, m_widget);
}

static void initSpinbox(uiSpinbox* picker, uiBox* box, const rapidjson::Value& j) noexcept {
    uiBoxAppend(box, uiControl(picker), 0);
    SetTooltip(uiControl(picker), j);
}

IntPicker::IntPicker(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    int min = json_utils::GetInt(j, "min", 0);
    int max = json_utils::GetInt(j, "max", 100);
    if (min > max) {
        int x = min;
        min = max;
        max = x;
    }
    int inc = json_utils::GetInt(j, "inc", 1);
    if (inc < 0)
        inc = -inc;
    else if (inc == 0)
        inc = 1;
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

void IntPicker::GetConfig(rapidjson::Document& config) noexcept {
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, val, config.GetAllocator());
}

void IntPicker::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsInt()) {
        int val = config[m_id.c_str()].GetInt();
        uiSpinboxSetValue(static_cast<uiSpinbox*>(m_widget), val);
    }
}

FloatPicker::FloatPicker(uiBox* box, const rapidjson::Value& j) noexcept
    : StringComponentBase(box, j) {
    double min = json_utils::GetDouble(j, "min", 0.0);
    double max = json_utils::GetDouble(j, "max", 100.0);
    if (min > max) {
        double x = min;
        min = max;
        max = x;
    }
    double inc = json_utils::GetDouble(j, "inc", 1.0);
    if (inc < 0)
        inc = -inc;
    else if (inc == 0)
        inc = 1.0;
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

void FloatPicker::GetConfig(rapidjson::Document& config) noexcept {
    double val = uiSpinboxValueDouble(static_cast<uiSpinbox*>(m_widget));
    rapidjson::Value n = CreateConfigValue(config, m_id);
    config.AddMember(n, val, config.GetAllocator());
}

void FloatPicker::SetConfig(const rapidjson::Value& config) noexcept {
    if (config.HasMember(m_id.c_str()) && config[m_id.c_str()].IsDouble()) {
        double val = config[m_id.c_str()].GetDouble();
        uiSpinboxSetValueDouble(static_cast<uiSpinbox*>(m_widget), val);
    }
}
