#include <stdio.h>
void DeclareAndInitArray(void);
void PrintArray(void);

int main(int argc, char* argv[])
{
	DeclareAndInitArray();
	PrintArray();
	return 0;
}

void DeclareAndInitArray(void)
{
	int array[10];
	int i;
	printf("Addr1 = %p\n", array);
	for(int i=0; i<10; ++i)
	{
		array[i]=i;
	}
}

void PrintArray(void)
{
	int array[10];
	int i;
	printf("Addr2 = %p\n", array);
	for(int i=0; i<10; ++i)
	{
		printf("%d ",array[i]);
	}
	putchar('\n');
}
