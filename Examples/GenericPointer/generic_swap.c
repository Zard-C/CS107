#include <stdio.h>
#include <memory.h>
void swap(void*, void*, ssize_t );

int main(void)
{
	double a =3.0;
	double b =5.0;
	
	printf("Before: a = %.2lf, b = %.2lf\n", a, b);
	swap(&a, &b, sizeof(double));	
	printf("After : a = %.2lf, b = %.2lf\n", a, b);
	
	printf("Done.\n");
	return 0;
}

/* 
 * generic swap function, set the size of data by the caller
 * 通用swap函数， 由调用者通过函数参数指定数据的大小
 * 使用轮转换位策略
 * 函数参数： 
 * 			void* vp1 		元素a的地址								
 * 			void* vp2 		元素b的地址
 * 			ssize_t size	元素的大小	 
 */

void swap(void*vp1, void*vp2, ssize_t size)
{
	char buffer[size];
	// 轮转换位
	memcpy(buffer, vp1, size);
	memcpy(vp1, vp2, size);
	memcpy(vp2, buffer, size);
}




