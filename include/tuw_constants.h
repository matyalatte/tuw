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
    constexpr char VERSION[] = "0.6.0";
    constexpr int VERSION_INT = 600;

#ifdef _WIN32
    constexpr char OS[] = "win";
    const int GRID_XSPACE = 2;
    const int GRID_YSPACE = 0;
    const int BOX_MAIN_SPACE = 6;
    const int BOX_SUB_SPACE = 1;
    const int BOX_CHECKS_SPACE = 0;
#elif defined(__linux__)
    constexpr char OS[] = "linux";
    const int GRID_XSPACE = 4;
    const int GRID_YSPACE = 0;
    const int BOX_MAIN_SPACE = 12;
    const int BOX_SUB_SPACE = 4;
    const int BOX_CHECKS_SPACE = 0;
#else
    constexpr char OS[] = "mac";
    const int GRID_XSPACE = 3;
    const int GRID_YSPACE = 0;
    const int BOX_MAIN_SPACE = 9;
    const int BOX_SUB_SPACE = 3;
    const int BOX_CHECKS_SPACE = 3;
#endif
}  // namespace tuw_constants
