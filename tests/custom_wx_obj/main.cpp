// Tests for custom_wx_obj.cpp
// Todo: Write more tests

#include <gtest/gtest.h>
#include "wx/app.h"
#include "custom_wx_obj.h"

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 1);

    // Make dummy app
    wxApp* app = new wxApp();
    wxApp::SetInstance(app);

    // Initialize app
    wxEntryStart(argc, argv);
    app->OnInit();

    return RUN_ALL_TESTS();
}

class TestFrame: public wxFrame{
 public:
    CustomFilePicker* m_file_picker;
    CustomDirPicker* m_dir_picker;
    TestFrame(): wxFrame(nullptr, wxID_ANY, "TestFrame") {
        m_file_picker = new CustomFilePicker(this, wxID_ANY, "", "", "(*)|*", "Empty File");
        m_dir_picker = new CustomDirPicker(this, wxID_ANY, "", "", "Empty Dir");
        this->Show();
    }
};

template <typename Picker>
std::string GetText(Picker* picker) {
    return std::string(picker->GetTextCtrl()->GetValue().c_str());
}

template <typename Picker>
std::string GetActualText(Picker* picker) {
    return std::string(picker->GetTextCtrlValue().c_str());
}

bool Wait(long msec, wxWindow* frame) {
    wxMilliSleep(msec);
    return wxSafeYield(frame);
}

TEST(CustomWxObjTest, ShowEmptyMessage) {
    TestFrame* frame = new TestFrame();
    Wait(30, frame);
    frame->m_file_picker->GetTextCtrl()->SetFocus();
    Wait(30, frame);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "Empty Dir");
    EXPECT_STREQ(GetActualText(frame->m_dir_picker).c_str(), "");
    frame->m_dir_picker->GetTextCtrl()->SetFocus();
    Wait(30, frame);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "Empty File");
    EXPECT_STREQ(GetActualText(frame->m_file_picker).c_str(), "");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "");
}

TEST(CustomWxObjTest, HideEmptyMessage) {
    TestFrame* frame = new TestFrame();
    frame->m_file_picker->GetTextCtrl()->SetFocus();
    Wait(30, frame);
    static_cast<CustomTextCtrl*>(frame->m_file_picker->GetTextCtrl())->UpdateText("test");
    Wait(30, frame);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "test");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "Empty Dir");
    frame->m_dir_picker->GetTextCtrl()->SetFocus();
    Wait(30, frame);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "test");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "");
}
