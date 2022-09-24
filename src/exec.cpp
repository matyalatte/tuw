#include "exec.h"

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

std::string LastLine(const std::string& input) {
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
std::array<std::string, 2> Exec(const char* cmd) {
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
        std::cout << str;
        in_msg += str;

        // store error messages
        err_msg += ReadStream(estream, ebuf, size);
    }
    // get last line
    in_msg = LastLine(in_msg);

    // print and return error messages
    std::cout << err_msg;
    return {in_msg, err_msg};
}
