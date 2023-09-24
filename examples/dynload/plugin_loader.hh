#pragma once
#include "plugin_api/plugin_base.hh"
#include <cstddef>
#include <span>

class PluginLoader {

public:
	enum class Error { OK, PluginNotValid };

	// TODO: std::expected

	[[nodiscard]] PluginBase *load_plugin(std::span<char> raw_data)
	{
		// Parse the object file raw data

		// 1. Parse header: first word is offset to init_array

		// 2. search/replace link addr with load addr in the init_array

		// 3. call function at the start (after header), it returns a ptr to plugin

		auto ptr = reinterpret_cast<PluginBase *>(raw_data.data());
		return ptr;
	}
};
