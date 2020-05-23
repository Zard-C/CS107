#include <stdio.h>
#include <stdlib.h>

/*
 *  通过轮转换位算法实现swap函数
 *  函数参数: 两个交换数据类型指针(int*)
 */ 

void swap(int* a, int* b)
{
	int temp =0;
	temp=*a;
	*a=*b;
	*b=temp;
}

int main(int argc, char* argv[])
{
	int a =3;
	int b =4;
	
	printf("Originally: a = %d, b = %d\n", a ,b);
	swap(&a, &b);
	printf("Now:        a = %d, b = %d\n", a, b);
	exit(0);
}
