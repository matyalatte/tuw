// Tests for json embedding
// Todo: Write more tests

#include "test_utils.h"

Validator GetValidator(const char* config_str) {
    rapidjson::Document config(rapidjson::kObjectType);
    config.Parse(config_str);
    Validator validator;
    validator.Initialize(config);
    return validator;
}

TEST(ValidatorTest, Regex) {
    const char* config =
        "{"
        "    \"regex\": \"^[0-9]*$\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_TRUE(validator.Validate("1234"));
    EXPECT_FALSE(validator.Validate("1234abcd"));
    EXPECT_STREQ("Regex match failed for pattern: ^[0-9]*$",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, RegexCompileError) {
    const char* config =
        "{"
        "    \"regex\": \"^[0-9\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate("1234"));
    EXPECT_STREQ("Failed to parse regex pattern: ^[0-9",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, RegexGroupOp) {
    const char* config =
        "{"
        "    \"regex\": \"(a|b)\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate("1234"));
    EXPECT_STREQ("Regex compile error: () operators are not supported.",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, RegexGroupOpEscape) {
    const char* config =
        "{"
        "    \"regex\": \"^\\(a|b\\)\"$"
        "}";
    Validator validator = GetValidator(config);
    EXPECT_TRUE(validator.Validate("(a"));
    EXPECT_TRUE(validator.Validate("b)"));
}

TEST(ValidatorTest, RegexCustomError) {
    const char* config =
        "{"
        "    \"regex\": \"^[0-9]*$\","
        "    \"regex_error\": \"Should be numeric!\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate("abcd"));
    EXPECT_STREQ("Should be numeric!",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, Wildcard) {
    const char* config =
        "{"
        "    \"wildcard\": \"test*foo\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_TRUE(validator.Validate("testfoo"));
    EXPECT_TRUE(validator.Validate("testbarfoo"));
    EXPECT_FALSE(validator.Validate("test"));
    EXPECT_STREQ("Wildcard match failed for pattern: test*foo",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, WildcardCustomError) {
    const char* config =
        "{"
        "    \"wildcard\": \"test*foo\","
        "    \"wildcard_error\": \"Custom message!\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate("test"));
    EXPECT_STREQ("Custom message!",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, NotEmpty) {
    const char* config =
        "{"
        "    \"not_empty\": true"
        "}";
    Validator validator = GetValidator(config);
    EXPECT_TRUE(validator.Validate("test"));
    EXPECT_FALSE(validator.Validate(""));
    EXPECT_STREQ("Empty string is NOT allowed.",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, NotEmptyCustomError) {
    const char* config =
        "{"
        "    \"not_empty\": true,"
        "    \"not_empty_error\": \"Custom message!\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate(""));
    EXPECT_STREQ("Custom message!",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, Exist) {
    const char* config =
        "{"
        "    \"exist\": true"
        "}";
    Validator validator = GetValidator(config);
    EXPECT_TRUE(validator.Validate(JSON_ALL_KEYS));
    EXPECT_FALSE(validator.Validate(noex::string(JSON_ALL_KEYS) + ".fake.does_not_exist"));
    EXPECT_STREQ("Path does NOT exist.",
                 validator.GetError().c_str());
}

TEST(ValidatorTest, ExistCustomError) {
    const char* config =
        "{"
        "    \"exist\": true,"
        "    \"exist_error\": \"Custom message!\""
        "}";
    Validator validator = GetValidator(config);
    EXPECT_FALSE(validator.Validate(noex::string(JSON_ALL_KEYS) + ".fake.does_not_exist"));
    EXPECT_STREQ("Custom message!",
                 validator.GetError().c_str());
}
