#pragma once
#include "module.hh"
#include <string_view>

class PluginBase {
public:
	std::string_view name;
	PluginModule *module;
};
