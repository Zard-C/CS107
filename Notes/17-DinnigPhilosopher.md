# Dinning Philosopher

- [x] 尽可能用最小的改动来移除死锁，留给线程调度器更多弹性空间
- [x] 不要使用busy-waiting, 这样会浪费处理器的时间片
- [x] 使用信号量来实现多线程之间connection
- [ ] Ice Cream Store Simulation

## 1.Dinning Philosopher

### Problem

possible mutual deadlock

```c
Semaphore forks[] = {1,1,1,1,1}	// shorthand 5 global semaphores


void Philosopher (int id)
{
    for(int i = 0; i < 3; i++)
    {
        Think();
        // take right hand
        sw(forks[id]);
        // take left hand, consider all 5 threads could be swapped out right here 1
        sw(forks[(id + 1)% 5 ]);
        Eat();
        
        ss(forks[id]);
        ss(forks[(id + 1) % 5]);
        
    }
    Think();  
}
```

- each semaphore represent the availbility of a resource
- 1 consider that all 5 threads could be swapped out, then if they get the processer again, all 5 threads was blocked, experiencing mutual deadlock.
- one's right fork is another one's left fork. 

### Solution

hint: minimum amount of  work to remove the possibility of  dead lock.

```c
Semaphore forks[] = {1,1,1,1,1} // shorthand 5 global semaphores

Semaphore numAllowedToEat(2);   // at most 2 philosophers 

void Philosopher (int id)
{
    for(int i = 0; i < 3; i++)
    {
        Think();
        
        sw(numAllowedToEat); // only 2 can go forward since at most 2 philosophers are able to eat at any time
        
        // take right hand
        sw(forks[id]);
        // take left hand 1
        sw(forks[(id + 1)% 5 ]);
        Eat();
        
        ss(forks[id]);
        ss(forks[(id + 1) % 5]);
        
        ss(numAllowedToEat)
    }
    Think();  
}
```

- from last lesson, 5 / 2 =2, at most 4 forks can be held while philosophers are eating, which means at most 2 philosophers gonna be able to eat 
- 多达三个线程阻塞在了`sw(numAllowedToEat);`

```c
Semaphore forks[] = {1,1,1,1,1} // shorthand 5 global semaphores

Semaphore numAllowedToEat(4);   // why? 

// we could put a global integer here and said it equaled to 4
// and check to see whether or not it was greater than 0
// and if so, acted on it.
// but it would saperate between test and action 
// that was problematic in the ticket agents example.

// we could also use a binary lock to protect this global integer
// but what would happen?
// you would right some while-loop, around and repeatly check 
// to see whether or not the global variable went positive from 0.
// that is called busy-waiting.

void Philosopher (int id)
{
    for(int i = 0; i < 3; i++)
    {
        Think();
        
        sw(numAllowedToEat); // only 2 can go forward since at most 2 philosophers are able to eat at any time
        
        // take right hand
        sw(forks[id]);
        // take left hand 1
        sw(forks[(id + 1)% 5 ]);
        Eat();
        
        ss(forks[id]);
        ss(forks[(id + 1) % 5]);
        
        ss(numAllowedToEat)
    }
    Think();  
}
```

- if `Semaphore numAllowedToEat(5)`,  `sw(numAllowedToEat)` is useless.
- 如果1个线程被阻塞在`sw(numAllowedToEat);`, 其他4个线程可以继续进行下面的拿叉子操作，那么至少有一个线程可以拿起两个叉子（可能其他3个线程也阻塞了），然后走出这个critical region, 释放叉子让其他线程来使用。
- 这是避免deadlock 需要在代码中所做的最小的改动
- 原因: 所做的改动越小，避免了deadlock，并且给线程调度器留出足够多的弹性空间去进行调度, 至少有4个线程可以尽量向前执行直到被阻塞，如果填2的话会过早的钝化某些线程。

从设计的角度来说，2和4都是正确的，但是我们更喜欢给调度器留出更多的弹性空间。

### Busy waiting

demo code.

```c
while(true)
{
    lock();
    if(global_variable > 0)
    {
        break;
    }
    unlock();
}
```

- 如果总是while循环，我们无法做一些有意义的事情直到跳出while循环
- 在时间片里面只是为了确认变量是否还为0 而不断的重复获取和释放锁
- 如果有很多个处理器，busy-waiting 是有意义的，因为其他线程可以在别的处理器上运行并改变这个值，来很快的释放这个资源。
- 假设我们处于一个单核心处理器的环境下，没有理由用busy-waiting不断的检查一个全局变量的值，因为在当前时间片内这个值是不会改变的，除非等待线程被换出处理器，其他线程得以被执行。
- `sw(numAllowedToEat);` 如果阻塞在这里，线程管理器将该线程放到`blocked queue`, 只有当其他线程释放了该信号量，线程管理器才会将它放到`ready queue` ,处理器才会考虑执行该线程。
- busy-waiting 是不得已而为之的方法，因为它会浪费处理器的时间, 这个时间片完全可以用来执行其他的线程
- busy-waiting 和 spinlock并不是一个概念，spinlock会动态调整等待的次数，必要的时候会yeild（参考boost::spinlock实现）

### Demo code

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define PHILOSOPHERS 5

sem_t forks[PHILOSOPHERS];
pthread_t philosophers[PHILOSOPHERS];

/* mocking eat, but we are really using cpu*/
void eat()
{
    for (int i = 0; i < 1000000; i++)
    {
        asm("nop");
    }
}

void think()
{
    usleep(10);
}

void *philosopher(void *arg)
{
    int id = (int)arg;
    int left = id;
    int right = (id + 1) % PHILOSOPHERS;

    while (1)
    {
        printf("Philosopher %d is thinking\n", id);
        think();
        printf("Philosopher %d is hungry\n", id);
        sem_wait(&forks[left]);
        sem_wait(&forks[right]);
        eat();
        sem_post(&forks[left]);
        sem_post(&forks[right]);
        printf("Philosopher %d done eating\n", id);
    }
}

int main()
{
    for (int i = 0; i < PHILOSOPHERS; i++)
    {
        sem_init(&forks[i], 0, 1);
    }

    for (int i = 0; i < PHILOSOPHERS; i++)
    {
        pthread_create(&philosophers[i], NULL, philosopher, (void *)i);
    }

    for (int i = 0; i < PHILOSOPHERS; i++)
    {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}

```

wait for the freezing console output.

```shell
Philosopher 1 is thinking
Philosopher 1 is hungry
Philosopher 0 done eating
Philosopher 0 is thinking
Philosopher 0 is hungry
Philosopher 4 done eating
Philosopher 4 is thinking
Philosopher 4 is hungry
Philosopher 3 done eating
Philosopher 3 is thinking
Philosopher 3 is hungry
Philosopher 2 done eating
Philosopher 2 is thinking
Philosopher 2 is hungry
Philosopher 1 done eating
Philosopher 1 is thinking
Philosopher 1 is hungry
Philosopher 0 done eating
Philosopher 0 is thinking
Philosopher 0 is hungry
Philosopher 4 done eating
Philosopher 4 is thinking
Philosopher 4 is hungry
Philosopher 3 done eating
Philosopher 3 is thinking
Philosopher 3 is hungry

```

use gdb attach to see the status of threads

```shell
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".
__pthread_clockjoin_ex (threadid=140191941629696, thread_return=0x0, clockid=<optimized out>, abstime=<optimized out>, block=<optimized out>) at pthread_join_common.c:145
145     pthread_join_common.c: No such file or directory.
(gdb) info thread 
  Id   Target Id                                 Frame 
* 1    Thread 0x7f80fae1a740 (LWP 73887) "a.out" __pthread_clockjoin_ex (threadid=140191941629696, thread_return=0x0, clockid=<optimized out>, abstime=<optimized out>, 
    block=<optimized out>) at pthread_join_common.c:145
  2    Thread 0x7f80fae19700 (LWP 73888) "a.out" futex_abstimed_wait_cancelable (private=0, abstime=0x0, clockid=0, expected=0, futex_word=0x563e905d2060 <forks+32>)
    at ../sysdeps/nptl/futex-internal.h:320
  3    Thread 0x7f80fa618700 (LWP 73889) "a.out" futex_abstimed_wait_cancelable (private=0, abstime=0x0, clockid=0, expected=0, futex_word=0x563e905d2080 <forks+64>)
    at ../sysdeps/nptl/futex-internal.h:320
  4    Thread 0x7f80f1e17700 (LWP 73890) "a.out" futex_abstimed_wait_cancelable (private=0, abstime=0x0, clockid=0, expected=0, futex_word=0x563e905d20a0 <forks+96>)
    at ../sysdeps/nptl/futex-internal.h:320
  5    Thread 0x7f80f9e17700 (LWP 73891) "a.out" futex_abstimed_wait_cancelable (private=0, abstime=0x0, clockid=0, expected=0, futex_word=0x563e905d20c0 <forks+128>)
    at ../sysdeps/nptl/futex-internal.h:320
  6    Thread 0x7f80f9616700 (LWP 73892) "a.out" futex_abstimed_wait_cancelable (private=0, abstime=0x0, clockid=0, expected=0, futex_word=0x563e905d2040 <forks>)
    at ../sysdeps/nptl/futex-internal.h:320
(gdb) 
```

thread 2 - 6 (philosopher thread) are all blocked.

## 2. Multi-thread File Downloading

- [x] 使用信号量来实现多线程之间connection

```c
// return value is the total bytes of the file. 
int DownloadSingleFile(const char* server, const char* path);

// returns sum of all file bytes.
int DownloadAllFiles(const char* server, const char* files[], int n)
{
   // rather to spawn n threads 
   int totalBytes = 0; 
   // ThreadNew doesn't returen anything.
   Semaphore lock = 1; // short hand 
   for(int i = 0; i < n; ++i)
   {
       ThreadNew(-, DownloadHelper, 4, server, files[i], &totalBytes, lock);
   }
    
   return totalBytes;
}

// proxy function DownloadHelper 

void DownloadHelper(const char* server, 
                    const char* path, 
                    int* numBytesp,
                    Semaphore lock)
{
    // another n -1 thread also trying to call this function
    // to download the files in parallel.
    int bytesDownloaded = 
        DownloadSingleFile(server, path);
    SW(lock);
    *numBytesp += bytesDownloaded;
    SS(lock);
}
```

- 需要一个DownloadHelper, 因为ThreadNew 返回值是void

```c
void DownloadHelper(const char* server, 
                    const char* path, 
                    int* numBytesp,
                    Semaphore lock)
{
    // What if get the lock before DownloadSingleFile
    SW(lock);
    int bytesDownloaded = 
        DownloadSingleFile(server, path);
    *numBytesp += bytesDownloaded;
    SS(lock);
}
```

- 还是会返回的，但是感觉会很慢，就像你没有使用多线程一样，按照顺序下载文件
- 将 `DownloadSingleFile`放在了临界区域，那么至多有一个线程在下载文件

Q：如果有10000个文件需要下载，也要创建n个线程吗？

A： 当前假设只有40个或者至多100个文件要下载，操作系统可以~~重用~~产生(spawn off)的线程数量是有上限的。有些操作系统可以回收完成任务的线程空间。

### Problem

注意看一下DownloadAllFiles这个实现，在for循环执行了n次ThreadNew之后很快就走到了return这里。然而此时这些下载线程可能没有运行或者没有运行完毕，考虑下载文件其实是一个很耗时的操作，我们大概率会得到一个0的返回值。

我们要做的是让当前线程阻塞在return 之前，直到所有下载线程都完成了下载任务。

### Solution

thread to thread communication.

1 - n relationship: one thread spawn off and  wait for n threads done.

use concurrency and semaphores.

```c
int DownloadAllFiles(const char* server, const char* files[], int n)
{
    
   Semaphore childrenDone = 0;  // use this sem as connection
 
   int totalBytes = 0; 
   Semaphore lock = 1;
   for(int i = 0; i < n; ++i)
   {
       ThreadNew(-, DownloadHelper, 5, server, files[i], &totalBytes, lock, childrenDone);  // 5 params, pass childrenDone
   }
    
    
    // wait n threads done.
    // technically it's not busy-waiting.
    for(int i = 0; i < n; i++)
    {
        SW(childrenDone);
    }
    
   return totalBytes;
}

void DownloadHelper(const char* server, 
                    const char* path, 
                    int* numBytesp,
                    Semaphore lock,
                    parentToSignal)
{
    int bytesDownloaded = 
        DownloadSingleFile(server, path);
    SW(lock);
    *numBytesp += bytesDownloaded;
    SS(lock);
    SS(parentToSignal);     // waaah ~
}
```

Q&A

你不能相信从一个信号量那里查询来的值，因为在代码执行过程中可能有其他线程将这个值修改了(SW, SS)，其他库中可以指定消耗信号量的值。

两个单独下载的文件可能保存在主机的同一个路径下面，这有点麻烦，除非操作系统上的m ake directory 命令是原子类型的。

有一些库可以将信号量初始化为负值

将totalBytes替换成数组，每个线程都去修改自己的numBytes，这样可以消除一部分竞态条件，当然这也要等待childrenDone之后再去将数组中的值求和并返回，这是一个很好的想法，唯一要考虑的是n可能会很大。

### Scenario

`DownloadAllFiles` 可能会在 `DownloadHelper` 之前返回，但这不影响结果的正确性

考虑：

1. 所有`DownloadHelper`执行到了 `SS(parentToSignal);`之后，但是`}`之前就失去了cpu
2. 这时恰好`DownloadAllFiles` 的线程被调度到了，然后迅速执行完循环然后退出。

## 3.Ice Cream Store Simulation

The largest example.

- 10 Customers  [1-4 ice creams to order]
- 1 Casher
- 1 Manager thread.
- 10 - 40 Clerks: customers are mean, they want the ice cream to be made in parallel .

![Ice Cream Store](17-DinnigPhilosopher.assets/ice_cream_store_simulation.png)
