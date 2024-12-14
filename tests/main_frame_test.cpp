// Tests for main_frame.cpp
// Todo: Write more tests

#include "test_utils.h"

class MainFrameTest : public ::testing::Test {
 protected:
    MainFrame* main_frame;

    virtual void SetUp() {
        uiInitOptions options;

        memset(&options, 0, sizeof (uiInitOptions));
        const char* msg = uiInit(&options);
        if (msg != NULL)
            printf("%s\n", msg);
        EXPECT_TRUE(msg == NULL);
        EXPECT_EQ(noex::OK, noex::get_error_no());
        uiMainSteps();
    }

    virtual void TearDown() {
        main_frame->Close();
        uiUninit();
    #ifdef __TUW_UNIX__
        // Need to reset the pointer to the log.
        SetLogEntry(NULL);
    #endif
        EXPECT_EQ(noex::OK, noex::get_error_no());
    }

    void TestConfig(rapidjson::Document& test_json, noex::string config) {
        rapidjson::Document test_config;
        json_utils::JsonResult result = json_utils::LoadJson(config, test_config);
        EXPECT_TRUE(result.ok);
        main_frame = new MainFrame(test_json, test_config);
        main_frame->SaveConfig();
        rapidjson::Document saved_config;
        result = json_utils::LoadJson("gui_config.json", saved_config);
        EXPECT_TRUE(result.ok);
        EXPECT_EQ(test_config, saved_config);
    }
};

TEST_F(MainFrameTest, MakeDefaultMainFrame) {
    main_frame = new MainFrame();
    rapidjson::Document json1;
    rapidjson::Document json2;
    json_utils::GetDefaultDefinition(json1);
    main_frame->GetDefinition(json2);
    EXPECT_EQ(json1, json2);
}

void GetDummyConfig(rapidjson::Document& dummy_config) {
    dummy_config.SetObject();
    dummy_config.AddMember("test", 0, dummy_config.GetAllocator());
}

TEST_F(MainFrameTest, InvalidDefinition) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][4]["default"].SetString("number");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    json_utils::GetDefaultDefinition(test_json);
    rapidjson::Document actual_json;
    main_frame->GetDefinition(actual_json);
    EXPECT_EQ(test_json, actual_json);
}

TEST_F(MainFrameTest, InvalidHelp) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["help"][0]["url"].SetInt(1);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->GetDefinition(test_json);
    EXPECT_FALSE(test_json.HasMember("help"));
}

TEST_F(MainFrameTest, GetCommand1) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("command!");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_STREQ("command!", main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, GetCommand2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    noex::string expected = "echo file: \"test.txt\" & echo folder: \"testdir\"";
    expected += " & echo combo: value3 & echo radio: value3 & echo check: flag!";
    expected += " & echo check_array:  --f2 & echo textbox: remove this text!";
    expected += " & echo int: 10 & echo float: 0.01";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, GetCommand3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    noex::string expected = "echo file:  & echo folder:  & echo combo: value1 & echo radio: value1";
    expected += " & echo check:  & echo check_array:  & echo textbox: ";
    expected += " & echo int: 0 & echo float: 0.0";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, RunCommandSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);

    rapidjson::Document actual_json;
    main_frame->GetDefinition(actual_json);
    ASSERT_EQ(test_json["help"], actual_json["help"]);

    noex::string cmd = main_frame->GetCommand();
    ExecuteResult result = Execute(cmd);
    EXPECT_EQ(0, result.exit_code);
    EXPECT_STREQ("", result.err_msg.c_str());
}

TEST_F(MainFrameTest, RunCommandFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("I'll fail");
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);

    noex::string cmd = main_frame->GetCommand();
    ExecuteResult result = Execute(cmd);
    EXPECT_NE(0, result.exit_code);
    EXPECT_STRNE("", result.err_msg.c_str());
}

TEST_F(MainFrameTest, UpdateFrame) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->UpdatePanel(1);
    main_frame->Fit();
    main_frame->UpdatePanel(2);
    main_frame->Fit();
}

TEST_F(MainFrameTest, RunCommandShowLast) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Document dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->UpdatePanel(2);

    noex::string cmd = main_frame->GetCommand();
    ExecuteResult result = Execute(cmd);
    EXPECT_EQ(0, result.exit_code);
    EXPECT_STREQ("", result.err_msg.c_str());
    EXPECT_STREQ("sample message!", result.last_line.c_str());
}

TEST_F(MainFrameTest, LoadSaveConfigAscii) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    TestConfig(test_json, JSON_CONFIG_ASCII);
}

TEST_F(MainFrameTest, LoadSaveConfigUTF) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    noex::string cmd = test_json["gui"][0]["command"].GetString();
    memcpy(cmd.data() + 12, "ファイル", 4);
    test_json["gui"][0]["command"].SetString(rapidjson::StringRef(cmd.c_str()));
    test_json["gui"][0]["components"][1]["id"].SetString("ファイル");
    TestConfig(test_json, JSON_CONFIG_UTF);
}
