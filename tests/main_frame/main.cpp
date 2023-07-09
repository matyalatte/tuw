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
    rapidjson::Document json1;
    rapidjson::Document json2;
    json_utils::GetDefaultDefinition(json1);
    main_frame->GetDefinition(json2);
    EXPECT_EQ(json1, json2);
}

void GetTestJson(rapidjson::Document& test_json) {
    json_utils::LoadJson(json_file, test_json);
    EXPECT_FALSE(test_json.ObjectEmpty());
}

void GetDummyConfig(rapidjson::Document& dummy_config) {
    dummy_config.SetObject();
    dummy_config.AddMember("test", 0, dummy_config.GetAllocator());
}

TEST(MainFrameTest, InvalidDefinition) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][4]["default"].SetString("number");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    json_utils::GetDefaultDefinition(test_json);
    rapidjson::Document actual_json;
    main_frame->GetDefinition(actual_json);
    EXPECT_EQ(test_json, actual_json);
}

TEST(MainFrameTest, InvalidHelp) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["help"][0]["url"].SetInt(1);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    main_frame->GetDefinition(test_json);
    EXPECT_FALSE(test_json.HasMember("help"));
}

TEST(MainFrameTest, GetCommand1) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("command!");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_STREQ("command!", main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, GetCommand2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    std::string expected = "echo file: \"test.txt\" & echo folder: \"testdir\"";
    expected += " & echo choice: value3 & echo check: flag!";
    expected += " & echo check_array:  --f2 & echo textbox: remove this text!";
    expected += " & echo int: 10 & echo float: 0.01";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, GetCommand3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    std::string expected = "echo file:  & echo folder:  & echo choice: value1";
    expected += " & echo check:  & echo check_array:  & echo textbox: ";
    expected += " & echo int: 0 & echo float: 0.0";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().ToUTF8());
}

TEST(MainFrameTest, RunCommandSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);

    rapidjson::Document actual_json;
    main_frame->GetDefinition(actual_json);
    ASSERT_EQ(test_json["help"], actual_json["help"]);

    std::string last_line = main_frame->RunCommand();
    EXPECT_STRNE("", last_line.c_str());
}

TEST(MainFrameTest, RunCommandFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("I'll fail");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
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
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent();
    main_frame->ClickButton(event);
}

TEST(MainFrameTest, UpdateFrame) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent(wxEVT_NULL, wxID_HIGHEST + 2);
    main_frame->UpdateFrame(event);
}

TEST(MainFrameTest, ClickRunButtonShowLast) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCommandEvent event = wxCommandEvent(wxEVT_NULL, wxID_HIGHEST + 3);
    main_frame->UpdateFrame(event);
    main_frame->ClickButton(event);
}

TEST(MainFrameTest, DeleteFrame) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    MainFrame* main_frame = new MainFrame(test_json, dummy_config);
    wxCloseEvent event = wxCloseEvent();
    main_frame->OnClose(event);
}

void TestConfig(rapidjson::Document& test_json, std::string config) {
    rapidjson::Document test_config;
    json_utils::LoadJson(config, test_config);
    MainFrame* main_frame = new MainFrame(test_json, test_config);
    main_frame->SaveConfig();
    rapidjson::Document saved_config;
    json_utils::LoadJson("gui_config.json", saved_config);
    EXPECT_EQ(test_config, saved_config);
}

TEST(MainFrameTest, LoadSaveConfigAscii) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    TestConfig(test_json, config_ascii);
}

TEST(MainFrameTest, LoadSaveConfigUTF) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    std::string cmd = test_json["gui"][0]["command"].GetString();
    cmd.replace(12, 4, "ファイル");
    test_json["gui"][0]["command"].SetString(rapidjson::StringRef(cmd.c_str()));
    test_json["gui"][0]["components"][1]["id"].SetString("ファイル");
    TestConfig(test_json, config_utf);
}
