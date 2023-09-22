#include "plugin/plugin_base.hh"

struct PluginA : PluginBase {
	void init() override
	{
		data[0] = 1.1f;
		data[1] = 2.2f;
		data[2] = 3.3f;
		data[3] = 4.4f;
	}

	float get_val(Channel &chan) override { return data[make_number(chan)]; }

	void set_val(Channel const &chan, float val) override { data[make_number(chan)] = val; }

	void update() override
	{
		data[2] = perform_calculationA(data[3]);
		data5++;
	}

	unsigned data5 = 0;
};
