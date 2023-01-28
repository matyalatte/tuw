// Tests for json checking
// Todo: Write more tests

#include <gtest/gtest.h>
#include "json_utils.h"

char const * broken;
char const * json_file;

int main(int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 3);

    broken = argv[1];
    json_file = argv[2];

    return RUN_ALL_TESTS();
}

TEST(JsonCheckTest, LoadJsonFail) {
    nlohmann::json test_json = json_utils::LoadJson("");
    EXPECT_EQ(nlohmann::json({}), test_json);
}

TEST(JsonCheckTest, LoadJsonFail2) {
    nlohmann::json test_json = json_utils::LoadJson(broken);
    EXPECT_EQ(nlohmann::json({}), test_json);
}

nlohmann::json GetTestJson() {
    nlohmann::json test_json = json_utils::LoadJson(json_file);
    EXPECT_NE(nlohmann::json({}), test_json);
    return test_json;
}

TEST(JsonCheckTest, LoadJsonSuccess) {
    GetTestJson();
}

TEST(JsonCheckTest, checkGUISuccess) {
    nlohmann::json test_json = GetTestJson();
    json_utils::CheckDefinition(test_json);
}

TEST(JsonCheckTest, checkGUISuccess2) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0]["components"][5]["item"] = test_json["gui"][0]["components"][5]["items"];
    test_json["gui"][0]["components"][5].erase("items");
    json_utils::CheckDefinition(test_json);
}

void CheckGUIError(nlohmann::json test_json, const char* expected) {
    try {
        json_utils::CheckDefinition(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, checkGUIFail) {
    nlohmann::json test_json = GetTestJson();
    test_json.erase("gui");
    CheckGUIError(test_json, "['gui'] not found.");
}

TEST(JsonCheckTest, checkGUIFail2) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0]["components"][5].erase("items");
    CheckGUIError(test_json, "['options']['items'] not found.");
}

TEST(JsonCheckTest, checkGUIFail3) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"] = 1;
    CheckGUIError(test_json, "['gui'] should be an array of json objects.");
}

TEST(JsonCheckTest, checkGUIFail4) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0]["components"][0]["label"] = 1;
    CheckGUIError(test_json, "['components']['label'] should be a string.");
}

TEST(JsonCheckTest, checkGUIFail5) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][2]["show_last_line"] = "test";
    CheckGUIError(test_json, "['show_last_line'] should be a boolean.");
}

TEST(JsonCheckTest, checkGUIFail6) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][1]["components"][4]["values"] = nlohmann::json::array();
    CheckGUIError(test_json,
        "['Combo box']['values'] and ['Combo box']['items'] should have the same size.");
}

TEST(JsonCheckTest, checkGUIFail7) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0]["components"][5]["default"] = nlohmann::json::array();
    CheckGUIError(test_json,
        "['options']['default'] and ['options']['items'] should have the same size.");
}

TEST(JsonCheckTest, checkHelpSuccess) {
    nlohmann::json test_json = GetTestJson();
    json_utils::CheckHelpURLs(test_json);
}

void CheckHelpError(nlohmann::json test_json, const char* expected) {
    try {
        json_utils::CheckHelpURLs(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, checkHelpFail) {
    nlohmann::json test_json = GetTestJson();
    test_json["help"][0].erase("label");
    CheckHelpError(test_json, "['label'] not found.");
}

TEST(JsonCheckTest, checkHelpFail2) {
    nlohmann::json test_json = GetTestJson();
    test_json["help"][0]["label"] = 3;
    CheckHelpError(test_json, "['label'] should be a string.");
}
