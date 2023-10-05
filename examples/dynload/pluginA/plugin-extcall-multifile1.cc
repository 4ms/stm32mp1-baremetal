// foreign function
int external_call(int);

// internal functions
void init_plugin2(int *result);

void init_plugin(int *result)
{
	*result = 7 + *result;

	init_plugin2(result);

	*result = external_call(*result);
}
