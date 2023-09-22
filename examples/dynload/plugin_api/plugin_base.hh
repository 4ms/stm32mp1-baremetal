#pragma once
#include "common.hh"
#include <cstdint>

class PluginBase {
public:
	virtual void init() = 0;
	virtual float get_val(Channel &chan) = 0;
	virtual void set_val(Channel const &chan, float val) = 0;
	virtual void update() = 0;

	float data[4];
};
