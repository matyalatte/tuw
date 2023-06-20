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
        rapidjson::Document test_json;
        json_utils::LoadJson("fake.json", test_json);
        FAIL();
    }
    catch(std::exception& err) {
        const char* expected = "Failed to open fake.json";
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, LoadJsonFail2) {
    try {
        rapidjson::Document test_json;
        json_utils::LoadJson(broken, test_json);
        FAIL();
    }
    catch(std::exception& err) {
        const char* expected = "Failed to parse JSON: Missing a comma or '}'"
                               " after an object member. (offset: 122)";
        EXPECT_STREQ(expected, err.what());
    }
}

void GetTestJson(rapidjson::Document& json) {
    json_utils::LoadJson(json_file, json);
    EXPECT_NE(json.Size(), 0);
}

TEST(JsonCheckTest, LoadJsonSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
}

TEST(JsonCheckTest, checkGUISuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::CheckDefinition(test_json);
}

TEST(JsonCheckTest, checkGUISuccess2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Value& comp = test_json["gui"][0]["components"][5];
    comp.AddMember("item_array", comp["item"], test_json.GetAllocator());
    comp.RemoveMember("item");
    json_utils::CheckDefinition(test_json);
}

TEST(JsonCheckTest, checkGUISuccess3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    json_utils::CheckDefinition(test_json);
}

TEST(JsonCheckTest, checkGUISuccess4) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][2]);
    json_utils::CheckDefinition(test_json);
}

void CheckGUIError(rapidjson::Document& test_json, const char* expected) {
    try {
        json_utils::CheckDefinition(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, checkGUIFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json.RemoveMember("gui");
    CheckGUIError(test_json, "['gui'] not found.");
}

TEST(JsonCheckTest, checkGUIFail2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][5].RemoveMember("items");
    CheckGUIError(test_json, "['options']['items'] not found.");
}

TEST(JsonCheckTest, checkGUIFail3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"].SetInt(1);
    CheckGUIError(test_json, "['gui'] should be an array of json objects.");
}

TEST(JsonCheckTest, checkGUIFail4) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][0]["label"].SetInt(1);
    CheckGUIError(test_json, "['components']['label'] should be a string.");
}

TEST(JsonCheckTest, checkGUIFail5) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][2]["show_last_line"].SetString("test");
    CheckGUIError(test_json, "['show_last_line'] should be a boolean.");
}

TEST(JsonCheckTest, checkGUIFail6) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"].PopBack();
    CheckGUIError(test_json,
        "The command requires more components for arguments;"
        " echo file: __comp1__ & echo folder: __comp2__ & echo choice: __comp3__ &"
        " echo check: __comp4__ & echo check_array: __comp5__ & echo textbox: __comp6__ &"
        " echo int: __comp7__ & echo float: __comp???__");
}

TEST(JsonCheckTest, checkGUIFail7) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][1].AddMember("id", "aaa", test_json.GetAllocator());
    CheckGUIError(test_json,
        "The ID of [\"commponents\"][1] is unused in the command;"
        " echo file: __comp2__ & echo folder: __comp3__ & echo choice: __comp4__"
        " & echo check: __comp5__ & echo check_array: __comp6__ & echo textbox: __comp7__"
        " & echo int: __comp8__ & echo float: __comp???__");
}

TEST(JsonCheckTest, checkHelpSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    json_utils::CheckHelpURLs(test_json);
}

void CheckHelpError(rapidjson::Document& test_json, const char* expected) {
    try {
        json_utils::CheckHelpURLs(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ(expected, err.what());
    }
}

TEST(JsonCheckTest, checkHelpFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["help"][0].RemoveMember("label");
    CheckHelpError(test_json, "['label'] not found.");
}

TEST(JsonCheckTest, checkHelpFail2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["help"][0]["label"].SetInt(3);
    CheckHelpError(test_json, "['label'] should be a string.");
}

TEST(JsonCheckTest, checkVersionSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString(scr_constants::VERSION);
    json_utils::CheckVersion(test_json);
    EXPECT_FALSE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString("0.2.3");
    json_utils::CheckVersion(test_json);
    EXPECT_TRUE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionSuccess2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString(scr_constants::VERSION);
    json_utils::CheckVersion(test_json);
}

TEST(JsonCheckTest, checkVersionFail2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString("1.0.0");
    try {
        json_utils::CheckVersion(test_json);
        FAIL();
    }
    catch(std::exception& err) {
        EXPECT_STREQ("Version 1.0.0 is required.", err.what());
    }
}
