# Lec 14 





# 

```c
void foo() 
{
    int array[4]; 
    int i ; 
    for(i = 0; i <= 4; ++i)
    {
        array[i] -= 4; 
    }
}
```

陷入死循环的原因：缓冲区溢出导致修改了savedPC的值，将savedPC值-4，将本该指向`CALL<foo>`下一条语句的saved PC指向了`CALL <foo>`	当函数返回时，又执行了`CALL<foo>`.实际上是一种递归。

在solaris的内存模型比我们讲到的要复杂一些。它们并不是让savedPC与这些变量直接相邻，不过也不会差太多。

```c
int main(int argc, char**argv)
{
    DeclareAndInitArray(); 
    PrintArray(); 
}
```

