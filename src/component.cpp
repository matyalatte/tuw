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
Component::Component(const rapidjson::Value& j) {
    m_widget = nullptr;
    m_has_string = false;
    m_is_wide = false;
    m_label = j["label"].GetString();
    m_id = json_utils::GetString(j, "id", "");
    if (m_id == "") {
        uint32_t hash = Fnv1Hash32(j["label"].GetString());
        m_id = "_" + std::to_string(hash);
    }
    m_add_quotes = json_utils::GetBool(j, "add_quotes", false);
}

Component::~Component() {
}

std::string Component::GetString() {
    std::string str = GetRawString();
    if (m_add_quotes)
        return "\"" + str + "\"";
    return str;
}

std::string const Component::GetID() {
    return m_id;
}

Component* Component::PutComponent(uiBox* box, const rapidjson::Value& j) {
    Component* comp = nullptr;
    int type = j["type_int"].GetInt();
    switch (type) {
        case COMP_EMPTY:
            comp = new EmptyComponent(box, j);
            break;
        case COMP_STATIC_TEXT:
            comp = new StaticText(box, j);
            break;
        case COMP_FILE:
            comp = new FilePicker(box, j);
            break;
        case COMP_FOLDER:
            comp = new DirPicker(box, j);
            break;
        case COMP_COMBO:
            comp = new ComboBox(box, j);
            break;
        case COMP_RADIO:
            comp = new RadioButtons(box, j);
            break;
        case COMP_CHECK:
            comp = new CheckBox(box, j);
            break;
        case COMP_CHECK_ARRAY:
            comp = new CheckArray(box, j);
            break;
        case COMP_TEXT:
            comp = new TextBox(box, j);
            break;
        case COMP_INT:
            comp = new IntPicker(box, j);
            break;
        case COMP_FLOAT:
            comp = new FloatPicker(box, j);
            break;
        default:
            break;
    }
    return comp;
}

// Static Text
StaticText::StaticText(uiBox* box, const rapidjson::Value& j)
    : Component(j) {
    uiLabel* text = uiNewLabel(m_label.c_str());
    uiBoxAppend(box, uiControl(text), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(text), json_utils::GetString(j, "tooltip", ""));
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    uiBox* box, const rapidjson::Value& j)
    : Component(j) {
    m_has_string = false;
    uiLabel* text = uiNewLabel(m_label.c_str());
    uiBoxAppend(box, uiControl(text), 0);
}

void StringComponentBase::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    rapidjson::Value val(GetRawString(), config.GetAllocator());
    config.AddMember(n, val, config.GetAllocator());
}

static uiWindow* GetToplevel(uiControl* c) {
    if (uiControlToplevel(c)) return uiWindow(c);
    return GetToplevel(uiControlParent(c));
}

static void onOpenFileClicked(uiButton *b, void *data) {
    FilePicker* picker = static_cast<FilePicker*>(data);
    picker->OpenFile();
    UNUSED(b);
}

static void onFilesDropped(uiEntry *e, int count, char** names, void *data) {
    if (count < 1) return;
    uiEntrySetText(e, names[0]);
    UNUSED(data);
}

// File Picker
FilePicker::FilePicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    m_is_wide = true;
    m_ext = json_utils::GetString(j, "extension", "any files (*.*)|*.*");
    const char* value = json_utils::GetString(j, "default", "");
    const char* placeholder = json_utils::GetString(j, "placeholder", "");
    const char* button_label = json_utils::GetString(j, "button", "Browse");

    uiEntry* entry = uiNewEntry();
    uiEntryOnFilesDropped(entry, onFilesDropped, NULL);
    uiEntrySetAcceptDrops(entry, 1);
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, placeholder);

    uiButton* button = uiNewButton(button_label);
    uiButtonOnClicked(button, onOpenFileClicked, this);

    uiGrid* grid = uiNewGrid();
    uiGridAppend(grid, uiControl(entry),
        0, 0, 1, 1,
        1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(button),
        1, 0, 1, 1,
        0, uiAlignFill, 0, uiAlignFill);
    uiGridSetSpacing(grid, tuw_constants::GRID_COMP_XSPACE, 0);

    uiBoxAppend(box, uiControl(grid), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(entry), json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string FilePicker::GetRawString() {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    std::string str = text;
    uiFreeText(text);
    return str;
}

void FilePicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

class Filter {
 private:
    const char* name;
    std::vector<const char*> patterns;
 public:
    Filter(): name(), patterns() {}
    void SetName(const char* n) {
        name = n;
    }
    void AddPattern(const char* pattern) {
        patterns.push_back(pattern);
    }
    uiFileDialogParamsFilter ToLibuiFilter() {
        return {
            name,
            patterns.size(),
            &patterns[0]
        };
    }
};

class FilterList {
 private:
    char* filter_buf;
    std::vector<Filter*> filters;
    uiFileDialogParamsFilter* ui_filters;

 public:
    FilterList(): filter_buf(NULL), filters(), ui_filters(NULL) {}
    void MakeFilters(const std::string& ext) {
        if (filter_buf != NULL)
            delete[] filter_buf;
        filter_buf =  new char[ext.length() + 1];
        size_t i = 0;
        size_t start = 0;
        bool is_reading_pattern = false;
        Filter* filter = new Filter();
        for (const char c : ext) {
            if (c == "|"[0]) {
                filter_buf[i] = 0;
                if (is_reading_pattern) {
                    filter->AddPattern(&filter_buf[start]);
                    AddFilter(filter);
                    filter = new Filter();
                } else {
                    filter->SetName(&filter_buf[start]);
                }
                is_reading_pattern = !is_reading_pattern;
                start = i + 1;
            } else if (is_reading_pattern && (c == ";"[0])) {
                filter_buf[i] = 0;
                filter->AddPattern(&filter_buf[start]);
                start = i + 1;
            } else {
                filter_buf[i] = c;
            }
            i++;
        }
        filter_buf[i] = 0;
        if (is_reading_pattern) {
            filter->AddPattern(&filter_buf[start]);
            AddFilter(filter);
        }

        ui_filters = new uiFileDialogParamsFilter[filters.size()];
        for (size_t j = 0; j < filters.size(); j++) {
            ui_filters[j] = filters[j]->ToLibuiFilter();
        }
    }

    ~FilterList() {
        for (Filter* f : filters) {
            if (f != NULL)
                delete f;
        }
        if (filter_buf != NULL)
            delete[] filter_buf;
        if (ui_filters != NULL)
            delete[] ui_filters;
    }

    void AddFilter(Filter* f) {
        filters.push_back(f);
    }

    size_t GetSize() {
        return filters.size();
    }

    uiFileDialogParamsFilter* ToLibuiFilterList() {
        return ui_filters;
    }
};

void FilePicker::OpenFile() {
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

static void onOpenFolderClicked(uiButton *b, void *data) {
    DirPicker* picker = static_cast<DirPicker*>(data);
    picker->OpenFolder();
    UNUSED(b);
}

// Dir Picker
DirPicker::DirPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    m_is_wide = true;
    const char* value = json_utils::GetString(j, "default", "");
    const char* placeholder = json_utils::GetString(j, "placeholder", "");
    const char* button_label = json_utils::GetString(j, "button", "Browse");

    uiEntry* entry = uiNewEntry();
    uiEntryOnFilesDropped(entry, onFilesDropped, NULL);
    uiEntrySetAcceptDrops(entry, 1);
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, placeholder);

    uiButton* button = uiNewButton(button_label);
    uiButtonOnClicked(button, onOpenFolderClicked, this);

    uiGrid* grid = uiNewGrid();
    uiGridAppend(grid, uiControl(entry),
        0, 0, 1, 1,
        1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(button),
        1, 0, 1, 1,
        0, uiAlignFill, 0, uiAlignFill);
    uiGridSetSpacing(grid, tuw_constants::GRID_COMP_XSPACE, 0);

    uiBoxAppend(box, uiControl(grid), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(entry), json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string DirPicker::GetRawString() {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    std::string str = text;
    uiFreeText(text);
    return str;
}

void DirPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

void DirPicker::OpenFolder() {
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
ComboBox::ComboBox(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    uiCombobox* combo = uiNewCombobox();
    std::vector<std::string> values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiComboboxAppend(combo, label);
        const char* value = json_utils::GetString(i, "value", label);
        values.push_back(value);
    }
    uiBox* hbox = uiNewHorizontalBox();
    uiBoxAppend(hbox, uiControl(combo), 0);
    uiBoxAppend(box, uiControl(hbox), 0);
    uiComboboxSetSelected(combo, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(combo), json_utils::GetString(j, "tooltip", ""));
    m_widget = combo;
}

std::string ComboBox::GetRawString() {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    return m_values[sel];
}

void ComboBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int i = config[m_id].GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiComboboxSetSelected(static_cast<uiCombobox*>(m_widget), i);
    }
}

void ComboBox::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, sel, config.GetAllocator());
}

// RadioButtons
RadioButtons::RadioButtons(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    uiRadioButtons* radio = uiNewRadioButtons();
    std::vector<std::string> values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiRadioButtonsAppend(radio, label);
        const char* value = json_utils::GetString(i, "value", label);
        values.push_back(value);
    }
    uiBox* hbox = uiNewHorizontalBox();
    uiBoxAppend(hbox, uiControl(radio), 0);
    uiBoxAppend(box, uiControl(hbox), 0);
    uiRadioButtonsSetSelected(radio, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(radio), json_utils::GetString(j, "tooltip", ""));
    m_widget = radio;
}

std::string RadioButtons::GetRawString() {
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    return m_values[sel];
}

void RadioButtons::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int i = config[m_id].GetInt();
        if (i >= 0 && i < static_cast<int>(m_values.size()))
            uiRadioButtonsSetSelected(static_cast<uiRadioButtons*>(m_widget), i);
    }
}

void RadioButtons::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    int sel = uiRadioButtonsSelected(static_cast<uiRadioButtons*>(m_widget));
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, sel, config.GetAllocator());
}

// CheckBox
CheckBox::CheckBox(uiBox* box, const rapidjson::Value& j)
    : Component(j) {
    m_has_string = true;
    uiCheckbox* check = uiNewCheckbox(m_label.c_str());
    uiCheckboxSetChecked(check, json_utils::GetBool(j, "default", false));
    uiBoxAppend(box, uiControl(check), 0);

    m_value = json_utils::GetString(j, "value", m_label.c_str());

    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(check), json_utils::GetString(j, "tooltip", ""));
    m_widget = check;
}

std::string CheckBox::GetRawString() {
    if (uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget)))
        return m_value;
    return "";
}

void CheckBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsBool())
        uiCheckboxSetChecked(static_cast<uiCheckbox*>(m_widget), config[m_id].GetBool());
}

void CheckBox::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    bool checked = uiCheckboxChecked(static_cast<uiCheckbox*>(m_widget));
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, checked, config.GetAllocator());
}

// CheckArray
CheckArray::CheckArray(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    std::vector<uiCheckbox*>* checks = new std::vector<uiCheckbox*>();
    std::vector<std::string> values;
    uiBox* check_array_box = uiNewVerticalBox();
    uiBoxSetSpacing(check_array_box, tuw_constants::BOX_CHECKS_SPACE);
    size_t id = 0;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiCheckbox* check = uiNewCheckbox(label);
        uiCheckboxSetChecked(check, json_utils::GetBool(i, "default", false));
        uiBoxAppend(check_array_box, uiControl(check), 0);
        if (i.HasMember("tooltip")) {
            uiControlSetTooltip(uiControl(check),
                                            json_utils::GetString(i, "tooltip", ""));
        }
        checks->push_back(check);
        const char* value = json_utils::GetString(i, "value", label);
        values.push_back(value);
        id++;
    }
    uiBoxAppend(box, uiControl(check_array_box), 0);
    SetValues(values);
    m_widget = checks;
}

std::string CheckArray::GetRawString() {
    std::string str = "";
    std::vector<uiCheckbox*> checks;
    checks = *(std::vector<uiCheckbox*>*)m_widget;
    for (size_t i = 0; i < checks.size(); i++) {
        if (uiCheckboxChecked(checks[i])) {
            str += m_values[i];
        }
    }
    return str;
}

void CheckArray::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsArray()) {
        std::vector<uiCheckbox*> checks = *(std::vector<uiCheckbox*>*)m_widget;
        for (unsigned i = 0; i < config[m_id].Size() && i < checks.size(); i++) {
            if (config[m_id][i].IsBool())
                uiCheckboxSetChecked(checks[i], config[m_id][i].GetBool());
        }
    }
}

void CheckArray::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);

    rapidjson::Value ints;
    ints.SetArray();
    for (uiCheckbox* check : *(std::vector<uiCheckbox*>*)m_widget) {
        ints.PushBack(static_cast<bool>(uiCheckboxChecked(check)),
                      config.GetAllocator());
    }
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, ints, config.GetAllocator());
}

// TextBox
TextBox::TextBox(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    m_is_wide = true;
    const char* value = json_utils::GetString(j, "default", "");
    const char* placeholder = json_utils::GetString(j, "placeholder", "");
    uiEntry* entry = uiNewEntry();
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, placeholder);
    uiBoxAppend(box, uiControl(entry), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(entry), json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string TextBox::GetRawString() {
    char* text = uiEntryText(static_cast<uiEntry*>(m_widget));
    std::string str = text;
    uiFreeText(text);
    return str;
}

void TextBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

IntPicker::IntPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    int min = json_utils::GetInt(j, "min", 0);
    int max = json_utils::GetInt(j, "max", 100);
    if (min > max) {
        int x = min;
        min = max;
        max = x;
    }
    int inc = json_utils::GetInt(j, "inc", 1);
    if (inc < 0) {
        inc = -inc;
    } else if (inc == 0) {
        inc = 1;
    }
    int val = json_utils::GetInt(j, "default", min);
    bool wrap = json_utils::GetBool(j, "wrap", false);
    uiSpinbox* picker = uiNewSpinboxDoubleEx(
        static_cast<double>(min),
        static_cast<double>(max),
        0,
        static_cast<double>(inc),
        static_cast<int>(wrap));
    uiSpinboxSetValue(picker, val);
    uiBox* hbox = uiNewHorizontalBox();
    uiBoxAppend(hbox, uiControl(picker), 0);
    uiBoxAppend(box, uiControl(hbox), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(picker), json_utils::GetString(j, "tooltip", ""));
    m_widget = picker;
}

std::string IntPicker::GetRawString() {
    char* text = uiSpinboxValueText(static_cast<uiSpinbox*>(m_widget));
    std::string str(text);
    uiFreeText(text);
    return str;
}

void IntPicker::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    config.AddMember(n, val, config.GetAllocator());
}

void IntPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int val = config[m_id].GetInt();
        uiSpinboxSetValue(static_cast<uiSpinbox*>(m_widget), val);
    }
}

FloatPicker::FloatPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    double min = json_utils::GetDouble(j, "min", 0.0);
    double max = json_utils::GetDouble(j, "max", 100.0);
    if (min > max) {
        double x = min;
        min = max;
        max = x;
    }
    double inc = json_utils::GetDouble(j, "inc", 1.0);
    if (inc < 0) {
        inc = -inc;
    } else if (inc == 0) {
        inc = 1.0;
    }
    int digits = json_utils::GetInt(j, "digits", 1);
    double val = json_utils::GetDouble(j, "default", min);
    bool wrap = json_utils::GetBool(j, "wrap", false);
    uiSpinbox* picker = uiNewSpinboxDoubleEx(min, max, digits, inc, static_cast<int>(wrap));
    uiSpinboxSetValueDouble(picker, val);
    uiBox* hbox = uiNewHorizontalBox();
    uiBoxAppend(hbox, uiControl(picker), 0);
    uiBoxAppend(box, uiControl(hbox), 0);
    if (j.HasMember("tooltip"))
        uiControlSetTooltip(uiControl(picker), json_utils::GetString(j, "tooltip", ""));
    m_widget = picker;
}

std::string FloatPicker::GetRawString() {
    char* text = uiSpinboxValueText(static_cast<uiSpinbox*>(m_widget));
    std::string str(text);
    uiFreeText(text);
    return str;
}

void FloatPicker::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    double val = uiSpinboxValueDouble(static_cast<uiSpinbox*>(m_widget));
    config.AddMember(n, val, config.GetAllocator());
}

void FloatPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsDouble()) {
        double val = config[m_id].GetDouble();
        uiSpinboxSetValueDouble(static_cast<uiSpinbox*>(m_widget), val);
    }
}
