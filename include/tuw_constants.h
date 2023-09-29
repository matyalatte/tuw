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
#elif defined(__linux__)
    constexpr char OS[] = "linux";
#else
    constexpr char OS[] = "mac";
#endif
}  // namespace tuw_constants
