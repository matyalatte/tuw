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
    try {
        nlohmann::json test_json = json_utils::LoadJson("fake.json");
        FAIL();
    }
    catch(std::exception& err) {
        const char* expected = "Failed to open fake.json";
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, LoadJsonFail2) {
    try {
        nlohmann::json test_json = json_utils::LoadJson(broken);
        FAIL();
    }
    catch(std::exception& err) {
        const char* expected = "[json.exception.parse_error.101] parse error at line 7, column 5:"
                               " syntax error while parsing object - unexpected ']'; expected '}'";
        EXPECT_STREQ(expected, err.what());
    }
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
    nlohmann::json comp = test_json["gui"][0]["components"][5];
    comp["item_array"] = comp["item"];
    comp.erase("item");
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
    test_json["gui"][0]["components"][5].erase("item");
    CheckGUIError(test_json, "['options']['item'] not found.");
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
    test_json["gui"][1]["components"][4]["value"] = nlohmann::json::array();
    CheckGUIError(test_json,
        "['Combo box']['value'] and ['Combo box']['item'] should have the same size.");
}

TEST(JsonCheckTest, checkGUIFail7) {
    nlohmann::json test_json = GetTestJson();
    test_json["gui"][0]["components"][5]["default"] = nlohmann::json::array();
    CheckGUIError(test_json,
        "['options']['default'] and ['options']['item'] should have the same size.");
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

TEST(JsonCheckTest, checkVersionSuccess) {
    nlohmann::json test_json = GetTestJson();
    test_json["recommended"] = "0.3.0";
    json_utils::CheckVersion(test_json);
    EXPECT_FALSE(test_json["not_recommended"]);
}

TEST(JsonCheckTest, checkVersionFail) {
    nlohmann::json test_json = GetTestJson();
    test_json["recommended"] = "0.2.3";
    json_utils::CheckVersion(test_json);
    EXPECT_TRUE(test_json["not_recommended"]);
}

TEST(JsonCheckTest, checkVersionSuccess2) {
    nlohmann::json test_json = GetTestJson();
    test_json["minimum_required"] = "0.3.0";
    json_utils::CheckVersion(test_json);
}

TEST(JsonCheckTest, checkVersionFail2) {
    nlohmann::json test_json = GetTestJson();
    test_json["minimum_required"] = "1.0.0";
    try {
        json_utils::CheckVersion(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ("Version 1.0.0 is required.", err.what());
    }
}
