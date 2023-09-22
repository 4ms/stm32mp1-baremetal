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
		// Parse the object file raw data and find where the PluginBase derived class is (table of function ptrs)
		// How do we know?
		auto ptr = reinterpret_cast<PluginBase *>(raw_data.data());
		return ptr;
	}
};
