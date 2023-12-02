#include "Hooks.h"

#include <algorithm>

#include "Configs.h"

namespace Hooks {
	REL::Relocation<std::uintptr_t> target1{ REL::ID(196618), 0x6E };
	REL::Relocation<std::uintptr_t> target2{ REL::ID(196618), 0xB3 };

	void CollectFiles_Hook(RE::BSTArray<RE::BSFixedString>& a_pluginList, bool arg2) {
		using func_t = void(*)(RE::BSTArray<RE::BSFixedString>&, bool);
		REL::Relocation<func_t> func{ REL::ID(33174) };
		func(a_pluginList, arg2);

		logger::info("--- Plugins before patch ---");

		for (auto it = a_pluginList.begin(); it != a_pluginList.end(); ) {
			logger::info("> {}", *it);
			auto plugin_iter = std::find_if(Configs::g_finalOrders.begin(), Configs::g_finalOrders.end(), [&it](const std::pair<std::string, bool>& element) {
				return element.first == *it;
				});
			if (plugin_iter == Configs::g_finalOrders.end()) {
				it++;
				continue;
			}

			a_pluginList.erase(it);
			plugin_iter->second = true;
		}

		logger::info("");

		for (const auto& pluginPair : Configs::g_finalOrders) {
			if (!pluginPair.second)
				continue;

			a_pluginList.push_back(pluginPair.first);
		}

		logger::info("--- Plugins after patch ---");

		for (const auto& plugin : a_pluginList)
			logger::info("> {}", plugin);

		logger::info("");
	}

	void Install() {
		F4SE::Trampoline& trampoline = F4SE::GetTrampoline();
		trampoline.write_call<5>(target1.get(), CollectFiles_Hook);
		trampoline.write_call<5>(target2.get(), CollectFiles_Hook);
	}
}
