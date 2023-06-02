#pragma once
#include <iostream>
#include <array>
#include <string>
#include "wx/wx.h"
#include "wx/process.h"

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
    LogFrame& operator <<(std::string& str) {
        m_log_box->AppendText(wxString::FromUTF8(str));
        return *this;
    }
    LogFrame& operator <<(wxString& str) {
        m_log_box->AppendText(str);
        return *this;
    }
    LogFrame& operator <<(const char* str) {
        m_log_box->AppendText(wxString::FromUTF8(str));
        return *this;
    }
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);
    LogFrame& operator <<(StandardEndLine endl) {
        *this << "\n";
        return *this;
    }
};
#endif


// run command and return error messages
#ifdef __linux__
std::string Exec(LogFrame& ostream,
#else
std::string Exec(std::ostream& ostream,
#endif
                 wxString& cmd,
                 bool check_exit_code = false,
                 int exit_success = 0,
                 bool show_last_line = false);
