#pragma once
namespace scr_constants {
    constexpr char TOOL_NAME[] = "Simple Command Runner";
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
}  // namespace scr_constants
