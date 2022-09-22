// Tests for MainFrame.cpp
// Todo: Write more tests

#include <gtest/gtest.h>
#include "wx/app.h"
#include "wx/modalhook.h"
#include "main_frame.h"

char const * json_file;
char const * config_file;

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
    assert(argc == 3);

    json_file = argv[1];
    config_file = argv[2];

    // Make dummy app
    wxApp* app = new wxApp();

    // Initialize app
    wxEntryStart(argc, argv);

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

TEST(MainFrameTest, InvalidDefinition) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][1]["components"][4]["default"] = "number";
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));
    EXPECT_EQ(json_utils::GetDefaultDefinition(), main_frame->GetDefinition()["gui"][0]);
    EXPECT_TRUE(main_frame->GetDefinition().contains("help"));
}

TEST(MainFrameTest, InvalidHelp) {
    nlohmann::json test_json = GetTestJson();
    test_json["help"][0]["url"] = 1;
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));
    EXPECT_FALSE(main_frame->GetDefinition().contains("help"));
    EXPECT_EQ(test_json["gui"][0]["components"],
        main_frame->GetDefinition()["gui"][0]["components"]);
}

TEST(MainFrameTest, RunCommandSuccess) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));

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
    test_json["gui"][0]["command"] = "I want errors";
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));

    std::array<std::string, 2> msg = main_frame->RunCommand();
    std::string errmsg = msg[1];
    EXPECT_STREQ("", msg[0].c_str());
    EXPECT_STREQ("Json format error(Can not make command)", msg[1].c_str());
}

TEST(MainFrameTest, RunCommandFail2) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0] = json_utils::GetDefaultDefinition();
    test_json["gui"][0]["command"] = "I'll fail";
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));

    std::array<std::string, 2> msg = main_frame->RunCommand();
    std::string errmsg = msg[1];
    EXPECT_STREQ("", msg[0].c_str());
    EXPECT_STRNE("", msg[1].c_str());
}

TEST(MainFrameTest, DeleteFrame) {
    nlohmann::json test_json = GetTestJson();
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json));
    main_frame->Destroy();
}

TEST(MainFrameTest, LoadSaveConfig) {
    nlohmann::json test_json = GetTestJson();
    nlohmann::json test_config = json_utils::LoadJson(config_file);
    MainFrame* main_frame = new MainFrame(nlohmann::json(test_json), nlohmann::json(test_config));
    main_frame->SaveConfig();
    nlohmann::json saved_config = json_utils::LoadJson("gui_config.json");
    test_config.erase("Some folder path");
    saved_config.erase("Some folder path");
    EXPECT_EQ(test_config, saved_config);
}
