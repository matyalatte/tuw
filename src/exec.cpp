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
 public:
    explicit wxProcessExecute(int flags) : wxProcess(flags) {}

    long Execute(const wxString cmd) {
        return wxExecute(cmd, wxEXEC_ASYNC, this);
    }

    virtual void OnTerminate(int pid, int status) {
        if (wxProcessExecute::Exists(pid)) {
            wxProcessExecute::Kill(pid);
        }
    }

    static wxProcessExecute* Open(const wxString& cmd, int flags = wxEXEC_ASYNC) {
        wxASSERT_MSG(!(flags & wxEXEC_SYNC), wxT("wxEXEC_SYNC should not be used."));
        wxProcessExecute* process = new wxProcessExecute(wxPROCESS_REDIRECT);
        long pid = wxExecute(cmd, flags, process);
        if (!pid) {
            delete process;
            return NULL;
        }
        process->SetPid(pid);
        return process;
    }
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
std::array<std::string, 2> Exec(LogFrame& ostream, wxString& cmd) {
#else
std::array<std::string, 2> Exec(std::ostream& ostream, wxString& cmd) {
#endif
    // open process
    wxProcessExecute* process = wxProcessExecute::Open(cmd);
    if (!process) {
        return { "", "Failed to open a process." };
    }

    // get stream
    wxInputStream* istream = process->GetInputStream();
    wxInputStream* estream = process->GetErrorStream();

    // set buffers
    size_t size = 512;
    char ibuf[512];  // buffer for output
    char ebuf[512];  // buffer for error messages
    std::string str;
    std::string in_msg = "";
    std::string err_msg = "";

    while ((!istream->Eof() && !estream->Eof()) ||
        istream->CanRead() || estream->CanRead()) {  // while process is running
        // print outputs
        str = ReadStream(istream, ibuf, size);
        ostream << str;
        in_msg += str;

        // store error messages
        err_msg += ReadStream(estream, ebuf, size);
    }
    // get last line
    in_msg = GetLastLine(in_msg);

    // print and return error messages
    ostream << err_msg;
    return {in_msg, err_msg};
}
