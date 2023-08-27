#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* lsearch(void*, void*, int, int);
void* lsearch_p(void*, void*, int, int, int(*cmpfn)(void*, void*));
int IntCmp(void*, void*);
void show(int*, int);

int main(void)
{
	int arr[8] = {1, 4, 7, 3, 6, 9, 2, 5};
	int target =5;
	int index = 0;
	
	show(arr, 8);
	int *res = lsearch(arr, &target, 8, sizeof(int));
	if(res!=NULL)
		index = res-arr;
	else 
		index = -1;
	printf("The first index of %d is %d\n", target, index);
	int *resp= lsearch_p(arr, &target, 8, sizeof(int), IntCmp);
	if(resp!=NULL)
		index = res-arr;
	else
		index = -1;
	printf("The first index of %d is %d\n", target, index);
	printf("Done.\n");
	exit(0);

}

/*
 *	版本1
 * 	手动进行地址偏移运算，使用memcpy进行内存级别的比较，判断是否命中元素
 * 	缺陷：由于结构体的内存对齐机制，导致实际存储空间中存在空洞，直接进行
 *  内存级别的比较可能产生错误的结果
 * 	函数参数	 
 * 				void* base 数组的起始地址
 *  			void* key	目标元素的地址
 * 				int n		数组的元素个数
 * 				int elemsize 数组的元素大小
 *  返回值
 * 				void* 成功返回找到的第一个元素地址，失败返回NULL
 * 				 
 */
void* lsearch(void*base, void*key, int n, int elemsize)
{
	for(int i=0; i<n; i++)
	{
		void*elemAddr = (char*)base + i*elemsize;
		if(memcmp(key, elemAddr, elemsize)==0)   //memory comparision
			return elemAddr;
	
	}
	return NULL;
}

/*
 *	版本2 
 * 	手动进行地址偏移计算，使用调用者提供的回调函数进行比较，判断是否命中元素
 * 	改进：通过调用者提供的比较函数进行判等操作，不会受到结构体内存对齐因素的影响
 * 	函数参数
 * 			void* base 		数组的起始地址
 * 			void* key 		目标元素的地址
 *			int	n			数组元素个数
 *			int elemsize	元素大小
 * 			int (*cmpfn)(void* vp1, void* vp2) 调用者提供的回调函数 callback
 * 	返回值
 * 			查找成功返回首个元素的地址， 失败返回NULL 
 */  

void* lsearch_p(void*base, void*key, int n, int elemsize, int(*cmpfn)(void*vp1, void*vp2))
{
	for(int i=0; i<n; i++)
	{
		void*elemAddr = (char*)base + i*elemsize;
		if(cmpfn(elemAddr, key)==0)  // function pointer
			return elemAddr;
	
	}
	return NULL;

}

/*
 * 	compare function, provided by the caller of lsearch_p();
 * 	判等函数，lsearch_p 函数调用者提供
 */

int IntCmp(void*elem1, void*elem2)
{
	int *ip1 = elem1;
	int *ip2=  elem2;
	return *ip2-*ip1; 
}

void show(int*arr, int size)
{
	for(int i=0; i<size; i++)
		printf("%d ", arr[i]);

	putchar('\n');

}














