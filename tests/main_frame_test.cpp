// Tests for main_frame.cpp
// Todo: Write more tests

#include "test_utils.h"
#include "process_utils.h"

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

    void TestConfig(tuwjson::Value& test_json, noex::string config) {
        tuwjson::Value test_config;
        noex::string err = json_utils::LoadJson(config, test_config);
        EXPECT_TRUE(err.empty());
        main_frame = new MainFrame(test_json, test_config);
        main_frame->SaveConfig();
        tuwjson::Value saved_config;
        err = json_utils::LoadJson("gui_config.json", saved_config);
        EXPECT_TRUE(err.empty());
        EXPECT_EQ(test_config, saved_config);
    }
};

TEST_F(MainFrameTest, MakeDefaultMainFrame) {
    main_frame = new MainFrame();
    tuwjson::Value json1;
    tuwjson::Value json2;
    json_utils::GetDefaultDefinition(json1);
    main_frame->GetDefinition(json2);
    EXPECT_EQ(json1, json2);
}

void GetDummyConfig(tuwjson::Value& dummy_config) {
    dummy_config.SetObject();
    dummy_config["test"].SetInt(0);
}

TEST_F(MainFrameTest, InvalidDefinition) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][4]["default"].SetString("number");
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    json_utils::GetDefaultDefinition(test_json);
    tuwjson::Value actual_json;
    main_frame->GetDefinition(actual_json);
    EXPECT_EQ(test_json, actual_json);
}

TEST_F(MainFrameTest, InvalidHelp) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["help"][0]["url"].SetInt(1);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->GetDefinition(test_json);
    EXPECT_FALSE(test_json.HasMember("help"));
}

TEST_F(MainFrameTest, GetCommand1) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("command!");
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    EXPECT_STREQ("command!", main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, GetCommand2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    noex::string expected = "echo file: \"test.txt\" & echo folder: \"testdir\"";
    expected += " & echo combo: value3 & echo radio: value3 & echo check: flag!";
    expected += " & echo check_array:  --f2 & echo textbox: remove this text!";
    expected += " & echo int: 10 & echo float: 0.01";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, GetCommand3) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    noex::string expected = "echo file:  & echo folder:  & echo combo: value1 & echo radio: value1";
    expected += " & echo check:  & echo check_array:  & echo textbox: ";
    expected += " & echo int: 0 & echo float: 0.0";
    EXPECT_STREQ(expected.c_str(), main_frame->GetCommand().c_str());
}

TEST_F(MainFrameTest, RunCommandSuccess) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);

    tuwjson::Value actual_json;
    main_frame->GetDefinition(actual_json);
    ASSERT_EQ(test_json["help"], actual_json["help"]);

    noex::string cmd = main_frame->GetCommand();
    ExecuteResult result = Execute(cmd);
    EXPECT_EQ(0, result.exit_code);
    EXPECT_STREQ("", result.err_msg.c_str());
}

TEST_F(MainFrameTest, RunCommandFail) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    json_utils::GetDefaultDefinition(test_json);
    test_json["gui"][0]["command"].SetString("I'll fail");
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);

    noex::string cmd = main_frame->GetCommand();
    ExecuteResult result = Execute(cmd);
    EXPECT_NE(0, result.exit_code);
    EXPECT_STRNE("", result.err_msg.c_str());
}

TEST_F(MainFrameTest, UpdateFrame) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->UpdatePanel(1);
    main_frame->Fit();
    main_frame->UpdatePanel(2);
    main_frame->Fit();
}

TEST_F(MainFrameTest, RunCommandShowLast) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    tuwjson::Value dummy_config;
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
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    TestConfig(test_json, JSON_CONFIG_ASCII);
}

TEST_F(MainFrameTest, LoadSaveConfigUTF) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    noex::string cmd = test_json["gui"][0]["command"].GetString();
    memcpy(cmd.data() + 12, "ファイル", 4);
    test_json["gui"][0]["command"].SetString(cmd);
    test_json["gui"][0]["components"][1]["id"].SetString("ファイル");
    TestConfig(test_json, JSON_CONFIG_UTF);
}

TEST_F(MainFrameTest, CrossPlatform) {
    tuwjson::Value test_json;
    GetTestJson(test_json, JSON_CROSS_PLATFORM);
    tuwjson::Value dummy_config;
    GetDummyConfig(dummy_config);
    main_frame = new MainFrame(test_json, dummy_config);
    main_frame->UpdatePanel(1);
}

class OpenURLTest : public MainFrameTest {
 protected:
    tuwjson::Value definition;
    tuwjson::Value config;

    void SetUp() override {
        MainFrameTest::SetUp();
        GetTestJson(definition);
        GetDummyConfig(config);
    }

    void ReplaceURL(const char* url) {
        definition["help"][0]["url"].SetString(url);
    }
};

#if USE_BROWSER
TEST_F(OpenURLTest, OpenUrl) {
    main_frame = new MainFrame(definition, config);
    noex::string msg = main_frame->OpenURLBase(0);
    EXPECT_STREQ(msg.c_str(), "");
}
#endif  // USE_BROWSER

TEST_F(OpenURLTest, OpenUrlWithFilePath) {
    ReplaceURL("file:///test.txt");
    main_frame = new MainFrame(definition, config);
    noex::string msg = main_frame->OpenURLBase(0);
    const char* expected =
        "Use 'file' type for a path, not 'url' type. (file:///test.txt)";
    EXPECT_STREQ(msg.c_str(), expected);
}

TEST_F(OpenURLTest, OpenUrlWithFtps) {
    ReplaceURL("ftps://example.com");
    main_frame = new MainFrame(definition, config);
    noex::string msg = main_frame->OpenURLBase(0);
    const char* expected =
        "Unsupported scheme detected. "
        "It should be http or https. (ftps)";
    EXPECT_STREQ(msg.c_str(), expected);
}

TEST_F(OpenURLTest, OpenUrlWithSpace) {
    ReplaceURL("https://example .com");
    main_frame = new MainFrame(definition, config);
    noex::string msg = main_frame->OpenURLBase(0);
    const char* expected =
        "URL should NOT contains ' ', ';', '|', '&', '\\r', nor '\\n'.\n"
        "URL: https://example .com";
    EXPECT_STREQ(msg.c_str(), expected);
}
