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
    std::cout << test_json["gui"][1]["components"][4] << std::endl;
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_EQ(json_utils::GetDefaultDefinition(), main_frame->GetDefinition()["gui"][0]);
    EXPECT_TRUE(main_frame->GetDefinition().contains("help"));
}

TEST(MainFrameTest, InvalidHelp) {
    nlohmann::json test_json = GetTestJson();
    test_json["help"][0]["url"] = 1;
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_FALSE(main_frame->GetDefinition().contains("help"));
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
    expected += " & echo check_array:  --f2 & echo textbox: remove this text!";
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

    ASSERT_EQ(test_json["help"], main_frame->GetDefinition()["help"]);

    std::string last_line = main_frame->RunCommand();
    EXPECT_STRNE("", last_line.c_str());
}

TEST(MainFrameTest, RunCommandFail) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = json_utils::GetDefaultDefinition();
    test_json["gui"][0]["command"] = "I'll fail";
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);

    try {
        main_frame->RunCommand();
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STRNE("", err.what());
    }
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
    std::string cmd = test_json["gui"][0]["command"].get<std::string>();
    cmd.replace(12, 4, "ファイル");
    test_json["gui"][0]["command"] = cmd;
    test_json["gui"][0]["components"][1]["id"] = "ファイル";
    TestConfig(test_json, config_utf);
}
