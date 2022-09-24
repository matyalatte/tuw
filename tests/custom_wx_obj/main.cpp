// Tests for MainFrame.cpp
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
    return std::string(picker->GetTextCtrl()->GetValue());
}

template <typename Picker>
std::string GetActualText(Picker* picker) {
    return std::string(picker->GetTextCtrlValue());
}

template <typename Picker>
void Focus(Picker* picker) {
    picker->GetTextCtrl()->SetFocus();
    wxYield();
}

TEST(CustomWxObjTest, ShowEmptyMessage) {
    TestFrame* frame = new TestFrame();
    wxYield();
    Focus(frame->m_file_picker);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "Empty Dir");
    EXPECT_STREQ(GetActualText(frame->m_dir_picker).c_str(), "");
    Focus(frame->m_dir_picker);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "Empty File");
    EXPECT_STREQ(GetActualText(frame->m_file_picker).c_str(), "");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "");
}

TEST(CustomWxObjTest, HideEmptyMessage) {
    TestFrame* frame = new TestFrame();
    wxYield();
    Focus(frame->m_file_picker);
    reinterpret_cast<CustomTextCtrl*>(frame->m_file_picker->GetTextCtrl())->UpdateText("test");
    wxYield();
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "test");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "Empty Dir");
    Focus(frame->m_dir_picker);
    EXPECT_STREQ(GetText(frame->m_file_picker).c_str(), "test");
    EXPECT_STREQ(GetText(frame->m_dir_picker).c_str(), "");
}
