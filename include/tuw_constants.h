#pragma once
namespace tuw_constants {
    constexpr char LOGO[] =
        "  _____  \n"
        " |_   _|   ___      __\n"
        "   | || | | \\ \\ /\\ / /\n"
        "   | || |_| |\\ V  V /\n"
        "   |_| \\__,_| \\_/\\_/\n"
        "  Tiny UI wrapper for\n"
        "       CLI tools\n";
    constexpr char TOOL_NAME[] = "Tuw";
    constexpr char AUTHOR[] = "matyalatte";
    constexpr char VERSION[] = "0.7.2";
    constexpr int VERSION_INT = 702;

#ifdef _WIN32
    #define TUW_CONSTANTS_OS "win"
    const int GRID_COMP_XSPACE = 2;
    const int GRID_MAIN_SPACE = 7;
    const int BOX_MAIN_SPACE = 6;
    const int BOX_SUB_SPACE = 1;
    const int BOX_CHECKS_SPACE = 0;
    const int BTN_WIDTH = 90;
    const int BTN_HEIGHT = 24;
#elif defined(__TUW_UNIX__)
    #define TUW_CONSTANTS_OS "linux"
    const int GRID_COMP_XSPACE = 4;
    const int GRID_MAIN_SPACE = 12;
    const int BOX_MAIN_SPACE = 12;
    const int BOX_SUB_SPACE = 4;
    const int BOX_CHECKS_SPACE = 0;
    const int BTN_WIDTH = 84;
    const int BTN_HEIGHT = -1;
#else
    #define TUW_CONSTANTS_OS "mac"
    const int GRID_COMP_XSPACE = 4;
    const int GRID_MAIN_SPACE = 12;
    const int BOX_MAIN_SPACE = 12;
    const int BOX_SUB_SPACE = 4;
    const int BOX_CHECKS_SPACE = 4;
    const int BTN_WIDTH = 84;
    const int BTN_HEIGHT = 0;
#endif
}  // namespace tuw_constants
