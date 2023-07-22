#include "exec.h"

#ifdef __linux__
// Console window for linux
LogFrame::LogFrame(wxString exepath) : wxFrame(nullptr, wxID_ANY, exepath,
    wxDefaultPosition, wxSize(600, 400),
    wxSYSTEM_MENU |
    wxRESIZE_BORDER |
    wxMINIMIZE_BOX |
    wxMAXIMIZE_BOX |
    wxCAPTION |
    wxCLIP_CHILDREN) {

    m_log_box = new wxTextCtrl(this, wxID_ANY,
        "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    m_log_box->SetBackgroundColour(*wxBLACK);
    m_log_box->SetForegroundColour(*wxWHITE);
    wxFont font = m_log_box->GetFont();
    font.SetPointSize(font.GetPointSize() + 1);
    m_log_box->SetFont(font);
    m_log_redirector = new wxStreamToTextRedirector(m_log_box);
    Centre();
    wxPoint pos = GetPosition();
    SetPosition(wxPoint(pos.x-300, pos.y));
    Show();
}

void LogFrame::OnClose(wxCloseEvent& event) {
    Destroy();
}
#endif

class wxProcessExecute : public wxProcess {
 private:
    bool m_running = false;
    int m_exit_code;

 public:
    explicit wxProcessExecute(int flags) : wxProcess(flags), m_exit_code(1), m_running(false) {}

    long Execute(const wxString cmd) {
        long pid = wxExecute(cmd, wxEXEC_ASYNC, this);
        if (pid) {
            m_running = true;
        }
        return pid;
    }

    void OnTerminate(int pid, int status) {
        m_exit_code = status;
        m_running = false;
        if (wxProcessExecute::Exists(pid)) {
            wxProcessExecute::Kill(pid);
        }
    }

    static wxProcessExecute* Open(const wxString& cmd) {
        wxProcessExecute* process = new wxProcessExecute(wxPROCESS_REDIRECT);
        long pid = process->Execute(cmd);
        if (!pid) {
            delete process;
            return NULL;
        }
        process->SetPid(pid);
        return process;
    }

    int GetExitCode() { return m_exit_code; }
    void Wait() { while (m_running) { wxYield(); } }
};

inline bool IsReturn(const char& input) {
    return input == '\n' || input == '\r';
}

std::string GetLastLine(const std::string& input) {
    if (input.length() <= 2) return input;
    size_t position = input.length() - 3;
    while ((!IsReturn(input[position])) && position > 0) position--;
    if (IsReturn(input[position])) position += 1;
    return input.substr(position);
}

// get string from stream
std::string ReadStream(wxInputStream* stream, char* buf, size_t size) {
    if (stream->CanRead()) {
        size_t read_size;
        stream->Read(buf, size);
        read_size = stream->LastRead();
        return std::string(buf, read_size);
    }
    return "";
}

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
                 wxString& last_line) {
    // open process
    wxProcessExecute* process = wxProcessExecute::Open(cmd);
    if (!process)
        return { false, "Failed to open a process." };

    // get stream
    wxInputStream* istream = process->GetInputStream();
    wxInputStream* estream = process->GetErrorStream();

    // set buffers
    size_t size = 512;
    char ibuf[512];  // buffer for output
    char ebuf[512];  // buffer for error messages
    std::string in_msg = "";
    std::string err_msg = "";

    while ((!istream->Eof() && !estream->Eof()) ||
        istream->CanRead() || estream->CanRead()) {  // while process is running
        // print outputs
        std::string str = ReadStream(istream, ibuf, size);
    #ifdef __linux__
        ostream << str;
    #else
        wxPrintf(str.c_str());
    #endif
        in_msg += str;
        if (in_msg.length() > 2048) {
            in_msg = in_msg.substr(in_msg.length() - 1024, 1024);
        }

        // store error messages
        err_msg += ReadStream(estream, ebuf, size);
        if (err_msg.length() > 4096) {
            err_msg = err_msg.substr(err_msg.length() - 2048, 2048);
        }
    }
    // get last line
    last_line = GetLastLine(in_msg).c_str();

    process->Wait();

    // print and return error messages
    if (err_msg != "")
        return { false, err_msg.c_str() };

    int exit_code = process->GetExitCode();
    if (check_exit_code && (exit_code != exit_success)) {
        if (show_last_line)
            return { false, last_line };
        else
            return { false, wxString::Format("Invalid exit code (%d)", exit_code) };
    }

    return { true };
}
