#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace jsonUtils {
	nlohmann::json loadJson(std::string file);
	bool saveJson(nlohmann::json json, std::string file);

	bool hasKey(nlohmann::json json, std::string key);

	nlohmann::json default_definition();
	std::string checkSubDefinition(nlohmann::json& sub_definition);
	std::string checkHelpURLs(nlohmann::json& definition);
}