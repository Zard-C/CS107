# Lec 3 指针

## 指针的底层机制

### 指针的强制类型转换

```c
double d = 3.1416; 
char ch = *(char *)&d; 
```

​	先对d取地址，然后将这个地址的起始第一个字节赋值给ch。



```c
short s = 45;
double d = *(double *) &s
```

强制重新解释地址，当成double类型进行解引用操作。这是一个越界访问的行为。

- big endian	0000 0000 0000 0001
- little endian   0001 0000 0000 0000

### 结构体 

```c
struct fraction
{
  	int num;
   	int denum;
}; 

fraction pi; 		/* a storage of fraction type */

pi.num = 22; 
pi.denunm = 7;

((fraction *)&(pi.denum))->num = 12; 
((fraction *)&(pi.denum))->denum = 33; /* illegal way to visit */

/* using array trick to visit  */
(&pi)[1].num; 
(*(&pi + 1)).num; 
(&pi + 1)->num; 
// 具体内容在后面的活动记录中
```

### 数组

```c
int array[10]; 
array[0] = 44;
array[9] = 100;
array[5] = 45;
array[10] = 1; /*no bound check of raw array in c and cpp*/
array[25] = 25; 
array[-4] = 77;
```

#### 指针偏移与下标的关系

- 基于数据类型获得实际偏移的字节数

```shell
				array == &array[0] 
		array + k == &array[k] 
		*array		== array[0]
	*(array + k) == array[k]
```

#### 强制类型转换

```c
int arr[5]; 
arr[3] = 128; 
(short* )arr[6] = 2;
((short *)(((char* )(&arr[1])) + 8))[3] = 100; // (short*)(arr)[9] = 100; 
```

 #### 结构体数组

```c
struct student
{
  	char *name; 
  	char suid[8]; 
  	int numUnits; 
}

student pupils[4]; 
pupils[0].numUnits = 2; 
pupils[2].name = strdup("Adam"); 
pupils[3].name = pupils[0].suid + 6; 

strcpy(pupils[1].suid, "40415xx"); 
strcpy(pupils[3].name, "123456"); 
pupils[7].suid[11] = 'A'; 
```

### swap函数(int 类型)

```c
/* rotating shift */
void swap(int *ap, int *bp)
{
  	int temp = *ap;
  	*ap = *bp;
  	*bp = temp; 
}

int x = 7;
int y = 11; 
swap(&x, &y);
```







