#pragma once
#include <algorithm>

// Todo: is this necessary?

// requires SOURCE::get_val(int chan)-->int, POSTFILTER::add_val(int val), POSTFILTER::val()-->FilteredT
template<typename AdcSource, int NumChans, typename PostFilter>
struct AnalogIn : AdcSource {
	AnalogIn() {
	}

	template<typename T>
	AnalogIn(T &&init)
		: AdcSource{std::move(init)} {
	}

	void read(int chan) {
		_postfilter[chan].add_val(this->get_val(chan));
	}
	void read() {
		for (int chan = 0; chan < NumChans; chan++)
			read(chan);
	}
	auto get(int chan) {
		return _postfilter[chan].val();
	}

private:
	PostFilter _postfilter[NumChans];
};
