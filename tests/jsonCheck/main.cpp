#include <gtest/gtest.h>
#include "jsonUtils.h"

char const * broken;
char const * fineHelp;

int main (int argc, char * argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    assert(argc == 3);

    broken = argv[1];
    fineHelp = argv[2];

    return RUN_ALL_TESTS();
}

TEST(JsonCheckTest, LoadJsonFail) {
    nlohmann::json test_json = jsonUtils::loadJson("");
    EXPECT_EQ(nlohmann::json({}), test_json);
}

TEST(JsonCheckTest, LoadJsonFail2) {
    nlohmann::json test_json = jsonUtils::loadJson(broken);
    EXPECT_EQ(nlohmann::json({}), test_json);
}

TEST(JsonCheckTest, LoadJsonSuccess) {
    nlohmann::json test_json = jsonUtils::loadJson(fineHelp);
    EXPECT_NE(nlohmann::json({}), test_json);
}

TEST(JsonCheckTest, checkHelpSuccess) {
    nlohmann::json test_json = jsonUtils::loadJson(fineHelp);
    ASSERT_NE(nlohmann::json({}), test_json);
    jsonUtils::checkHelpURLs(test_json);
}

TEST(JsonCheckTest, checkHelpFail) {
    nlohmann::json test_json = jsonUtils::loadJson(fineHelp);
    ASSERT_NE(nlohmann::json({}), test_json);
    test_json["help"][0].erase("label");
    try{
        jsonUtils::checkHelpURLs(test_json);
        FAIL();
    }
    catch(std::exception& expected) {
        EXPECT_STREQ("['label'] not found.", expected.what());
    }
}

TEST(JsonCheckTest, checkHelpFail2) {
    nlohmann::json test_json = jsonUtils::loadJson(std::string(fineHelp));
    ASSERT_NE(nlohmann::json({}), test_json);
    test_json["help"][0]["label"] = 3;
    try{
        jsonUtils::checkHelpURLs(test_json);
        FAIL();
    }
    catch(std::exception& expected) {
        EXPECT_STREQ("['label'] should be a string.", expected.what());
    }
}
