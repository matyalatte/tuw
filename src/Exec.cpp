#pragma once
#include "Exec.h"
#include <iostream>
#include <wx/process.h>

//run command and return error messages
std::string exec(const char* cmd) {

    //open process
    wxProcess* process = wxProcess::Open(cmd);
    if (!process) {
        return "__null__";
    }
    wxInputStream* istream = process->GetInputStream();
    wxInputStream* estream = process->GetErrorStream();
    long pid = process->GetPid();

    size_t size = 512;
    size_t read_size;
    char ibuf[512]; //buffer for output
    char ebuf[512]; //buffer for error messages

    std::string err_msg="";
    std::cout << std::endl;

    while (wxProcess::Exists(pid)) {//while process is running
        if (istream->CanRead()) {
            //print outputs
            istream->Read(ibuf, size);
            read_size = istream->LastRead();
            std::cout << std::string(ibuf, read_size);
        }
        if (estream->CanRead()) {
            //store error messages
            estream->Read(ebuf, size);
            read_size = estream->LastRead();
            err_msg += std::string(ebuf, read_size);
        }
    }
    //print and return error messages
    std::cout << err_msg;
    return err_msg;
}