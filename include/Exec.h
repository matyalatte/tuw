#pragma once
#include <array>
#include <string>
#include <wx/wx.h>
#include <wx/process.h>
#include <iostream>

//run command and return error messages
std::array<std::string, 2> exec(const char* cmd);