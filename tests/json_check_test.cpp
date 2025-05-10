// Tests for json checking
// Todo: Write more tests

#include "test_utils.h"

TEST(JsonCheckTest, LoadJsonFail) {
    tuwjson::Value test_json;
    noex::string err = json_utils::LoadJson("fake.json", test_json);
    const char* expected = "No such file or directory: fake.json";
    EXPECT_STREQ(expected, err.c_str());
}

TEST(JsonCheckTest, LoadJsonFail2) {
    tuwjson::Value test_json;
    noex::string err = json_utils::LoadJson(JSON_BROKEN, test_json);
    const char* expected =
        "Failed to parse JSON: comma ',' or closing brace '}' is missing"
        " (line: 7, column: 5)";
    EXPECT_STREQ(expected, err.c_str());
}

TEST(JsonCheckTest, LoadJsonWithComments) {
    // Check if json parser supports c-style comments and trailing commas.
    tuwjson::Value test_json;
    noex::string err = json_utils::LoadJson(JSON_RELAXED, test_json);
    EXPECT_TRUE(err.empty());
}

TEST(JsonCheckTest, LoadJsonSuccess) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
}

TEST(JsonCheckTest, checkGUISuccess) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_TRUE(result.ok);
}

TEST(JsonCheckTest, checkGUISuccess2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    tuwjson::Value& comp = test_json["gui"][0]["components"][6];
    comp.ReplaceKey("items", "item_array");
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_TRUE(result.ok);
}

TEST(JsonCheckTest, checkGUISuccess3) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][1]);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_TRUE(result.ok);
}

TEST(JsonCheckTest, checkGUISuccess4) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0].Swap(test_json["gui"][2]);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_TRUE(result.ok);
}

TEST(JsonCheckTest, checkGUISuccessRelaxed) {
    tuwjson::Value test_json;
    noex::string err = json_utils::LoadJson(JSON_RELAXED, test_json);
    EXPECT_TRUE(err.empty());
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_TRUE(result.ok);
}

void CheckGUIError(tuwjson::Value& test_json, const char* expected) {
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckDefinition(result, test_json);
    EXPECT_FALSE(result.ok);
    EXPECT_STREQ(expected, result.msg.c_str());
}

TEST(JsonCheckTest, checkGUIFail) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json.ReplaceKey("gui", "g");
    CheckGUIError(test_json, "gui definition requires \"components\"");
}

TEST(JsonCheckTest, checkGUIFail2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][6].ReplaceKey("items", "notitems");
    CheckGUIError(test_json, "check array requires \"items\" (line: 60, column: 17)");
}

TEST(JsonCheckTest, checkGUIFail3) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"].SetInt(1);
    CheckGUIError(test_json, "\"gui\" should be an array of json objects (line: 4, column: 12)");
}

TEST(JsonCheckTest, checkGUIFail4) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][0]["label"].SetInt(1);
    CheckGUIError(test_json, "\"label\" should be a string (line: 11, column: 30)");
}

TEST(JsonCheckTest, checkGUIFail5) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][2]["show_last_line"].SetString("test");
    CheckGUIError(test_json, "\"show_last_line\" should be a boolean (line: 268, column: 31)");
}

TEST(JsonCheckTest, checkGUIFail6) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"].GetArray()->pop_back();
    CheckGUIError(test_json,
        "The command requires more components for arguments;"
        " echo file: __comp1__ & echo folder: __comp2__ & echo combo: __comp3__ &"
        " echo radio: __comp4__ & echo check: __comp5__ & echo check_array: __comp6__ &"
        " echo textbox: __comp7__ & echo int: __comp8__ & echo float: __comp???__");
}

TEST(JsonCheckTest, checkGUIFail7) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][1]["id"].SetString("aaa");
    CheckGUIError(test_json,
        "component id \"aaa\" (line: 15, column: 27) is unused in the command;"
        " echo file: __comp???__ & echo folder: __comp2__ & echo combo: __comp3__"
        " & echo radio: __comp4__ & echo check: __comp5__ & echo check_array: __comp6__"
        " & echo textbox: __comp7__ & echo int: __comp8__ & echo float: __comp9__");
}

TEST(JsonCheckTest, checkGUIFail8) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][0]["components"][0]["id"].SetString("aaa");
    test_json["gui"][0]["components"][1]["id"].SetString("aaa");
    CheckGUIError(test_json,
        "Found a duplicated id: \"aaa\" (line: 15, column: 27)");
}

TEST(JsonCheckTest, checkGUIFailRelaxed) {
    tuwjson::Value test_json;
    noex::string err = json_utils::LoadJson(JSON_RELAXED, test_json);
    EXPECT_TRUE(err.empty());
    test_json["exit_success"].SetString("a");
    CheckGUIError(test_json, "\"exit_success\" should be an int");
}

TEST(JsonCheckTest, checkHelpSuccess) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckHelpURLs(result, test_json);
    EXPECT_TRUE(result.ok);
}

void CheckHelpError(tuwjson::Value& test_json, const char* expected) {
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckHelpURLs(result, test_json);
    EXPECT_FALSE(result.ok);
    EXPECT_STREQ(expected, result.msg.c_str());
}

TEST(JsonCheckTest, checkHelpFail) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["help"][0].ReplaceKey("label", "notlabel");
    CheckHelpError(test_json, "help document requires \"label\" (line: 281, column: 9)");
}

TEST(JsonCheckTest, checkHelpFail2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["help"][0]["label"].SetInt(3);
    CheckHelpError(test_json, "\"label\" should be a string (line: 283, column: 22)");
}

TEST(JsonCheckTest, checkVersionSuccess) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString(tuw_constants::VERSION);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionFail) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString("0.2.3");
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(test_json["not_recommended"].GetBool());
}

TEST(JsonCheckTest, checkVersionFail2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["recommended"].SetString("foo");
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_FALSE(result.ok);
    EXPECT_STREQ("Can NOT convert 'foo' to int.", result.msg.c_str());
}

TEST(JsonCheckTest, checkVersionSuccess2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString(tuw_constants::VERSION);
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_TRUE(result.ok);
}

TEST(JsonCheckTest, checkVersionFail3) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString("1.0.0");
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_FALSE(result.ok);
    EXPECT_STREQ("Version 1.0.0 is required.", result.msg.c_str());
}

TEST(JsonCheckTest, checkVersionFail4) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["minimum_required"].SetString("foo");
    json_utils::JsonResult result = JSON_RESULT_OK;
    json_utils::CheckVersion(result, test_json);
    EXPECT_FALSE(result.ok);
    EXPECT_STREQ("Can NOT convert 'foo' to int.", result.msg.c_str());
}

TEST(JsonCheckTest, checkGUIIntInc) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][8]["inc"].SetInt(-1);
    CheckGUIError(test_json,
        "\"inc\" should be a positive number. (line: 240, column: 28)");
}

TEST(JsonCheckTest, checkGUIFloatInc2) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][9]["inc"].SetDouble(-0.1);
    CheckGUIError(test_json,
        "\"inc\" should be a positive number. (line: 253, column: 28)");
}

TEST(JsonCheckTest, checkGUIMinMax) {
    tuwjson::Value test_json;
    GetTestJson(test_json);
    test_json["gui"][1]["components"][9]["min"].SetDouble(2);
    CheckGUIError(test_json,
        "\"max\" should be greater than \"min\". (line: 251, column: 28)");
}
