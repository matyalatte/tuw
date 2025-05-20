#pragma once

#include <gtest/gtest.h>
#include "main_frame.h"
#include "string_utils.h"
#include "exec.h"
#include "json_utils.h"
#include "tuw_constants.h"
#include "exe_container.h"
#include "validator.h"
#include "env_utils.h"
#include "noex/vector.hpp"
#include "json.h"

// you need to copy it from examples/all_keys to the json folder
constexpr char JSON_ALL_KEYS[] = "./json/gui_definition.json";

// you need to copy the json folder to build dir.
constexpr char JSON_BROKEN[] = "./json/broken.json";
constexpr char JSON_CONFIG_ASCII[] = "./json/config_ascii.json";
constexpr char JSON_CONFIG_UTF[] = "./json/config_utf.json";
constexpr char JSON_RELAXED[] = "./json/relaxed.jsonc";
constexpr char JSON_CROSS_PLATFORM[] = "./json/platform.json";

inline void GetTestJson(tuwjson::Value& json) {
    noex::string err = json_utils::LoadJson(JSON_ALL_KEYS, json);
    EXPECT_TRUE(err.empty());
    EXPECT_FALSE(!json.IsObject() || json.IsEmptyObject());
}

inline void GetTestJson(tuwjson::Value& json, const char* file) {
    noex::string err = json_utils::LoadJson(file, json);
    EXPECT_TRUE(err.empty());
    EXPECT_FALSE(!json.IsObject() || json.IsEmptyObject());
}
