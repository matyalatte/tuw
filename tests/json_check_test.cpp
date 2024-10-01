#pragma once

// Tests for json checking
// Todo: Write more tests

#include "test_utils.h"

TEST(JsonCheckTest, LoadJsonFail) {
    rapidjson::Document test_json;
    ErrorState result;
    json_utils::LoadJson("fake.json", test_json, &result);
    const char* expected = "Failed to open fake.json";
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ(expected, result.GetErrorMsg());
}

TEST(JsonCheckTest, LoadJsonFail2) {
    rapidjson::Document test_json;
    ErrorState result;
    json_utils::LoadJson(JSON_BROKEN, test_json, &result);
    const char* expected = "Failed to parse JSON: Missing a comma or '}'"
                           " after an object member.";
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ(expected, std::string(result.GetErrorMsg(), 68).c_str());
}

TEST(JsonCheckTest, LoadJsonWithComments) {
    // Check if json parser supports c-style comments and trailing commas.
    rapidjson::Document test_json;
    ErrorState result;
    json_utils::LoadJson(JSON_RELAXED, test_json, &result);
    EXPECT_TRUE(result.Ok());
}

TEST(JsonCheckTest, LoadJsonSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
}

TEST(JsonCheckTest, checkGUISuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    ErrorState result;
    json_utils::CheckDefinition(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

TEST(JsonCheckTest, checkGUISuccess2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    rapidjson::Value& comp = test_json["gui"][0]["components"][6];
    comp.AddMember("item_array", comp["items"], test_json.GetAllocator());
    comp.RemoveMember("items");
    ErrorState result;
    json_utils::CheckDefinition(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

TEST(JsonCheckTest, checkGUISuccess3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    ErrorState result;
    json_utils::CheckDefinition(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

TEST(JsonCheckTest, checkGUISuccess4) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][2]);
    ErrorState result;
    json_utils::CheckDefinition(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

void CheckGUIError(rapidjson::Document& test_json, const char* expected) {
    ErrorState result;
    json_utils::CheckDefinition(&result, test_json);
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ(expected, result.GetErrorMsg());
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
    test_json["gui"][0]["components"][6].RemoveMember("items");
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
        " echo file: __comp1__ & echo folder: __comp2__ & echo combo: __comp3__ &"
        " echo radio: __comp4__ & echo check: __comp5__ & echo check_array: __comp6__ &"
        " echo textbox: __comp7__ & echo int: __comp8__ & echo float: __comp???__");
}

TEST(JsonCheckTest, checkGUIFail7) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][1].AddMember("id", "aaa", test_json.GetAllocator());
    CheckGUIError(test_json,
        "The ID of [\"components\"][1] is unused in the command;"
        " echo file: __comp2__ & echo folder: __comp3__ & echo combo: __comp4__"
        " & echo radio: __comp5__ & echo check: __comp6__ & echo check_array: __comp7__"
        " & echo textbox: __comp8__ & echo int: __comp9__ & echo float: __comp???__");
}

TEST(JsonCheckTest, checkHelpSuccess) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    ErrorState result;
    json_utils::CheckHelpURLs(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

void CheckHelpError(rapidjson::Document& test_json, const char* expected) {
    ErrorState result;
    json_utils::CheckHelpURLs(&result, test_json);
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ(expected, result.GetErrorMsg());
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
    test_json["recommended"].SetString(tuw_constants::VERSION);
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_TRUE(result.Ok());
    EXPECT_FALSE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionFail) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString("0.2.3");
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_TRUE(result.Ok());
    EXPECT_TRUE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionFail2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString("foo");
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ("Can NOT convert 'foo' to int.", result.GetErrorMsg());
}

TEST(JsonCheckTest, checkVersionSuccess2) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString(tuw_constants::VERSION);
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_TRUE(result.Ok());
}

TEST(JsonCheckTest, checkVersionFail3) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString("1.0.0");
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ("Version 1.0.0 is required.", result.GetErrorMsg());
}

TEST(JsonCheckTest, checkVersionFail4) {
    rapidjson::Document test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString("foo");
    ErrorState result;
    json_utils::CheckVersion(&result, test_json);
    EXPECT_FALSE(result.Ok());
    EXPECT_STREQ("Can NOT convert 'foo' to int.", result.GetErrorMsg());
}
