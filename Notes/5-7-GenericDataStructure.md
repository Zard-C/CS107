# Lec 5 -7

在没有类，模板的情况下实现通用数据结构

.h 和 .c 方式将行为描述和具体实现分开。

### 1.1 int 类型stack

```c
typedef struct 
{
  	int *elems;
  	int logicalLen;
  	int alloclength;
}Stack;

// 构造函数
int stackNew(Stack * s); // 显式实现 this指针
// 析构函数
void stackDispose(Stack *s);

void stackPush(Stack *s, int value);

int stackPop(Stack *s);


Stack s; 
stackNew(&s);
for(int i = 0; i < 5; ++i)
{
  	stackPush(&s, i);
}
stackDispose(&s); 

int stackNew(Stack* s)
{
  	s->logicalLen = 0;
  	s->alloclength = 4;
  	s->elems = malloc(4 *sizeof(int)); // new 会隐式地考虑权限。
  	
  	// assert(s->elems != NULL);
  	if(s->elems != NULL)
    {
      return 0;
    }
  	return -1; 
}


void stackDispose(Stack * s)
{
  	free(s->elems);	// s->elems 中保存的变量该如何处理？
//free(s); ? stack 已经被分配好，即使需要free也是上层调用者来完成。
}

void stackPush(Stack * s, int value)
{
		if(s->logicalLen == s->allocLength)
    {
      	s->allocLength *= 2; 
    	  realloc(s->elems, s->allocLength * sizeof(int));// 别忘了size ,捕获返回值非常重要。
    }
  	// check for NULL;
  
  	s->elems[s->logicalLenth] = value;
		s->logicalLenth++; 
}	


int stackPop(Stack * s)
{
  	assert(s->logicalLen > 0);
  	s->logicalLen --;
  	// 为了提高运行速度，通常会忽略掉缩小申请的堆内存的操作
  	return s->elems[s->logicalLen]; 
}
```

1. 将结构体看成黑盒，不直接使用使用函数来操纵结构体成员变量。
2. 在c++中可以有privtae关键字指出类中的成员是私有的。不能被外部访问，但是在c中整个结构体都暴露在外，任何人都可以访问，在c++中struct和class都可以声明一个类。区别在于struct声明的类成员变量默认是public而class声明的成员变量默认是private。作者需要写一个文档来指出它的内部结构不可见。
3. 申请的空间不够时，使用加倍策略要重新向操作系统申请空间，并将之前的数据复制过来，再进行新数据的拷贝。
4. 通过传入待操作的数据结构进行操作。

```c
// realloc 函数
void* realloc(void*, size_t);

 The realloc() function tries to change the size of the allocation pointed to by ptr to size,  and returns ptr.  If there is not enough room to enlarge the memory allocation pointed to by ptr, realloc() creates a new allocation, copies as much of the old data pointed to by ptr as will fit to the new allocation, frees the old allocation, and returns a pointer to the allocated memory.  If ptr is NULL, realloc() is identical to a call to malloc() for size bytes.  If size is zero and ptr is not NULL, a new, minimum sized object is allocated and the original object is freed.  When extending a region allocated with calloc(3), realloc(3) does not guarantee that the additional memory is also zero-filled. 
   O(m), m is the size of the heap
```

### 1.2 Generic Stack

1. 失去了元素类型信息：大小
2. 不能进行指针运算



```c
// stack.h 
typedef struct 
{
	void *elems; 
  	int elemSize;
  	int loglength;
  	int allocLength;
  	void (*freefn)(void*); 
}stack;

void stackNew(stack * s, int elemSize);
void stackDispose(stack * s); 
void stackPush(stack *s, void * elemAddr); 
void stackPop(stack *s, void *elemAddr); 



// stack.c 
void stackNew(stack *s, int elemSize, void (*freefn)(void *))
{
  	assert(s->elemSize > 0); 
  	s->loglength = 0; 
  	s->allocLength = 4;
  	s->elems = malloc(4 * elemSize); 
  	s->freefn = freefn; 
  	assert(s->elems != NULL);  
}

void stackDispose(stack *s )
{
		if(s->freefn != NULL)
    {
      	for(int i = 0; i < s->logLength; ++i)
        {
          	freefn((char*) s->elems + i * s->elemSize); 
        }
    }
 
  	free(s->elems);   
}

void stackPush(stack *s, void *elemAddr)
{
  	if(s->logLength == s->allocLength)
    {
      	// reallocation part
      	stackGrow(s); 
    }
  	
  	void *target = (char* )s->elems + s->logLength * s->elemSize;
		memcpy(target, elemAddr, s->elemSize); // unsigned long  same size as pointer in 32bit 
  	s->logLength ++; 
}

static	void stackGrow(stack *s) 
{
  	s->allocLength *=2;
  	s->elems = realloc(s->elems, s->allocLength * s->elemSize); 
}

void stackPop(stack *s, void *elemAddr)
{
  	void *source = (char *)s->elems + (s->logLength - 1) * s->elemSize;
  	memcpy(elemAddr, source, s->elemSize); 
  	s->logLength --;
}

```

- 动态申请空间的函数，它也有对偶的函数来负责这些空间的释放工作。
- unit test

```c
void StringFree(void * vp)
{
  	free(*(char**)vp); 
}

int main()
{
  	const char * friends[] = {"Al", "Bob", "Carl"}; 
  	stack StringStack;
  	stackNew(&StringStack, sizeof(char*), StringFree); 
  	
  	for(int i = 0; i < 3; ++i)
    {
      	char *copy = strdup(friends[i]); // deep copy 
      	stackPush(&StringStack, copy); 	
    }
  
  	for(int i = 0; i < 3; ++i)
    {
      	char ** names = NULL;
      	stackPop(&StringStack, &names); 
    }
  	stackDispose(&StringStack); // elems 中的元素应该如何处理？freefn!
}
```

### 2.1 more generic 

#### 2.1.1 rotate

```c
void rotate(void* front, void *middle, void * end)
{
  	// memcpy 无法处理数据重叠的情况，如果调用者不能确保地址是否重叠，那么可以调用一个效率稍低的版本memmove
  	int forntSize = (char*)middle - (char*) front; 
  	int backsize = (char*)end - (char*)middle;
  
  	char buffer[frontsize]; 
  	memcpy(buffer, front, frontsize); 
  	memmove(fornt, middle, backsize); 
  	memcpy((char*)end - frontsize, buffer, frontsize); 
}
```

#### 2.1.2 qsort

```c
 void qsort(void *base, size_t nel, size_t width, int (*compar)(const void *, const void *));
```



查看编译器生成的汇编代码：

https://godbolt.org/ 

