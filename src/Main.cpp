#pragma once
#include "MainFrame.h"

//Main
class MainApp : public wxApp
{
    virtual bool OnInit() override;
};

bool MainApp::OnInit()
{

    if (!wxApp::OnInit())
        return false;

    //make main window
    MainFrame* frame = new MainFrame;
    frame->Show();

    return true;
}

wxDECLARE_APP(MainApp);
wxIMPLEMENT_APP(MainApp);

int main(int argc, char* argv[]) {
    return wxEntry(argc, argv);
}