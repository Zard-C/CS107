# Ways to use semaphore

- Ticket Agent, part II
- Ringbuffer: another way to use semaphore
- The Dinnig Philosophers, part I

## previous code （Ticket Agent)

 ```c
 void SellTickets(int agent, int* numTicketsp, Semaphore lock)
 {
     while(true)
     {
       SemaphoreWait(lock);
       // critical region start
       if(*numTicketsp == 0)
       {
           break;    // 1⃣️
       }
       (*numTicktesp)--;
       // critical region end
       SemaphoreSignal(lock);  
       // you may want to write some log
       // sleep a little while
     }
     SemaphoreSignal(lock);  // 对应1⃣️处break的情况，需要释放资源 
     // every SemaphoreWait is bounced back by SemaphoreSignal
 }
 
 semaphore_wait();      // 同步地将变量-1
 semaphore_signal();    // 同步地将变量+1
 ```

任意一个到达临界区域(SemaphoreWait)的线程都会遇到两种情况：

1. semaphore的值为1(因为这个semaphore被初始化为1，并且wait和signal 一一对应)，它实际上对semaphore减一，通过SemaphoreWait并进入临界区域获取资源并进行处理。
2. semaphore的值为0，线程因此在这里阻塞，是因为有一个线程通过了SemaphoreWait，但是还没有SemaphoreSignal，可能这是这个线程的时间片就已经结束了，也可能在多个处理器工作的情况下两个线程同时在被执行。即便是一个线程拥有lock，这也并不意味着这个线程不会调度器从运行态调度到就绪态（即这个线程也会在时间片耗尽使失去处理器) 阻塞在这里的线程因为不能进行任何工作所以会让出时间片到就绪态，最终唯一可以工作的线程会拿到时间片，将Semaphore变成1。

通常情况下尽量保持尽可能小的临界区域: You can only do this when you have to.

特别是这个打印函数，只是将信息输出到控制台，所以没有必要放到临界区域中

### semaphore

`semaphore`: a basically, let's say a synchronized counter variable. (>=0)，要注意的是在cs107使用的系统中semaphore_wai t不允许将变量变成负数。这意味着，当计数变量变为0时，需要其他线程中调用semaphore_signal将变量++，这样才能通过semaphore_wait将变量减1而不会变成负数。

在实现上semaphore_wait是使用线程库

"OK, I can't make any progress right now. It pulls itself off the processor, and records itself as something that's called blocked, and it puts it in this cue of threads are not allowed to make progess until some other threads signals a semaphore they're waiting on "

这段解释提到的是一个线程（一个进程中的执行单元）意识到它无法继续向前推进的情况（可能是因为缺乏所需的资源或者因为它正在等待另一个操作完成）。因此，它停止了执行（‘从处理器上移除自己’）并将其状态改变为‘阻塞’。阻塞意味着线程是不活动的，并且不能取得进展。这个线程随后被放置在一个阻塞线程的队列中。线程将保持在这种状态，直到满足特定条件 — 通常，这个条件是由另一个线程通过信号量发出的。信号量是一种同步机制，用于在并发系统中控制对共享资源的访问。当信号量被发出时，它表示阻塞线程所等待的资源或条件现在可用，因此阻塞线程可以离开阻塞队列并恢复执行。

在这个demo code 中，信号量被初始化为1，这是因为我们想用它当作`true`来进行处理，它的作用类似于一个开关的开启和关闭(对应0和1)，当资源可用时为1，线程可以拿到资源进行处理；当资源被占用时为0，线程需要等待其他线程释放资源。在这里需要被保护的资源就是numTickets.

### what happens when race condition

当线程从被从处理器中换出时(swapped out)，它们的所有线程堆栈信息，寄存器会被存储下来.

没有semaphore的情况时,假设此时numTickets变量为100.

```c
(*numTicktesp)--; // 这一行代码会变成很多的汇编指令，简单一点解释就是load - sub -store
```

```shell
        ldr     r2, [r3]    ;load r2:100
        sub     r2, r2, #1  ;sub  r2:99
        str     r2, [r3]    ;store
```

当被换出处理器时，所有寄存器的值,都会被拷贝到被换出的线程底部小的堆栈结构，当它重新得到处理器时，则为该线程恢复寄存器设置，这也就是所谓的`context-switch`

如果线程在没有store的情况下就被换出了处理器，那么这个寄存器中的值也被保存了下来，此时另外一个线程可能会完成了load-sub-store 的完整过程，变量也就此更新为了99。这个被换出的线程又重新得到了处理器，此时完成上下文切换，寄存器的值恢复，那么此时完成store。

如果不使用Semaphore，那么在这种情况下，一个线程使用99覆盖了之前的变量值100，另一个99覆盖了其他线程的99。这不是我们想要的，即一张票被卖出了两次。

Semaphore的作用是只允许一个线程通过，把全局的值放入局部寄存器中，在线程内部中进行递减，然后更新为全局的整数。

如果我们将sempahore初始化为0，实际上会阻止所有线程进入临界区域，这就是一个死锁`dead-lock`

如果我们将semaphore初始化为2，这基本上和初始化为10一样坏，因为我们不想让多个线程在同一时刻进入critical region.

### context

线程的上下文(context)是指在特定时刻线程所包含的所有状态信息。这个信息使得操作系统能够在多个线程间进行切换，而每个线程能够恢复到正确的状态继续执行。线程上下文通常包括以下几个主要部分：

1. **CPU寄存器状态**：这包括了通用寄存器、指令计数器（指示下一条执行指令的位置）、堆栈指针（指向线程堆栈的顶部）和状态寄存器等。这些寄存器存储了线程在特定时刻的运行状态。

2. **程序计数器（Program Counter）**：程序计数器存储着线程将要执行的下一条指令的地址。它指示了线程执行的当前位置。

3. **堆栈信息**：线程的堆栈包括了函数调用的历史、局部变量、返回地址等信息。当线程进行函数调用时，会将相关信息压入其堆栈，当函数返回时，信息会从堆栈中弹出。

4. **线程的堆栈指针**：这个指针指向线程私有堆栈的当前位置，标识着堆栈中数据的顶部。

5. **线程特定的数据**：比如线程的优先级、状态（如就绪、运行、等待）和线程本地存储等。

6. **上下文切换**：当操作系统切换到另一个线程时，它会保存当前线程的上下文，并加载另一个线程的上下文。这使得每个线程似乎拥有连续的执行流，尽管实际上它们可能被频繁地中断和恢复。

因此，线程的上下文不仅包括了堆栈信息，还包括了CPU寄存器的状态和其他与线程运行状态相关的信息。这些信息对于线程的暂停和恢复至关重要。

其中堆栈信息在英语中通常表达为 "Stack Information"。在计算机科学和编程领域，这个术语通常指的是存储在程序的调用栈（call stack）上的数据，包括局部变量、函数调用的顺序和返回地址等，其实应该特指调用栈。

## Ringbuffer Example

**global variable is convenient, and also awful.**

使用ringbuffer 来模拟写入和读取的过程, 假设要读40字节的消息，需要循环5次.

1. Writer thread 写入40字符
2. Reader thread 读取40 字符
3. 我们希望reader 能够尽快读取，以免writer太快了导致覆盖没有被读取的数据.

### first version

```c
char buffer[8];

void Writer()
{
    for(int i = 0; i < 40; i++)
    {
        // 假设 prepareRandomChar is thread-safe.
        char c = prepareRandomChar();
        buffer[i % 8] = c;
    }
}

void Reader()
{
    for(int i = 0; i < 40; i++)
    {
        // 从全局空间取到local space 然后可以进行任意的处理
        char c = buffer[i % 8];
        // processChar(c);
    }
}

int main()
{
    ITP(false);
    ThreadNew("Writer", Writer, 0);
    ThreadNew("Reader", Reader, 0);
    RunAllThread();
}
```

问题在于如果reader需要操作的时间比较久，这可能导致

1. writer覆盖了没有被reader读取的内容
2. 如果reader先执行，那么它甚至可能读取没有被writer写入的信息

### semaphore version

semaphore 的另一种使用方式

1. 强制writer超前reader一点， reader不可以超过writer
2. writer也不能超过Reader一个周期(cycle ahead)

```c
char buffer[8];
Semaphore emptyBuffers(8); // 最开始有8个buffer为空，  可以被写入
Semaphore fullBuffers(0);  // 最开始有0个buffer被写入，可以被读取

void Writer()
{
    for(int i = 0; i < 40; i++)
    {
        SemaphoreWait(emptyBuffers);
        char c = prepareRandomChar();
        buffer[i % 8] = c;
        SemaphoreSignal(fullBuffers);
    }
}

void Reader()
{
    for(int i = 0; i < 40; i++)
    {
        // 从全局空间取到local space 然后可以进行任意的处理
        SemaphoreWait(fullBuffers);
        char c = buffer[i % 8];
        // processChar(c);
        SemaphoreSignal(emptyBuffers);
    }
}

int main()
{
    ITP(false);
    ThreadNew("Writer", Writer, 0);
    ThreadNew("Reader", Reader, 0);
    RunAllThread();
}
```

注意代码中的Ss和Sw，它们是互相对应两个的线程中的sw和ss

#### scenario 1

reader goes first

执行到`SemaphoreWait(fullBuffers);` 然后立即阻塞，因为它要wait的东西实际上是0，just like we want it to be

#### scenario 2

writer goes first

Writer 线程很快，并且高效地写完了8个buffer，then it hits a wall. 因为此时Reader还没有从缓冲区中拿出数据。

TicketAgent 示例中使用了一个Semaphore，并且在上面Signal 和 Wait，这种使用方式可视作一个`binary lock`, 因为Semaphore的值只有0和1.

Ringbuffer 示例则展现了Semaphore在多线程作为通信的另一个用途，这种使用方式类似于打电话，两个线程之间互相进行通知。当其他进程获得一些必须的进展之前，当前线程无法进一步工作。

#### discussion

```c
char buffer[8];
Semaphore emptyBuffers(4); // 最开始有4个buffer为空，  可以被写入
Semaphore fullBuffers(0);  // 最开始有0个buffer被写入，可以被读取
```

不会改变代码的正确性(取决你如何进行定义)，并且也不会得到死锁，这样只是限制了writer的自由度(由可以超前reader 8个字节变成了4个字节)

```c
char buffer[8];
Semaphore emptyBuffers(1); // 最开始有1个buffer为空，  可以被写入
Semaphore fullBuffers(0);  // 最开始有0个buffer被写入，可以被读取
```

更进一步限制了writer的自由度，只能在这两个函数read和write之间进行交替

```c
char buffer[8];
Semaphore emptyBuffers(0); // 最开始有0个buffer为空，  可以被写入
Semaphore fullBuffers(0);  // 最开始有0个buffer被写入，可以被读取
```

这就得到了一个死锁，互相都在等待对方取得进展。

Reader: I can't do anything, cause I have no place to read from

Writer: I can't do anything either, cause I have no place to write to.

如果**你的代码中有很多Semaphore时，你很容易在其中进行剪切和粘贴中犯这种错误**

死锁的时候，一切都似乎停止了，不会再有消息发送到控制台，程序也不会返回，两个线程在都在等待对方，或者说，没有人释放锁(或者类似的东西)。

```c
char buffer[8];
Semaphore emptyBuffers(7); // 最开始有7个buffer为空，  可以被写入
Semaphore fullBuffers(1);  // 最开始有1个buffer被写入，可以被读取
```

实际上允许Reader线程领先Writer线程一跳(hop)

```c
char buffer[8];
Semaphore emptyBuffers(16); // 最开始有16个buffer为空，  可以被写入
Semaphore fullBuffers(0);  // 最开始有0个buffer被写入，可以被读取
```

实际上为了确保编程的正确性，这个值应该是[1,8]中间的一个值，设置为16表示Writer允许使用两个循环领先Reader，这是不正确的，因为它最多也只能提前8个slots以免没有读取的数据被覆盖。

最佳的答案是8：如果你有多种选择来初始化这个信号量，我们应该用最大的灵活性来授权线程调度器，而且它也提高了每个线程能拥有尽可能长的时间片。

#### binary lock

SemaphoreWait和SemaphoreSignal 采用的是二进制锁的思想，当它的确是个锁的时候，会有`acquire`和`release`作为动词来获取和释放锁，有些版本的线程库实际上定义了锁这种类型，以及相应的获取和释放动作，这也是SemphoreWait和SemaphoreSignal 的封装器

#### Think about

有多个Writer的情况：需要额外的变量（二进制锁），以免两个以上的Writer线程同时进入critical region.

如果有多个Reader的时候，也需要做类似的工作。

## Dining Philosophers Problem

![image-20240123003622705](16-Semaphore.assets/DiningPhilosopher.png)

使用c线程的方式模拟哲学家干饭的问题，每个哲学家都是讲究人，需要同时拿起左手和右手的叉子才能干饭。

```c
Semaphore forks[] = {1,1,1,1,1};

void Philosopher(int id)
{
    for(int i = 0; i < 3; i++)
    {
        Think();    // assume that it's thread-safe
        
        SemaphoreWait(forks[id]);
        SemaphoreWait(forks[(id + 1)%5]);
        Eat();
        SemaphoreSignal(forks[id]);
        SemaphoreSignal(forks[(id + 1)%5]);
        //
    }
    
}
```

### Assumption

1. Phi.0 思考了一会，想要干饭，拿起了左边的叉子fork0, 然后被换出了处理器，这就让fork0 变成了一个不可用的资源。
2. Phi.1 思考了一会，想要干饭，拿起了左边的叉子fork1, 然后被换出了处理器，这就让fork1 变成了一个不可用的资源。
3. Phi.2 思考了一会，想要干饭，拿起了左边的叉子fork2, 然后被换出了处理器，这就让fork2 变成了一个不可用的资源。
4. Phi.3 思考了一会，想要干饭，拿起了左边的叉子fork3, 然后被换出了处理器，这就让fork3 变成了一个不可用的资源。
5. Phi.4 思考了一会，想要干饭，拿起了左边的叉子fork4, 然后被换出了处理器，这就让fork4 变成了一个不可用的资源。

这样会导致所有人都不可能拿到右手边的叉子了，5个线程中有互相的死锁，因为每个哲学家拥有它左边的哲学家需要的资源。

### CS107's solution

有多种方法来移除死锁的问题：

- 可以交替让哲学家获得奇数/偶数下标的fork

我们的做法是以最小的代码/逻辑改动为代价来移除死锁，同时仍然保证最大的线程数量，任何线程在其时间片内处理的工作量保持不变。

首先，我们可以将这个地方变成critical region

```c
        
        SemaphoreWait(forks[id]);
        SemaphoreWait(forks[(id + 1)%5]);
        Eat();
        SemaphoreSignal(forks[id]);
        SemaphoreSignal(forks[(id + 1)%5]);
        //
```

这样的话至多有一个哲学家能干饭，其他哲学家只能干瞪眼，这与我们的初衷不符。

通过瞪眼法观察得出，实际上在同一时间内最多也只能有两个哲学家同时干饭（资源限制，因为只有5个fork，如果3个哲学家同时干饭的话我们将需要6个fork):

1. 我们可以阻止1个哲学家获得fork，即4个哲学家可以尝试去干饭
2. 最终只有两个哲学家能够同时干饭

```c
Semaphore forks[] = {1,1,1,1,1};
Semaphore numAllowedToEat(4);

void Philosopher(int id)
{
    for(int i = 0; i < 3; i++)
    {
        Think();    // assume that it's thread-safe
        
        //  SW numAllowedToEat
        SemaphoreWait(forks[id]);
        SemaphoreWait(forks[(id + 1)%5]);
        Eat();
        SemaphoreSignal(forks[id]);
        SemaphoreSignal(forks[(id + 1)%5]);
        //  SS numAllowedToEat
    }
    
}
```
