#pragma once
#include <iostream>
#include <array>
#include <string>
#include "wx/wx.h"
#include "wx/process.h"

// run command and return error messages
std::array<std::string, 2> Exec(const char* cmd);
