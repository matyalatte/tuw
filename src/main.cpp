#include "main_frame.h"
#include <map>

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

#ifdef USE_JSON_EMBEDDING

void Merge(const wxString& exe_path, const wxString& json_path, const wxString& new_path) {
    nlohmann::json json = json_utils::LoadJson(json_path.ToStdString());
    if (json.empty()) {
        std::cout << "JSON file loaded but it has no data." << std::endl;
        return;
    }
    ExeContainer exe;
    exe.Read(exe_path);
    std::cout << "Importing a json file... (" << json_path << ")" << std::endl;
    exe.SetJson(json);
    exe.Write(new_path);
    std::cout << "Generated an executable. (" << new_path << ")" << std::endl;
}

void Split(const wxString& exe_path, const wxString& json_path, const wxString& new_path) {
    ExeContainer exe;
    exe.Read(exe_path);
    if (!exe.HasJson()) {
        std::cout << "The executable has no json data." << std::endl;
        return;
    }
    std::cout << "Extracting JSON data from the executable..." << std::endl;
    nlohmann::json json = exe.GetJson();
    exe.RemoveJson();
    exe.Write(new_path);
    bool saved = json_utils::SaveJson(json, json_path.ToStdString());
    if (!saved)
        throw std::runtime_error("Failed to save json file.");
    std::cout << "Generated an executable. (" << new_path << ")" << std::endl;
    std::cout << "Exported a json file. (" << json_path << ")" << std::endl;
}

void PrintUsage() {
    static const char* const usage =
        "Usage: SimpleCommandRunner [<command> [<json> [<new exe>]]]\n"
        "\n"
        "    command:\n"
        "        merge : merge this executable and a JSON file into a new exe.\n"
        "        split : split this executable into a JSON file and the original exe.\n"
        "        help  : show this message.\n"
        "    json   : path to a JSON file.\n"
        "    new exe: path to a new executable file.\n"
        "\n"
        "Example:\n"
        "    SimpleCommandRunner merge my_definition.json MyGUI.exe\n";
    std::cout << usage << std::endl;
}

enum COMMANDS: int {
    CMD_UNKNOWN = 0,
    CMD_MERGE,
    CMD_SPLIT,
    CMD_HELP,
    CMD_MAX
};

std::map <wxString, int> CMD_TO_INT_MAP = {
    {"merge", CMD_MERGE},
    {"m", CMD_MERGE},
    {"split", CMD_SPLIT},
    {"s", CMD_SPLIT},
    {"help", CMD_HELP},
    {"h", CMD_HELP},
};

int CmdToInt(const wxString& command) {
    if (CMD_TO_INT_MAP.count(command) == 0)
        return CMD_UNKNOWN;
    return CMD_TO_INT_MAP[command];
}

wxString GetFullPath(const wxString& path) {
    wxFileName fn(path);
    fn.MakeAbsolute();
    return fn.GetFullPath();
}
#endif

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif

#ifdef USE_JSON_EMBEDDING
    // Launch GUI if no args.
    if (argc == 1) return wxEntry(argc, argv);

    // Make dummy app
    new wxApp();

    wxString exe_path =  wxStandardPaths::Get().GetExecutablePath();

    wxString command_str = (argv[1]);
    command_str.Replace("-", "");
    int command = CmdToInt(command_str);
    if (command == CMD_UNKNOWN) {
        PrintUsage();
        std::cerr << "Error: Unsupported command detected. (" << command_str << ")" << std::endl;
        return 1;
    } else if (command == CMD_HELP) {
        PrintUsage();
        return 0;
    }

    wxString json_path;
    if (argc > 2)
        json_path = wxString(argv[2]);
    else if (command == CMD_MERGE)
        json_path = "gui_definition.json";
    else
        json_path = "new.json";
    json_path = GetFullPath(json_path);

    wxString new_exe_path;
    if (argc > 3)
        new_exe_path = wxString(argv[3]);
    else
        new_exe_path = exe_path + ".new";
    new_exe_path = GetFullPath(new_exe_path);


    if (json_path == exe_path || new_exe_path == exe_path) {
        PrintUsage();
        std::cerr << "Error: Can NOT overwrite the executable itself." << std::endl;
        return 1;
    }
    if (argc > 4)
        std::cout << "Warning: Command-line mode will use 3 arguments at most,"
                     " but there are more args." << std::endl;

    try {
        switch (command) {
            case CMD_MERGE:
                Merge(exe_path, json_path, new_exe_path);
                break;
            case CMD_SPLIT:
                Split(exe_path, json_path, new_exe_path);
                break;
            default:
                break;
        }
    }
    catch (nlohmann::json::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
#else
    return wxEntry(argc, argv);
#endif
}
