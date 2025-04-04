#include <stdio.h>
#ifdef _WIN32
#include <locale.h>
#else
#include <sys/stat.h>
#endif
#include "ui.h"
#include "json_utils.h"
#include "main_frame.h"
#include "exe_container.h"
#include "env_utils.h"
#include "string_utils.h"
#include "tuw_constants.h"

int main_app(const char* json_path = nullptr) noexcept {
#ifdef _WIN32
    // Enable ANSI escape sequences on the console window.
    EnableCSI();
#endif

    uiInitOptions options;
    const char *err;

    memset(&options, 0, sizeof (uiInitOptions));
    err = uiInit(&options);
    if (err != NULL) {
        fprintf(stderr, "error initializing libui: %s", err);
        uiFreeInitError(err);
        return 1;
    }

#ifdef __TUW_UNIX__
    // Need this for uiMainStep(1)
    uiMainSteps();
#endif

    MainFrame main_frame = MainFrame(json_path);
    uiMain();
    return 0;
}

bool AskOverwrite(const char *path) noexcept {
    if (!envuFileExists(path)) return true;
    PrintFmt("Overwrite %s? (y/n)\n", path);
    char answer;
    int ret = scanf("%c", &answer);
    fseek(stdin, 0, SEEK_END);
    return ret == 1 && (answer == "y"[0] || answer == "Y"[0]);
}

noex::string Merge(const noex::string& exe_path, const noex::string& json_path,
                    const noex::string& new_path, const bool force) noexcept {
    rapidjson::Document json;
    noex::string err;
    err = json_utils::LoadJson(json_path, json);
    if (!err.empty()) return err;

    if (!json.IsObject() || json.ObjectEmpty()) {
        PrintFmt("JSON file loaded but it has no data.\n");
        return "";
    }
    ExeContainer exe;
    err = exe.Read(exe_path);
    if (!err.empty()) return err;

    PrintFmt("Importing a json file... (%s)\n", json_path.c_str());
    exe.SetJson(json);
    if (!force && !AskOverwrite(new_path.c_str())) {
        PrintFmt("The operation has been cancelled.\n");
        return "";
    }
    err = exe.Write(new_path);
    if (!err.empty()) return err;
    PrintFmt("Generated an executable. (%s)\n", new_path.c_str());
#ifndef _WIN32
    // Allow executing file as program.
    chmod(new_path.c_str(),
          S_IRUSR | S_IWUSR | S_IXUSR |  // rwx
          S_IRGRP | S_IXGRP |  // r-x
          S_IROTH | S_IXOTH);  // r-x
#endif
    return "";
}

noex::string Split(const noex::string& exe_path, const noex::string& json_path,
                    const noex::string& new_path, const bool force) noexcept {
    ExeContainer exe;
    noex::string err = exe.Read(exe_path);
    if (!err.empty()) return err;
    if (!exe.HasJson()) {
        PrintFmt("The executable has no json data.\n");
        return "";
    }
    PrintFmt("Extracting JSON data from the executable...\n");
    rapidjson::Document json;
    exe.GetJson(json);
    exe.RemoveJson();
    if (!force && (!AskOverwrite(new_path.c_str()) || !AskOverwrite(json_path.c_str()))) {
        PrintFmt("The operation has been cancelled.\n");
        return "";
    }
    err = exe.Write(new_path);
    if (!err.empty()) return err;
    err = json_utils::SaveJson(json, json_path);
    if (!err.empty()) return err;
    PrintFmt("Generated an executable. (%s)\n", new_path.c_str());
    PrintFmt("Exported a json file. (%s)\n", json_path.c_str());
#ifndef _WIN32
    // Allow executing file as program.
    chmod(new_path.c_str(),
          S_IRUSR | S_IWUSR | S_IXUSR |  // rwx
          S_IRGRP | S_IXGRP |  // r-x
          S_IROTH | S_IXOTH);  // r-x
#endif
    return "";
}

void PrintUsage() noexcept {
    static const char* const usage =
        "Usage: Tuw [<command> [<options>]]\n"
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
        "       -f     : Force to overwrite files.\n"
        "\n"
        "Example:\n"
        "    Tuw merge -f -j my_definition.json -e MyGUI.exe\n"
        "\n";

    PrintFmt(usage);
}

enum Commands: int {
    CMD_UNKNOWN = 0,
    CMD_MERGE,
    CMD_SPLIT,
    CMD_VERSION,
    CMD_HELP,
    CMD_MAX
};

// don't use map. it will make exe larger.
int CmdToInt(const char* cmd) noexcept {
    while (*cmd == '-') {
        cmd++;
    }
    char c = *cmd;
    if (c && !cmd[1]) {
        if (c == 'm')
            return CMD_MERGE;
        if (c == 's')
            return CMD_SPLIT;
        if (c == 'v')
            return CMD_VERSION;
        if (c == 'h')
            return CMD_HELP;
    }
    if (strcmp(cmd, "merge") == 0)
        return CMD_MERGE;
    if (strcmp(cmd, "split") == 0)
        return CMD_SPLIT;
    if (strcmp(cmd, "ver") == 0)
        return CMD_VERSION;
    if (strcmp(cmd, "help") == 0)
        return CMD_HELP;
    return CMD_UNKNOWN;
}

enum Options: int {
    OPT_UNKNOWN = 0,
    OPT_JSON,
    OPT_EXE,
    OPT_FORCE,
    OPT_MAX
};

int OptToInt(const char* opt) noexcept {
    while (*opt == '-') {
        opt++;
    }
    char c = *opt;
    if (c && !opt[1]) {
        if (c == 'j')
            return OPT_JSON;
        if (c == 'e')
            return OPT_EXE;
        if (c == 'f' || c == 'y')
            return OPT_FORCE;
    }
    if (strcmp(opt, "json") == 0)
        return OPT_JSON;
    if (strcmp(opt, "exe") == 0)
        return OPT_EXE;
    if (strcmp(opt, "force") == 0)
        return OPT_FORCE;
    return OPT_UNKNOWN;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[]) noexcept {
    setlocale(LC_CTYPE, "");
#else
int main(int argc, char* argv[]) noexcept {
#endif  // _WIN32
    noex::vector<noex::string> args;
    for (int i = 0; i < argc; i++) {
#ifdef __APPLE__
        // Note: When you run Tuw as a macOS application,
        //       The first argument will be its PSN (process serial number).
        noex::string arg = argv[i];
        if (arg.starts_with("-psn_"))
            continue;  // Ignore the PSN
#endif  // __APPLE__
#ifdef _WIN32
        args.emplace_back(UTF16toUTF8(argv[i]));
#else
        args.emplace_back(argv[i]);
#endif  // _WIN32
    }

    noex::string exe_path = envuStr(envuGetExecutablePath());

    // Launch GUI if no args.
    if (args.size() <= 1) return main_app();

    // Launch GUI with a JSON path.
    if (args[1].contains('.')) return main_app(args[1].c_str());

    // Run as a CLI tool
    const char* cmd_str = args[1].c_str();
    int cmd_int = CmdToInt(cmd_str);
    if (cmd_int == CMD_UNKNOWN) {
        PrintUsage();
        FprintFmt(stderr, "Error: Unknown command detected. (%s)", cmd_str);
        return 1;
    }

    const char* json_path_cstr = nullptr;
    noex::string json_path;
    noex::string new_exe_path;
    bool force = false;

    for (size_t i = 2; i < args.size(); i++) {
        const char* opt_str = args[i].c_str();
        int opt_int = OptToInt(opt_str);
        if ((opt_int == OPT_JSON || opt_int == OPT_EXE) && args.size() <= i + 1) {
            PrintUsage();
            FprintFmt(stderr, "Error: This option requires a file path. (%s)\n", opt_str);
            return 1;
        } else if (opt_int == OPT_UNKNOWN) {
            PrintUsage();
            FprintFmt(stderr, "Error: Unknown option detected. (%s)\n", opt_str);
            return 1;
        } else if (opt_int == OPT_JSON) {
            i++;
            json_path_cstr = args[i].data();
        } else if (opt_int == OPT_EXE) {
            i++;
            new_exe_path = args[i];
        } else if (opt_int == OPT_FORCE) {
            force = true;
        }
    }

    if (!json_path_cstr || !*json_path_cstr) {
        if (cmd_int == CMD_MERGE)
            json_path_cstr = GetDefaultJsonPath();
        else
            json_path_cstr = "new.json";
    }

    if (new_exe_path.empty())
        new_exe_path = exe_path + ".new";

    json_path = envuStr(envuGetFullPath(json_path_cstr));
    new_exe_path = envuStr(envuGetFullPath(new_exe_path.c_str()));

    if (json_path == exe_path || new_exe_path == exe_path) {
        PrintUsage();
        fprintf(stderr, "Error: Can NOT overwrite the executable itself.\n");
        return 1;
    }

    rapidjson::Document json(rapidjson::kObjectType);
    noex::string err;

    if (cmd_int == CMD_MERGE)
        err = Merge(exe_path, json_path, new_exe_path, force);
    else if (cmd_int == CMD_SPLIT)
        err = Split(exe_path, json_path, new_exe_path, force);
    else if (cmd_int == CMD_VERSION)
        PrintFmt("%s\n", tuw_constants::VERSION);
    else if (cmd_int == CMD_HELP)
        PrintUsage();

    if (!err.empty()) {
        FprintFmt(stderr, "Error: %s\n", err.c_str());
        return 1;
    }
    return 0;
}
