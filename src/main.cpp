#include "main_frame.h"

// Main
class MainApp : public wxApp {
 private:
    MainFrame* m_frame;
 public:
    MainApp(): wxApp() {
        m_frame = nullptr;
    }
    bool OnInit() override;
};

bool MainApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    // make main window
    m_frame = new MainFrame();
    m_frame->Show();

    return true;
}

wxDECLARE_APP(MainApp);
wxIMPLEMENT_APP(MainApp);

#ifdef _WIN32
// no need for unix
int main(int argc, char* argv[]) {
    return wxEntry(argc, argv);
}
#endif
