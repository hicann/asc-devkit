# asc\_threadfence

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence_res.md#id1 -->

## 功能说明

在SIMT编程范式中，来自不同线程对同一份内存的读写操作可能造成**数据竞争**（Data Race），这是由于NPU架构的特性，线程的执行顺序和内存访问顺序可能不一致，从而导致其他线程看到的数据更新顺序与实际写入顺序不同。

内存栅栏类接口可以解决这类多线程同步读写共享内存的问题，其中`asc_threadfence()`接口用于保证所有线程对同一份全局、共享内存的访问过程中，**写入操作的时序性和可见性**。

**关键特征**：

-   该接口**不会阻塞线程**，仅保证内存操作的可见性顺序。
-   作用范围是全局的：调用后，确保调用线程在asc\_threadfence\(\)之前的所有全局内存和共享内存写操作对其他线程可见。

本接口一般适用的场景包括：

-   生产者-消费者同步场景：确保数据写入在标志位设置之前完成。
-   原子计数器场景：在原子计数器更新前确保相关数据已写入。
-   指针更新场景：链表、树等数据结构操作时确保数据一致性
-   多线程共享数据：确保共享数据的更新顺序对其他线程可见。

下图展示了没有内存栅栏时可能出现的内存可见性问题：

![](../../../figures/threadface_memory_visibility.png)

通过使用asc\_threadfence\(\)接口，可解决上述内存可见性问题：

![](../../../figures/threadface_function.png)

## 函数原型

```cpp
inline void asc_threadfence()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_sync_functions.h`头文件。

```cpp
#include "simt_api/device_sync_functions.h"
```

## 调用示例

完整样例请参考[MemoryFence样例](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md)。

以下示例中，假设执行前`data[0]`为旧值、`flag[0]`为0。生产者线程先写入`data[0] = 100`，随后调用`asc_threadfence()`约束全局可见性顺序，再写入`flag[0] = 1`发布数据就绪标志。消费者线程轮询等待`flag[0]`变为1后读取`data[0]`，此时不应读到旧值，而应读到生产者写入的100。

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelThreadFence(int32_t* data, int32_t* flag, int32_t* result)
    {
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[0] = 100;
            asc_threadfence(); // 保证data[0]的写入先于flag[0]的写入对全局可见
            flag[0] = 1;
        }

        if (blockIdx.x == 1 && threadIdx.x == 0) {
            volatile int32_t* flag_volatile = reinterpret_cast<volatile int32_t*>(flag);
            while (flag_volatile[0] == 0) {  // 等待生产者将flag[0]置1
            }
            result[0] = data[0];
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelThreadFence(
        __gm__ int32_t* data, __gm__ int32_t* flag, __gm__ int32_t* result)
    {
        if (blockIdx.x == 0 && threadIdx.x == 0) {
            data[0] = 100;
            asc_threadfence(); // 保证data[0]的写入先于flag[0]的写入对全局可见
            flag[0] = 1;
        }

        if (blockIdx.x == 1 && threadIdx.x == 0) {
            volatile __gm__ int32_t* flag_volatile = reinterpret_cast<volatile __gm__ int32_t*>(flag);
            while (flag_volatile[0] == 0) {   // 等待生产者将flag[0]置1
            }
            result[0] = data[0];
        }
    }
    ```