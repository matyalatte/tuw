// Tests for main_frame.cpp
// Todo: Write more tests

#include <gtest/gtest.h>
#include "wx/app.h"
#include "wx/modalhook.h"
#include "main_frame.h"

char const * json_file;
char const * config_ascii;
char const * config_utf;

// Hook to skip message dialogues
class DialogSkipper : public wxModalDialogHook {
 protected:
    virtual int Enter(wxDialog* dialog) {
        if ( wxDynamicCast(dialog, wxMessageDialog) ) {
            return wxID_CANCEL;
        }
        return wxID_NONE;
    }
    virtual void Exit(wxDialog* dialog){}
};

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 4);

    json_file = argv[1];
    config_ascii = argv[2];
    config_utf = argv[3];

    // Make dummy app
    wxApp* app = new wxApp();

    // Initialize app
    int argc_ = 1;
    char *argv_[1] = { argv[0] };
    wxEntryStart(argc_, argv_);
    app->OnInit();

    // Make hook to skip message dialogues
    DialogSkipper* hook = new DialogSkipper();
    hook->Register();

    return RUN_ALL_TESTS();
}

TEST(MainFrameTest, MakeDefaultMainFrame) {
    MainFrame* main_frame = new MainFrame();
    EXPECT_EQ(json_utils::GetDefaultDefinition(), main_frame->GetDefinition()["gui"][0]);
}

nlohmann::json GetTestJson() {
    nlohmann::json test_json = json_utils::LoadJson(json_file);
    EXPECT_NE(nlohmann::json({}), test_json);
    return test_json;
}

nlohmann::json dummy_config = {{"dummy", 0}};

TEST(MainFrameTest, InvalidDefinition) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][1]["components"][4]["default"] = "number";
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_EQ(json_utils::GetDefaultDefinition(), main_frame->GetDefinition()["gui"][0]);
    EXPECT_TRUE(main_frame->GetDefinition().contains("help"));
}

TEST(MainFrameTest, InvalidHelp) {
    nlohmann::json test_json = GetTestJson();
    test_json["help"][0]["url"] = 1;
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_FALSE(main_frame->GetDefinition().contains("help"));
    EXPECT_EQ(test_json["gui"][0]["components"],
        main_frame->GetDefinition()["gui"][0]["components"]);
}

TEST(MainFrameTest, GetCommand1) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = json_utils::GetDefaultDefinition();
    test_json["gui"][0]["command"] = "command!";
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_STREQ("command!", main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, GetCommand2) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = test_json["gui"][1];
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    std::string expected = "echo file: \"test.txt\" & echo folder: \"testdir\"";
    expected += " & echo choice: value3 & echo check: flag!";
    expected += " & echo check_array:  --f3 & echo textbox: remove this text!";
    expected += " & echo int: 10 & echo float: 0.01";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, GetCommand3) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    std::string expected = "echo file:  & echo folder:  & echo choice: value1";
    expected += " & echo check:  & echo check_array:  & echo textbox: ";
    expected += " & echo int: 0 & echo float: 0.0";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, RunCommandSuccess) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);

    // The json file should not be fixed by app.
    ASSERT_EQ(test_json["help"], main_frame->GetDefinition()["help"]);
    ASSERT_EQ(test_json["gui"][0]["components"],
        main_frame->GetDefinition()["gui"][0]["components"]);

    std::array<std::string, 2> msg = main_frame->RunCommand();
    EXPECT_STRNE("", msg[0].c_str());
    EXPECT_STREQ("", msg[1].c_str());
}

TEST(MainFrameTest, RunCommandFail) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = json_utils::GetDefaultDefinition();
    test_json["gui"][0]["command"] = "I'll fail";
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);

    std::array<std::string, 2> msg = main_frame->RunCommand();
    EXPECT_STREQ("", msg[0].c_str());
    EXPECT_STRNE("", msg[1].c_str());
}

TEST(MainFrameTest, ClickRunButton) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent();
    main_frame->ClickButton(event);
}

TEST(MainFrameTest, UpdateFrame) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent(wxEVT_NULL, wxID_HIGHEST + 2);
    main_frame->UpdateFrame(event);
}

TEST(MainFrameTest, ClickRunButtonShowLast) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent(wxEVT_NULL, wxID_HIGHEST + 3);
    main_frame->UpdateFrame(event);
    main_frame->ClickButton(event);
}

TEST(MainFrameTest, DeleteFrame) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCloseEvent event = wxCloseEvent();
    main_frame->OnClose(event);
}

void TestConfig(nlohmann::json test_json, std::string config) {
    nlohmann::json test_config = json_utils::LoadJson(config);
    MainFrame* main_frame = new MainFrame(test_json, test_config);
    main_frame->SaveConfig();
    nlohmann::json saved_config = json_utils::LoadJson("gui_config.json");
    EXPECT_EQ(test_config, saved_config);
}

TEST(MainFrameTest, LoadSaveConfigAscii) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = test_json["gui"][1];
    TestConfig(test_json, config_ascii);
}

TEST(MainFrameTest, LoadSaveConfigUTF) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = test_json["gui"][1];
    test_json["gui"][0]["components"][1]["id"] = "ファイル";
    TestConfig(test_json, config_utf);
}
