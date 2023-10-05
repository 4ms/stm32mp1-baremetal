// foreign function
int external_call(int);

void init_plugin(int *result)
{
	*result = 7 + *result;
	*result = external_call(*result);
}
