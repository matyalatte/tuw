#include "component.h"
#include "json_utils.h"

enum ComponentType: int {
    COMP_UNKNOWN = 0,
    COMP_EMPTY,
    COMP_STATIC_TEXT,
    COMP_FILE,
    COMP_FOLDER,
    COMP_CHOICE,
    COMP_CHECK,
    COMP_CHECK_ARRAY,
    COMP_TEXT,
    COMP_INT,
    COMP_FLOAT,
    COMP_MAX
};

static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
static const uint32_t FNV_PRIME_32 = 16777619U;

static uint32_t Fnv1Hash32(const std::string& str) {
    uint32_t hash = FNV_OFFSET_BASIS_32;
    for (const char& c : str) hash = (FNV_PRIME_32 * hash) ^ c;
    return hash;
}

// Base class for GUI components (file picker, combo box, etc.)
Component::Component(const rapidjson::Value& j) {
    m_widget = nullptr;
    m_has_string = false;
    m_label = j["label"].GetString();
    m_id = json_utils::GetString(j, "id", "");
    if (m_id == "") {
        uint32_t hash = Fnv1Hash32(j["label"].GetString());
        m_id = "_" + std::to_string(hash);
    }
    m_add_quotes = json_utils::GetBool(j, "add_quotes", false);
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
        case COMP_CHOICE:
            comp = new Choice(box, j);
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
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(text, json_utils::GetString(j, "tooltip", ""));
}

// Base Class for strings
StringComponentBase::StringComponentBase(
    uiBox* box, const rapidjson::Value& j)
    : Component(j) {
    m_has_string = false;
    uiLabel* text = uiNewLabel(m_label.c_str());
    uiBoxAppend(box, uiControl(text), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(text, json_utils::GetString(j, "tooltip", ""));
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

static void onOpenFileClicked(uiButton *b, void *data)
{
    uiEntry *entry = uiEntry(data);
    char *filename;

    filename = uiOpenFile(GetToplevel(uiControl(entry)));
    if (filename == NULL) {
        return;
    }
    uiEntrySetText(entry, filename);
    uiFreeText(filename);
}

static void onFilesDropped(uiEntry *e, int count, char** names, void *data)
{
    if (count < 1) return;
    uiEntrySetText(e, names[0]);
}

// File Picker
FilePicker::FilePicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    const char* ext = json_utils::GetString(j, "extension", "any files (*)|*");
    const char* value = json_utils::GetString(j, "default", "");
    const char* empty_message = json_utils::GetString(j, "empty_message", "");
    const char* button_label = json_utils::GetString(j, "button", "Browse");

    uiEntry* entry = uiNewEntry();
    uiEntryOnFilesDropped(entry, onFilesDropped, NULL);
    uiEntrySetAcceptDrops(entry, 1);
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, empty_message);

    uiButton* button = uiNewButton(button_label);
    uiButtonOnClicked(button, onOpenFileClicked, entry);

    uiGrid* grid = uiNewGrid();
    uiGridAppend(grid, uiControl(entry),
        0, 0, 1, 1,
        1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(button),
        1, 0, 1, 1,
        0, uiAlignFill, 0, uiAlignFill);

    uiBoxAppend(box, uiControl(grid), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(text, json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string FilePicker::GetRawString() {
    return uiEntryText(static_cast<uiEntry*>(m_widget));
}

void FilePicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

static void onOpenFolderClicked(uiButton *b, void *data)
{
	uiEntry *entry = uiEntry(data);
	char *filename;

    filename = uiOpenFolder(GetToplevel(uiControl(entry)));
	if (filename == NULL) {
		return;
	}
	uiEntrySetText(entry, filename);
	uiFreeText(filename);
}

// Dir Picker
DirPicker::DirPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    const char* value = json_utils::GetString(j, "default", "");
    const char* empty_message = json_utils::GetString(j, "empty_message", "");
    const char* button_label = json_utils::GetString(j, "button", "Browse");

    uiEntry* entry = uiNewEntry();
    uiEntryOnFilesDropped(entry, onFilesDropped, NULL);
    uiEntrySetAcceptDrops(entry, 1);
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, empty_message);

    uiButton* button = uiNewButton(button_label);
    uiButtonOnClicked(button, onOpenFolderClicked, entry);

    uiGrid* grid = uiNewGrid();
    uiGridAppend(grid, uiControl(entry),
        0, 0, 1, 1,
        1, uiAlignFill, 0, uiAlignFill);
    uiGridAppend(grid, uiControl(button),
        1, 0, 1, 1,
        0, uiAlignFill, 0, uiAlignFill);

    uiBoxAppend(box, uiControl(grid), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(text, json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string DirPicker::GetRawString() {
    return uiEntryText(static_cast<uiEntry*>(m_widget));
}

void DirPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

// Choice
Choice::Choice(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    uiCombobox* choice = uiNewCombobox();
    std::vector<std::string> values;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiComboboxAppend(choice, label);
        const char* value = json_utils::GetString(i, "value", label);
        values.push_back(value);
    }
    uiBoxAppend(box, uiControl(choice), 0);
    uiComboboxSetSelected(choice, json_utils::GetInt(j, "default", 0) % j["items"].Size());

    SetValues(values);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(choice, json_utils::GetString(j, "tooltip", ""));
    m_widget = choice;
}

std::string Choice::GetRawString() {
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
    return m_values[sel];
}

void Choice::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int  i = config[m_id].GetInt();
        if (i < m_values.size())
            uiComboboxSetSelected(static_cast<uiCombobox*>(m_widget), i);
    }
}

void Choice::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    int sel = uiComboboxSelected(static_cast<uiCombobox*>(m_widget));
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

    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(check, json_utils::GetString(j, "tooltip", ""));
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
    size_t id = 0;
    for (const rapidjson::Value& i : j["items"].GetArray()) {
        const char* label = i["label"].GetString();
        uiCheckbox* check = uiNewCheckbox(label);
        uiCheckboxSetChecked(check, json_utils::GetBool(i, "default", false));
        uiBoxAppend(box, uiControl(check), 0);
        // libui doesn't support tooltips yet.
        // if (j.HasMember("tooltip"))
        //     uiControlSetTooltip(check, json_utils::GetString(j, "tooltip", ""));
        checks->push_back(check);
        const char* value = json_utils::GetString(i, "value", label);
        values.push_back(value);
        id++;
    }
    SetValues(values);
    m_widget = checks;
}

std::string CheckArray::GetRawString() {
    std::string str = "";
    std::vector<uiCheckbox*> checks;
    checks = *(std::vector<uiCheckbox*>*)m_widget;
    for (int i = 0; i < checks.size(); i++) {
        if (uiCheckboxChecked(checks[i])) {
            str += m_values[i];
        }
    }
    return str;
}

void CheckArray::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsArray()) {
        std::vector<uiCheckbox*> checks = *(std::vector<uiCheckbox*>*)m_widget;
        for (int i = 0; i < config[m_id].Size() && i < checks.size(); i++) {
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
        ints.PushBack(bool(uiCheckboxChecked(check)), config.GetAllocator());
    }
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    config.AddMember(n, ints, config.GetAllocator());
}

// TextBox
TextBox::TextBox(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    const char* value = json_utils::GetString(j, "default", "");
    const char* empty_message = json_utils::GetString(j, "empty_message", "");
    uiEntry* entry = uiNewEntry();
    uiEntrySetText(entry, value);
    uiEntrySetPlaceholder(entry, empty_message);
    uiBoxAppend(box, uiControl(entry), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(text, json_utils::GetString(j, "tooltip", ""));
    m_widget = entry;
}

std::string TextBox::GetRawString() {
    return uiEntryText(static_cast<uiEntry*>(m_widget));
}

void TextBox::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsString()) {
        const char* str = config[m_id].GetString();
        uiEntry* entry = static_cast<uiEntry*>(m_widget);
        uiEntrySetText(entry, str);
    }
}

static void onSpin(uiSpinbox *sender, void* data) {
    IntPicker* picker = static_cast<IntPicker*>(data);
    int inc = picker->GetInc();
    int min = picker->GetMin();
    int max = picker->GetMax();
    //bool wrap = picker->GetWrap();
    int old_val = picker->GetOldVal();
    int val = uiSpinboxValue(sender);
    int diff = val - old_val;
    if (diff == 0) {
        return;
    } else if (diff == 1) {
        val = old_val + inc;
        val = (val - min) / inc * inc + min;
        if (val > max)
            val = max;
    } else if (diff == -1) {
        val = old_val - inc;
        val = max - (max - val) / inc * inc;
        if (val < min)
            val = min;
    } else {
        picker->SetOldVal(val);
        return;
    }
    picker->SetOldVal(val);
    uiSpinboxSetValue(sender, val);
}

IntPicker::IntPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    m_min = json_utils::GetInt(j, "min", 0);
    m_max = json_utils::GetInt(j, "max", 100);
    if (m_min > m_max) {
        int x = m_min;
        m_min = m_max;
        m_max = x;
    }
    m_inc = json_utils::GetInt(j, "inc", 1);  // not supported yet?
    if (m_inc < 0) {
        m_inc = -m_inc;
    } else if (m_inc == 0) {
        m_inc = 1;
    }
    int val = json_utils::GetInt(j, "default", m_min);
    m_wrap = json_utils::GetBool(j, "wrap", false);  // not supported yet?
    uiSpinbox* picker = uiNewSpinbox(m_min, m_max);
    uiSpinboxOnChanged(picker, onSpin, this);
    uiSpinboxSetValue(picker, val);
    m_old_val = uiSpinboxValue(picker);
    uiBoxAppend(box, uiControl(picker), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(picker, json_utils::GetString(j, "tooltip", ""));
    m_widget = picker;
}

std::string IntPicker::GetRawString() {
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    return std::to_string(val);
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

static void onSpinDouble(uiSpinbox *sender, void* data) {
    FloatPicker* picker = static_cast<FloatPicker*>(data);
    int inc = int(picker->GetInc());
    printf("%d\n", inc);
    int min = int(picker->GetMin());
    int max = int(picker->GetMax());
    //bool wrap = picker->GetWrap();
    int old_val = int(picker->GetOldVal());
    int val = uiSpinboxValue(sender);
    int diff = val - old_val;
    if (diff == 0) {
        return;
    } else if (diff == 1) {
        val = old_val + inc;
        val = (val - min) / inc * inc + min;
        if (val > max)
            val = max;
    } else if (diff == -1) {
        val = old_val - inc;
        val = max - (max - val) / inc * inc;
        if (val < min)
            val = min;
    } else {
        picker->SetOldVal(double(val));
        return;
    }
    picker->SetOldVal(double(val));
    uiSpinboxSetValue(sender, val);
}

FloatPicker::FloatPicker(uiBox* box, const rapidjson::Value& j)
    : StringComponentBase(box, j) {
    m_min = json_utils::GetDouble(j, "min", 0.0);
    m_max = json_utils::GetDouble(j, "max", 100.0);
    if (m_min > m_max) {
        double x = m_min;
        m_min = m_max;
        m_max = x;
    }
    m_inc = json_utils::GetDouble(j, "inc", 1.0);  // not supported yet?
    if (m_inc < 0) {
        m_inc = -m_inc;
    } else if (int(m_inc) == 0) {
        m_inc = 1.0;
    }
    int val = json_utils::GetDouble(j, "default", m_min);
    bool wrap = json_utils::GetBool(j, "wrap", false);  // not supported yet?

    //uiSpinboxDouble is not supported yet.
    uiSpinbox* picker = uiNewSpinbox(int(m_min), int(m_max));
    uiSpinboxOnChanged(picker, onSpinDouble, this);
    uiSpinboxSetValue(picker, int(val));
    m_old_val = double(uiSpinboxValue(picker));
    uiBoxAppend(box, uiControl(picker), 0);
    // libui doesn't support tooltips yet.
    // if (j.HasMember("tooltip"))
    //     uiControlSetTooltip(picker, json_utils::GetString(j, "tooltip", ""));
    m_widget = picker;
}

std::string FloatPicker::GetRawString() {
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    return std::to_string(val);
}

void FloatPicker::GetConfig(rapidjson::Document& config) {
    if (config.HasMember(m_id))
        config.RemoveMember(m_id);
    rapidjson::Value n(m_id.c_str(), config.GetAllocator());
    int val = uiSpinboxValue(static_cast<uiSpinbox*>(m_widget));
    config.AddMember(n, val, config.GetAllocator());
}

void FloatPicker::SetConfig(const rapidjson::Value& config) {
    if (config.HasMember(m_id) && config[m_id].IsInt()) {
        int val = config[m_id].GetInt();
        uiSpinboxSetValue(static_cast<uiSpinbox*>(m_widget), val);
    }
}
