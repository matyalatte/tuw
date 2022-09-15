// Functions related to json.

#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace jsonUtils {
	nlohmann::json loadJson(std::string file);
	bool saveJson(nlohmann::json json, std::string file);

	nlohmann::json default_definition();
	void checkSubDefinition(nlohmann::json& sub_definition);
	void checkHelpURLs(nlohmann::json& definition);
}