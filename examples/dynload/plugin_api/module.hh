#pragma once

class PluginModule {
public:
	virtual void init() = 0;
	virtual float get_val(unsigned chan) = 0;
	virtual void set_val(unsigned chan, float val) = 0;
	virtual void update() = 0;

	float data[4];
};
