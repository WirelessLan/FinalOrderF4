#pragma once

namespace Configs {
	extern std::vector<std::string> g_finalOrderVector;
	extern std::unordered_map<std::string, bool> g_pluginExistsMap;

	void LoadConfig();
}
