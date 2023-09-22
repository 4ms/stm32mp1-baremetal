#include "plugin/plugin_base.hh"

struct PluginA : PluginBase {
	void init() override
	{
		data[0] = 101.1f;
		data[1] = 102.2f;
		data[2] = 103.3f;
		data[3] = 104.4f;
	}

	float get_val(Channel &chan) override { return data[make_number(chan)]; }

	void set_val(Channel const &chan, float val) override { data[make_number(chan)] = val; }

	void update() override
	{
		data[2] = perform_calculationB(data[3]);
		data5--;
	}

	unsigned data5 = 99999;
};
