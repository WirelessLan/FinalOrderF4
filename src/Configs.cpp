#include "Configs.h"

#include <fstream>

namespace Configs {
	std::vector<std::pair<std::string, bool>> g_finalOrders;

	std::string Trim(const std::string& a_str) {
		std::string trimmed_str = a_str;

		trimmed_str.erase(trimmed_str.begin(), std::find_if(trimmed_str.begin(), trimmed_str.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));

		trimmed_str.erase(std::find_if(trimmed_str.rbegin(), trimmed_str.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), trimmed_str.end());

		return trimmed_str;
	}

	void LoadConfig() {
		std::string configPath = fmt::format("Data\\F4SE\\Plugins\\{}.txt", Version::PROJECT);

		logger::info("Loading config...");

		std::ifstream configFile(configPath);
		if (!configFile.is_open()) {
			logger::error("Cannot open the config file: {}", configPath);
			return;
		}

		std::string line;
		while (std::getline(configFile, line)) {
			line = Trim(line);
			if (line.empty() || line[0] == '#')
				continue;

			logger::info ("> {}", line);
			g_finalOrders.push_back(std::make_pair(line, false));
		}

		logger::info("Config loaded...");
	}
}
