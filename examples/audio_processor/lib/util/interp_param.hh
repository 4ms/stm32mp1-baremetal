#pragma once

template<typename T, unsigned int num_updates>
struct InterpParam {
	InterpParam()
		: cur_val{0}
		, step_size{0}
	{
		static_assert(num_updates > 0);
	}

	void set_new_value(T new_val)
	{
		T d = new_val - cur_val;
		step_size = d / num_updates;
	}
	T next()
	{
		cur_val += step_size;
		return cur_val;
	}
	T get_step_size()
	{
		return step_size;
	}

	T cur_val;

private:
	T step_size;
};
