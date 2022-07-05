void _init(void) {}
void _fini(void) {}
void _exit(void)
{
	while (1)
		;
}
void _kill(void) {}
int _getpid(void)
{
	return 0;
}
int __errno;
void *__dso_handle = (void *)&__dso_handle;
