#pragma once
#include <vector>
#include <string>
#include <wx/wx.h>
#include <wx/process.h>
#include <iostream>

//run command and return error messages
std::vector<wxString> exec(const char* cmd);