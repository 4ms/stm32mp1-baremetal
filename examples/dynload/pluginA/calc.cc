

const int const_data[4]{0xAAAA, 0x55BB, 0x1234, 0x12345678};

unsigned calc_something(unsigned x)
{
	if (x < 3)
		return const_data[x];
	return x + (256 * x);
}
