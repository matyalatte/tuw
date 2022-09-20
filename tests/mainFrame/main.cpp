// Tests for MainFrame.cpp
// Todo: Write more tests

#include <gtest/gtest.h>
#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/app.h>
#include <wx/modalhook.h>
#include "MainFrame.h"

char const * json;

class MyModalDialogHook : public wxModalDialogHook
{
protected:
    virtual int Enter(wxDialog* dialog)
    {
        if ( wxDynamicCast(dialog, wxMessageDialog) )
        {
            return wxID_CANCEL;
        }
        return wxID_NONE;
    }
    virtual void Exit(wxDialog* dialog){}
};

int main (int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 2);

    json = argv[1];

    wxApp* app = new wxApp();
    return RUN_ALL_TESTS();
}

TEST(MainFrameTest, MakeDefaultMainFrame) {
    MyModalDialogHook hook;
    hook.Register();
    MainFrame* mainFrame = new MainFrame();
    EXPECT_EQ(jsonUtils::default_definition(), mainFrame->GetDefinition()["gui"][0]);
}

TEST(MainFrameTest, RunCommand) {
    nlohmann::json test_json = jsonUtils::loadJson(json);
    ASSERT_FALSE(nlohmann::json({}) == test_json);
    MyModalDialogHook hook;
    hook.Register();
    MainFrame* mainFrame = new MainFrame(nlohmann::json(test_json));
    ASSERT_EQ(test_json["help"], mainFrame->GetDefinition()["help"]);
    ASSERT_EQ(test_json["gui"][0]["components"], mainFrame->GetDefinition()["gui"][0]["components"]);
    std::array<std::string, 2> msg = mainFrame->RunCommand();
    EXPECT_TRUE(msg[0] != "");
    EXPECT_TRUE(msg[1] == "");
}
