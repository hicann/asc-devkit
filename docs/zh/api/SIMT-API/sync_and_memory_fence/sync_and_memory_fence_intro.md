# 同步与内存栅栏简介

SIMT程序中，不同Thread的执行进度和内存访问到达顺序可能不同。当线程之间存在数据依赖时，需要使用同步接口或内存栅栏接口约束执行顺序和内存可见性。

同步机制主要分为两类：

-   **同步屏障（barrier）**：要求同一作用范围内的线程都到达指定位置后，程序才能继续执行。
-   **内存栅栏（memory fence）**：约束调用线程在栅栏前后的内存访问顺序，使栅栏前的内存操作按指定范围对其他线程可见。内存栅栏不会等待其他线程到达同一位置。

## API列表

| 类型 | 接口名 | 作用范围 | 是否阻塞线程 | 功能说明 |
| --- | --- | --- | --- | --- |
| 同步屏障 | [asc\_syncthreads](sync_interface/asc_syncthreads.md) | 当前Thread Block | 是 | 等待当前Thread Block内所有Thread都执行到该同步点，并保证同步点前的内存操作对块内线程可见。 |
| 内存栅栏 | [asc\_threadfence\_block](memory_fence/asc_threadfence_block.md) | 当前Thread Block | 否 | 保证调用线程在栅栏前的内存操作，按顺序对当前Thread Block内的其他线程可见。 |
| 内存栅栏 | [asc\_threadfence](memory_fence/asc_threadfence.md) | 全局范围 | 否 | 保证调用线程在栅栏前的全局内存和共享内存写操作，按顺序对其他线程可见。 |

## 同步屏障

`asc_syncthreads`用于Thread Block内的阶段同步。典型场景是多个Thread先写入Unified Buffer（UB）中的共享数据，再统一进入下一阶段读取这些数据。

```cpp
__global__ __launch_bounds__(256) void block_reduce(float *out, const float *in)
{
    __ubuf__ float buf[256];
    uint32_t tid = threadIdx.x;

    buf[tid] = in[blockIdx.x * blockDim.x + tid];

    // 等待当前Thread Block内所有Thread完成buf写入。
    asc_syncthreads();

    if (tid == 0) {
        float sum = 0.0f;
        for (uint32_t i = 0; i < blockDim.x; ++i) {
            sum += buf[i];
        }
        out[blockIdx.x] = sum;
    }
}
```

使用`asc_syncthreads`时需要注意：

-   同一Thread Block内的所有Thread必须都执行到该同步点，否则已经到达同步点的Thread会一直等待，导致程序无法继续执行。
-   避免在分支中调用`asc_syncthreads`，除非可以保证当前Thread Block内所有Thread都会进入该分支。
-   `asc_syncthreads`只同步当前Thread Block内的Thread，不能用于不同Thread Block之间的全局同步。

## 内存栅栏

内存栅栏用于约束调用线程的内存访问顺序。它解决的是“某个线程先写数据、再发布状态或标志”这类可见性问题，但不会阻塞其他线程，也不会让其他线程自动等待。

典型的生产者-消费者场景如下：

```cpp
data[idx] = value;

// 确保data写入先于后续标志位更新对其他线程可见。
asc_threadfence();

asc_atomic_exch(ready, 1U);
```

在上述场景中，`asc_threadfence`保证调用线程在栅栏前的数据写入先于标志位更新对其他线程可见。消费者线程仍需要通过轮询、原子操作或其他同步方式判断`ready`状态，内存栅栏本身不会等待消费者线程。

`asc_threadfence_block`与`asc_threadfence`的区别在于作用范围不同：

-   `asc_threadfence_block`用于Thread Block内的数据可见性顺序约束，适合块内共享数据和UB协作场景。
-   `asc_threadfence`用于更大范围的数据可见性顺序约束，适合通过Global Memory发布数据或标志的场景。

## 使用建议

-   需要Thread Block内所有Thread完成某一阶段后再继续执行时，使用`asc_syncthreads`。
-   只需要保证当前Thread的内存写入顺序和可见性时，使用`asc_threadfence_block`或`asc_threadfence`。
-   不要将内存栅栏当作同步屏障使用。内存栅栏不会等待其他Thread，也不保证其他Thread已经执行到同一位置。
-   不要将`asc_syncthreads`当作跨Thread Block同步接口使用。不同Thread Block之间如需共享状态，应结合Global Memory、[原子操作](../atomic_operations/atomic_operations.md)和内存栅栏设计同步协议。
-   多个Thread并发写同一地址时，`asc_syncthreads`和内存栅栏都不能替代原子操作；需要避免写冲突，或使用原子操作保证读改写过程的原子性。
