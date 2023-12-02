#include "Hooks.h"

#include <algorithm>

#include <xbyak/xbyak.h>

#include "Configs.h"

namespace Hooks {
	using CollectFiles_t = void(*)(RE::BSTArray<RE::BSFixedString>&, bool);
	REL::Relocation<std::uintptr_t> target{ REL::ID(33174) };
	CollectFiles_t CollectFiles;

	void CollectFiles_Hook(RE::BSTArray<RE::BSFixedString>& a_pluginList, bool arg2) {
		CollectFiles(a_pluginList, arg2);

		logger::info("--- Plugins before patch ---");

		for (auto it = a_pluginList.begin(); it != a_pluginList.end(); ) {
			logger::info("> {}", *it);

			auto pluginExistsMap_iter = Configs::g_pluginExistsMap.find(it->data());
			if (pluginExistsMap_iter == Configs::g_pluginExistsMap.end()) {
				it++;
				continue;
			}

			a_pluginList.erase(it);
			pluginExistsMap_iter->second = true;
		}

		logger::info("");

		for (const auto& plugin : Configs::g_finalOrderVector) {
			auto pluginExistsMap_iter = Configs::g_pluginExistsMap.find(plugin);
			if (pluginExistsMap_iter == Configs::g_pluginExistsMap.end())
				continue;

			if (!pluginExistsMap_iter->second)
				continue;

			a_pluginList.push_back(plugin);
		}

		logger::info("--- Plugins after patch ---");

		for (const auto& plugin : a_pluginList)
			logger::info("> {}", plugin);

		logger::info("");
	}

	void Install() {
		struct asm_code : Xbyak::CodeGenerator {
			asm_code() {
				Xbyak::Label retLabel;

				mov(ptr[rsp + 0x08], rbx);

				jmp(ptr[rip + retLabel]);

				L(retLabel);
				dq(target.address() + 0x05);
			}
		};

		asm_code p;
		auto& trampoline = F4SE::GetTrampoline();
		void* codeBuf = trampoline.allocate(p);

		CollectFiles = (CollectFiles_t)codeBuf;

		trampoline.write_branch<5>(target.address(), CollectFiles_Hook);
	}
}
