#include <iomanip>
#include "main_frame.h"

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
    std::cout << "Overwrite " << path << "? (y/n)" << std::endl;
    char ans[8];
    std::cin >> std::setw(8) >> ans;
    std::cin.ignore(1024, '\n');
    return (ans[0] == "y"[0] || ans[0] == "Y"[0]);
}

void Merge(const wxString& exe_path, const wxString& json_path, const wxString& new_path,
           const bool force) {
    rapidjson::Document json;
    json_utils::LoadJson(json_path.ToStdString(), json);
    if (json.Size() == 0) {
        std::cout << "JSON file loaded but it has no data." << std::endl;
        return;
    }
    ExeContainer exe;
    exe.Read(exe_path);
    std::cout << "Importing a json file... (" << json_path << ")" << std::endl;
    exe.SetJson(json);
    if (!force && !AskOverwrite(new_path)) {
        std::cout << "The operation has been cancelled." << std::endl;
        return;
    }
    exe.Write(new_path);
    std::cout << "Generated an executable. (" << new_path << ")" << std::endl;
}

void Split(const wxString& exe_path, const wxString& json_path, const wxString& new_path,
           const bool force) {
    ExeContainer exe;
    exe.Read(exe_path);
    if (!exe.HasJson()) {
        std::cout << "The executable has no json data." << std::endl;
        return;
    }
    std::cout << "Extracting JSON data from the executable..." << std::endl;
    rapidjson::Document json;
    exe.GetJson(json);
    exe.RemoveJson();
    if (!force && (!AskOverwrite(new_path) || !AskOverwrite(json_path))) {
        std::cout << "The operation has been cancelled." << std::endl;
        return;
    }
    exe.Write(new_path);
    bool saved = json_utils::SaveJson(json, json_path.ToStdString());
    if (!saved)
        throw std::runtime_error("Failed to save json file.");
    std::cout << "Generated an executable. (" << new_path << ")" << std::endl;
    std::cout << "Exported a json file. (" << json_path << ")" << std::endl;
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
        "    SimpleCommandRunner merge -f -j my_definition.json -e MyGUI.exe\n";

    std::cout << usage << std::endl;
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

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif

    // Launch GUI if no args.
    if (argc == 1) return wxEntry(argc, argv);

    wxString cmd_str(argv[1]);
    cmd_str.Replace("-", "");
    int cmd_int = CMD_TO_INT.get(cmd_str.c_str(), CMD_UNKNOWN);

    // Make dummy app
    new wxApp();

    wxString exe_path =  wxStandardPaths::Get().GetExecutablePath();

    wxString json_path = "";
    wxString new_exe_path = "";
    bool force = false;

    try {
        for (size_t i = 2; i < argc; i++) {
            wxString opt_str = wxString(argv[i]);
            opt_str.Replace("-", "");
            int opt_int = OPT_TO_INT.get(opt_str.c_str(), OPT_UNKNOWN);
            if ((opt_int == OPT_JSON || opt_int == OPT_EXE) && argc <= i + 1) {
                wxString msg = "This option requires a file path. (" + opt_str + ")";
                throw std::runtime_error(msg.c_str());
            }
            switch (opt_int) {
                case OPT_UNKNOWN:
                    {
                        wxString msg = "Unknown option detected. (" + opt_str + ")";
                        throw std::runtime_error(msg.c_str());
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
    }
    catch (std::exception& e) {
        PrintUsage();
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
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
        std::cerr << "Error: Can NOT overwrite the executable itself." << std::endl;
        return 1;
    }

    try {
        switch (cmd_int) {
            case CMD_UNKNOWN:
                PrintUsage();
                {
                    wxString msg = "Unknown command detected. (" + cmd_str + ")";
                    throw std::runtime_error(msg.c_str());
                }
            case CMD_MERGE:
                Merge(exe_path, json_path, new_exe_path, force);
                break;
            case CMD_SPLIT:
                Split(exe_path, json_path, new_exe_path, force);
                break;
            case CMD_VERSION:
                std::cout << scr_constants::VERSION << std::endl;
                break;
            case CMD_HELP:
                PrintUsage();
                break;
            default:
                break;
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
