#include"apue.h"

int lsearch_int(int*, int*,int);
void show(int*, int );

int main(void)
{
	int source[6]={1, 4, 7, 3, 6, 9};
	int target = 6;
	
	int res=lsearch_int(source, &target, 6);
	show(source, 6);	
	printf("The index of element %d is %d\n", target, res);
	printf("Done!\n");
	exit(0);
}


int lsearch_int(int*src, int*tar, int size)
{
	
	for(int index=0; index<size; index++)
	{
		if(src[index] == *tar)
			return index;
	}
	
	return -1;

}


void show(int*p, int size)
{
	for(int i=0; i<size; i++)
		printf("%d ", p[i]);
	putchar('\n');
}









