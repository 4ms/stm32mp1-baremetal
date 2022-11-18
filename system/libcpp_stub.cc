namespace std
{
void __throw_bad_function_call()
{
	while (1)
		;
}
void __throw_bad_alloc()
{
	while (1)
		;
}
void __throw_length_error(char const *)
{
	while (1)
		;
}
} // namespace std

extern "C" void __cxa_pure_virtual()
{
	while (1)
		;
}
