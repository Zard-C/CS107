# Let 11 

主要内容：

- c语言和c++代码在汇编层面上的一致性
- 编译器是如何看待引用和指针的
- c++ 中的class 和struct
- 预处理：#define



## 1.swap

**c版本的swap**

```c
void foo()
{
    int x;
    int y;
    
    x = 11;
    y = 17;
    swap(&x, &y);
}

void swap(int* ap, int* bp)
{
    int temp = *ap; 
    *ap = *bp; 
    *bp = temp;
}
```

**汇编代码**

```assembly
<foo>:
# 为局部变量申请空间
SP = SP - 8; 
M[SP + 4] = 11;	// x = 11;  
M[SP] = 17; 	// y = 17; 

R1 = SP; 		// &y
R2 = SP + 4; 	// &x
# 为函数参数申请空间
SP = SP - 8;
# 函数参数入栈 按照从右向左的顺序
M[SP] = R2;		
M[SP + 4] = R1; 
CALL <swap> 
# 回收函数参数的空间
SP = SP - 8; 
# 回收局部变量的空间
SP = SP + 8; 
RET;


<swap>:
SP = SP - 4;
# int temp = *ap; 
R1 = M[SP + 8];
R2 = M[R1]; 
M[SP] = R2; 
# *ap = * bp; 
R1 = M[SP + 12]; // bp
R2 = M[R1]; 	 // *bp
R3 = M[SP + 8];  // ap
M[R3] = R2; 	 // *ap = *bp
# *bp = temp; 
R1 = M[Sp + 12]; 
R2 = M[SP]; 
M[R1] = R2; 

# deallocate space for temp 
SP = SP + 4; 
RET; 

```

**C++ 版本的swap**

```c++
void swap(int &a, int &b)
{
    int temp = a; 
    a = b;
    b = temp;
}

void test()
{
    int x; 
    int y; 
    
    x = 11; 
    y = 17;
    swap(x, y);	// 编译器实际上是会传进来地址
}
```

编译器对引用的实现实际上是对指针进行自动的解引用【左值引用】

**汇编代码**

```assembly
```

