#include <stdio.h>
#include <string.h>
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

int main_app() {
    uiInitOptions options;
    const char *err;

    memset(&options, 0, sizeof (uiInitOptions));
    err = uiInit(&options);
    if (err != NULL) {
        fprintf(stderr, "error initializing libui: %s", err);
        uiFreeInitError(err);
        return 1;
    }

#ifdef __linux__
    // Need this for uiMainStep(1)
    uiMainSteps();
#endif

    MainFrame main_frame = MainFrame();
    uiMain();
    return 0;
}


bool AskOverwrite(const std::string& path) {
    if (!env_utils::FileExists(path)) return true;
    PrintFmt("Overwrite %s? (y/n)\n", path.c_str());
    char answer;
    int ret = scanf("%c", &answer);
    fseek(stdin, 0, SEEK_END);
    return ret == 1 && (answer == "y"[0] || answer == "Y"[0]);
}

json_utils::JsonResult Merge(const std::string& exe_path, const std::string& json_path,
                             const std::string& new_path, const bool force) {
    rapidjson::Document json;
    json_utils::JsonResult result = json_utils::LoadJson(json_path, json);
    if (!result.ok) return result;

    if (json.Size() == 0) {
        PrintFmt("JSON file loaded but it has no data.\n");
        return { true };
    }
    ExeContainer exe;
    result = exe.Read(exe_path);
    if (!result.ok) return result;

    PrintFmt("Importing a json file... (%s)\n", json_path.c_str());
    exe.SetJson(json);
    if (!force && !AskOverwrite(new_path)) {
        PrintFmt("The operation has been cancelled.\n");
        return { true };
    }
    result = exe.Write(new_path);
    if (!result.ok) return result;
    PrintFmt("Generated an executable. (%s)\n", new_path.c_str());
#ifndef _WIN32
    // Allow executing file as program.
    chmod(new_path.c_str(),
          S_IRUSR | S_IWUSR | S_IXUSR |  // rwx
          S_IRGRP | S_IXGRP |  // r-x
          S_IROTH | S_IXOTH);  // r-x
#endif
    return { true };
}

json_utils::JsonResult Split(const std::string& exe_path, const std::string& json_path,
                             const std::string& new_path, const bool force) {
    ExeContainer exe;
    json_utils::JsonResult result = exe.Read(exe_path);
    if (!result.ok) return result;
    if (!exe.HasJson()) {
        PrintFmt("The executable has no json data.\n");
        return { true };
    }
    PrintFmt("Extracting JSON data from the executable...\n");
    rapidjson::Document json;
    exe.GetJson(json);
    exe.RemoveJson();
    if (!force && (!AskOverwrite(new_path) || !AskOverwrite(json_path))) {
        PrintFmt("The operation has been cancelled.\n");
        return { true };
    }
    result = exe.Write(new_path);
    if (!result.ok) return result;
    result = json_utils::SaveJson(json, json_path);
    if (!result.ok) return result;
    PrintFmt("Generated an executable. (%s)\n", new_path.c_str());
    PrintFmt("Exported a json file. (%s)\n", json_path.c_str());
#ifndef _WIN32
    // Allow executing file as program.
    chmod(new_path.c_str(),
          S_IRUSR | S_IWUSR | S_IXUSR |  // rwx
          S_IRGRP | S_IXGRP |  // r-x
          S_IROTH | S_IXOTH);  // r-x
#endif
    return { true };
}

void PrintUsage() {
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
int CmdToInt(const char* cmd) {
    if (strcmp(cmd, "merge") == 0)
        return CMD_MERGE;
    else if (strcmp(cmd, "m") == 0)
        return CMD_MERGE;
    else if (strcmp(cmd, "split") == 0)
        return CMD_SPLIT;
    else if (strcmp(cmd, "s") == 0)
        return CMD_SPLIT;
    else if (strcmp(cmd, "ver") == 0)
        return CMD_VERSION;
    else if (strcmp(cmd, "v") == 0)
        return CMD_VERSION;
    else if (strcmp(cmd, "help") == 0)
        return CMD_HELP;
    else if (strcmp(cmd, "h") == 0)
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

int OptToInt(const char* opt) {
    if (strcmp(opt, "json") == 0)
        return OPT_JSON;
    else if (strcmp(opt, "j") == 0)
        return OPT_JSON;
    else if (strcmp(opt, "exe") == 0)
        return OPT_EXE;
    else if (strcmp(opt, "e") == 0)
        return OPT_EXE;
    else if (strcmp(opt, "force") == 0)
        return OPT_FORCE;
    else if (strcmp(opt, "f") == 0)
        return OPT_FORCE;
    else if (strcmp(opt, "y") == 0)
        return OPT_FORCE;
    return OPT_UNKNOWN;
}

static std::string RemoveHyphen(const char* arg) {
    const char* pos = arg;
    while (pos[0] == "-"[0])
        pos++;
    return pos;
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
    setlocale(LC_CTYPE, "");
    std::vector<std::string> args;
    for (size_t i = 0; i < argc; i++) {
        args.push_back(UTF16toUTF8(argv[i]));
    }
#else
int main(int argc, char* argv[], char* envp[]) {
    std::vector<std::string> args;
    for (size_t i = 0; i < argc; i++) {
        args.push_back(argv[i]);
    }
#endif
    env_utils::InitEnv(envp);
    std::string exe_path = env_utils::GetExecutablePath();
    env_utils::SetCwd(env_utils::GetDirectory(exe_path));

    // Launch GUI if no args.
    if (argc == 1) return main_app();

    std::string cmd_str = RemoveHyphen(args[1].c_str());
    int cmd_int = CmdToInt(cmd_str.c_str());
    if (cmd_int == CMD_UNKNOWN) {
        PrintUsage();
        fprintf(stderr, "Error: Unknown command detected. (%s)", cmd_str.c_str());
        return 1;
    }

    std::string json_path = "";
    std::string new_exe_path = "";
    bool force = false;

    for (size_t i = 2; i < argc; i++) {
        std::string opt_str = RemoveHyphen(args[i].c_str());
        int opt_int = OptToInt(opt_str.c_str());
        if ((opt_int == OPT_JSON || opt_int == OPT_EXE) && argc <= i + 1) {
            PrintUsage();
            fprintf(stderr, "Error: This option requires a file path. (%s)\n", opt_str.c_str());
            return 1;
        }
        switch (opt_int) {
            case OPT_UNKNOWN:
                {
                    PrintUsage();
                    fprintf(stderr, "Error: Unknown option detected. (%s)\n", opt_str.c_str());
                    return 1;
                }
                break;
            case OPT_JSON:
                i++;
                json_path = args[i];
                break;
            case OPT_EXE:
                i++;
                new_exe_path = args[i];
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

    json_path = env_utils::GetFullPath(json_path);
    new_exe_path = env_utils::GetFullPath(new_exe_path);

    if (json_path == exe_path || new_exe_path == exe_path) {
        PrintUsage();
        fprintf(stderr, "Error: Can NOT overwrite the executable itself.\n");
        return 1;
    }

    rapidjson::Document json(rapidjson::kObjectType);
    json_utils::JsonResult result = { true };

    switch (cmd_int) {
        case CMD_MERGE:
            result = Merge(exe_path, json_path, new_exe_path, force);
            break;
        case CMD_SPLIT:
            result = Split(exe_path, json_path, new_exe_path, force);
            break;
        case CMD_VERSION:
            PrintFmt("%s\n", tuw_constants::VERSION);
            break;
        case CMD_HELP:
            PrintUsage();
            break;
        default:
            break;
    }

    if (!result.ok) {
        fprintf(stderr, "Error: %s\n", result.msg.c_str());
        return 1;
    }
    return 0;
}
