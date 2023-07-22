#pragma once
#include <cstdio>
#include <stdexcept>
#include <array>
#include <string>
#include "wx/wx.h"
#include "wx/process.h"
#include "wx_utils.h"

#ifdef __linux__
// A text window to show outputs.
// wxStreamToTextRedirector won't work because it can't handle utf-8 characters.
class LogFrame : public wxFrame {
 private:
    wxTextCtrl* m_log_box;
    wxStreamToTextRedirector* m_log_redirector;

 public:
    explicit LogFrame(wxString exepath);
    void OnClose(wxCloseEvent& event);
    LogFrame& operator <<(const std::string& str) {
        m_log_box->AppendText(wxString::FromUTF8(str.c_str()));
        return *this;
    }
    LogFrame& operator <<(const wxString& str) {
        m_log_box->AppendText(str);
        return *this;
    }
    LogFrame& operator <<(const char* str) {
        m_log_box->AppendText(wxString::FromUTF8(str));
        return *this;
    }
};
#endif


// run command and return error messages
#ifdef __linux__
wxResult Exec(LogFrame& ostream,
#else
wxResult Exec(
#endif
                 wxString& cmd,
                 bool check_exit_code,
                 int exit_success,
                 bool show_last_line,
                 wxString& last_line);
