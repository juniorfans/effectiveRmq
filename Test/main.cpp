
#define NULL 0

int main()
{
	int a = 0 ? 1 : 1;
	int *b = NULL;
	(NULL != b) ? (*b = 1) : 2;
	return true ? 0 : 2;
}