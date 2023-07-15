#include "main_frame.h"
#include <locale.h>

// Main
class MainApp : public wxApp {
 private:
    MainFrame* m_frame;
 public:
    MainApp(): wxApp(), m_frame(nullptr) {}
    bool OnInit() override;
};

bool MainApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    // make main window
    m_frame = new MainFrame();
    m_frame->Show();

    return true;
}

wxDECLARE_APP(MainApp);
wxIMPLEMENT_APP_NO_MAIN(MainApp);

bool AskOverwrite(const wxString& path) {
    if (!wxFileExists(path)) {
        return true;
    }
    wxPrintf("Overwrite %s? (y/n)\n", path);
    char ans;
    int ret = scanf("%c", &ans);
    return ret == 1 && (ans == "y"[0] || ans == "Y"[0]);
}

wxResult Merge(const wxString& exe_path, const wxString& json_path, const wxString& new_path,
           const bool force) {
    rapidjson::Document json;
    json_utils::JsonResult result = json_utils::LoadJson(std::string(json_path.c_str()), json);
    if (!result.ok)
        return { false, wxString::FromUTF8(result.msg.c_str())};

    if (json.Size() == 0) {
        wxPrintf("JSON file loaded but it has no data.\n");
        return { true };
    }
    ExeContainer exe;
    wxResult wxresult = exe.Read(exe_path);
    if (!wxresult.ok)
        return wxresult;
    wxPrintf("Importing a json file... (%s)\n", json_path);
    exe.SetJson(json);
    if (!force && !AskOverwrite(new_path)) {
        wxPrintf("The operation has been cancelled.\n");
        return { true };
    }
    wxresult = exe.Write(new_path);
    if (!wxresult.ok)
        return wxresult;
    wxPrintf("Generated an executable. (%s)\n", new_path);
    return { true };
}

wxResult Split(const wxString& exe_path, const wxString& json_path, const wxString& new_path,
           const bool force) {
    ExeContainer exe;
    wxResult wxresult = exe.Read(exe_path);
    if (!wxresult.ok)
        return wxresult;
    if (!exe.HasJson()) {
        wxPrintf("The executable has no json data.\n");
        return { true };
    }
    wxPrintf("Extracting JSON data from the executable...\n");
    rapidjson::Document json;
    exe.GetJson(json);
    exe.RemoveJson();
    if (!force && (!AskOverwrite(new_path) || !AskOverwrite(json_path))) {
        wxPrintf("The operation has been cancelled.\n");
        return { true };
    }
    wxresult = exe.Write(new_path);
    if (!wxresult.ok)
        return wxresult;
    json_utils::JsonResult result = json_utils::SaveJson(json, std::string(json_path.c_str()));
    if (!result.ok)
        return { false, wxString::FromUTF8(result.msg.c_str())};
    wxPrintf("Generated an executable. (%s)\n", new_path);
    wxPrintf("Exported a json file. (%s)\n", json_path);
    return { true };
}

void PrintUsage() {
    static const char* const usage =
        "Usage: SimpleCommandRunner [<command> [<options>]]\n"
        "\n"
        "    command:\n"
        "        merge : merge this executable and a JSON file into a new exe.\n"
        "        split : split this executable into a JSON file and the original exe.\n"
        "        ver   : show the tool version.\n"
        "        help  : show this message.\n"
        "\n"
        "    options:\n"
        "       -j str : path to a JSON file.\n"
        "                default to 'gui_definition.json'\n"
        "       -e str : path to a new executable file.\n"
        "                default to exe name + '.new'\n"
        "       -f     : Force to overwrite files."
        "\n"
        "Example:\n"
        "    SimpleCommandRunner merge -f -j my_definition.json -e MyGUI.exe\n"
        "\n";

    wxPrintf(usage);
}

enum Commands: int {
    CMD_UNKNOWN = 0,
    CMD_MERGE,
    CMD_SPLIT,
    CMD_VERSION,
    CMD_HELP,
    CMD_MAX
};

const matya::map_as_vec<int> CMD_TO_INT = {
    {"merge", CMD_MERGE},
    {"m", CMD_MERGE},
    {"split", CMD_SPLIT},
    {"s", CMD_SPLIT},
    {"ver", CMD_VERSION},
    {"v", CMD_VERSION},
    {"help", CMD_HELP},
    {"h", CMD_HELP},
};

enum Options: int {
    OPT_UNKNOWN = 0,
    OPT_JSON,
    OPT_EXE,
    OPT_FORCE,
    OPT_MAX
};

const matya::map_as_vec<int> OPT_TO_INT = {
    {"json", OPT_JSON},
    {"j", OPT_JSON},
    {"exe", OPT_EXE},
    {"e", OPT_EXE},
    {"force", OPT_FORCE},
    {"f", OPT_FORCE},
    {"y", OPT_FORCE},
};

wxString GetFullPath(const wxString& path) {
    wxFileName fn(path);
    fn.MakeAbsolute();
    return fn.GetFullPath();
}

#define STATUS_ERROR 1
#define STATUS_SUCCESS 0

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif
    setlocale(LC_CTYPE, "");

    // Launch GUI if no args.
    if (argc == 1) return wxEntry(argc, argv);

    wxString cmd_str(argv[1]);
    cmd_str.Replace("-", "");
    int cmd_int = CMD_TO_INT.get(cmd_str.c_str(), CMD_UNKNOWN);
    if (cmd_int == CMD_UNKNOWN) {
        PrintUsage();
        wxFprintf(stderr, "Error: Unknown command detected. (%s)", cmd_str);
        return STATUS_ERROR;
    }

    wxString exe_path =  stdpath::GetExecutablePath(argv[0]);

    wxString json_path = "";
    wxString new_exe_path = "";
    bool force = false;

    for (size_t i = 2; i < argc; i++) {
        wxString opt_str = wxString(argv[i]);
        opt_str.Replace("-", "");
        int opt_int = OPT_TO_INT.get(opt_str.c_str(), OPT_UNKNOWN);
        if ((opt_int == OPT_JSON || opt_int == OPT_EXE) && argc <= i + 1) {
            PrintUsage();
            wxFprintf(stderr, "Error: This option requires a file path. (%s)\n", opt_str);
            return STATUS_ERROR;
        }
        switch (opt_int) {
            case OPT_UNKNOWN:
                {
                    PrintUsage();
                    wxFprintf(stderr, "Error: Unknown option detected. (%s)\n", opt_str);
                    return STATUS_ERROR;
                }
                break;
            case OPT_JSON:
                i++;
                json_path = wxString(argv[i]);
                break;
            case OPT_EXE:
                i++;
                new_exe_path = wxString(argv[i]);
                break;
            case OPT_FORCE:
                force = true;
                break;
            default:
                break;
        }
    }

    if (json_path == "") {
        if (cmd_int == CMD_MERGE)
            json_path = "gui_definition.json";
        else
            json_path = "new.json";
    }

    if (new_exe_path == "")
        new_exe_path = exe_path + ".new";

    json_path = GetFullPath(json_path);
    new_exe_path = GetFullPath(new_exe_path);

    if (json_path == exe_path || new_exe_path == exe_path) {
        PrintUsage();
        wxFprintf(stderr, "Error: Can NOT overwrite the executable itself.\n");
        return STATUS_ERROR;
    }

    wxResult result = { true };

    switch (cmd_int) {
        case CMD_MERGE:
            result = Merge(exe_path, json_path, new_exe_path, force);
            break;
        case CMD_SPLIT:
            result = Split(exe_path, json_path, new_exe_path, force);
            break;
        case CMD_VERSION:
            wxPrintf("%s\n", scr_constants::VERSION);
            break;
        case CMD_HELP:
            PrintUsage();
            break;
        default:
            break;
    }

    if (!result.ok) {
        wxFprintf(stderr, "Error: %s\n", result.msg);
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

#undef STATUS_ERROR
#undef STATUS_SUCCESS
