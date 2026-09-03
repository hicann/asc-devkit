# asc\_syncthreads

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_sync_functions.h"`。

在SIMT（Single Instruction Multiple Threads）编程范式中，同一个线程块（Block）内的多个线程并行执行，但各线程的执行进度可能不同。当需要线程间协调工作、共享数据或确保某些操作按顺序执行时，必须使用同步机制。

asc\_syncthreads接口用于**阻塞当前线程块内所有线程**，直到所有线程都执行到该同步点位置。

**关键特征：**

- 该接口会阻塞线程执行，直到块内所有线程都到达同步点；
- 确保同步点之前的所有内存操作对块内所有线程可见；
- 常用于线程块内的数据共享、分阶段计算、并行归约等场景；

下图展示了同一个线程块内多线程共享数据场景可能出现的问题：

![](../../../figures/syncthreads_1.png)

asc\_syncthreads接口的执行流程示意图如下：

![](../../../figures/syncthreads_function.png)

需要注意，线程块内所有线程必须都调用到asc\_syncthreads\(\)接口，程序才能继续执行。如果有线程未到达同步点，其他线程将被阻塞等待，导致死锁，例如：若将本接口的调用放在分支中，就可能导致部分线程到达不了同步接口调用处，导致程序卡死。

## 函数原型

```cpp
inline void asc_syncthreads()
```

## 参数说明

无

## 返回值说明

无

## 约束说明

- 线程块内所有线程必须都执行到同步点，否则会导致死锁。

- 避免分支中调用本接口，除非能确保线程块内所有线程都进入该分支。
- 避免在循环次数不一致的情况下调用。

## 调用示例

完整样例请参考[MemoryFence样例](../../../../../../examples/03_simt_api/02_features/01_api_features/01_sync_instruction/memory_fence/README.md)。

- SIMT编程场景：

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __global__ __launch_bounds__(1024) void kernel_sync_threads(float* dst, int count)
    {
         int idx = threadIdx.x;
         if (idx > 0 && idx < count) {
             dst[idx] = 1;
         }

         // 等待block内所有thread都执行到当前代码
         asc_syncthreads();

         if (idx == 0) {
             dst[0] = 0;
             for(int i = 1023; i > 0; i--) {
                 if (i < count) {
                     dst[0] += dst[i];
                 }
             }
         }
    }
    ```

    ```text
    输出结果:
    [1023, 1, 1, 1 …]
    ```

- SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_sync_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel_sync_threads(__gm__ float* dst, int count)
    {
         int idx = threadIdx.x;
         if (idx > 0 && idx < count) {
             dst[idx] = 1;
         }

         // 等待block内所有thread都执行到当前代码
         asc_syncthreads();

         if (idx == 0) {
             dst[0] = 0;
             for(int i = 1023; i > 0; i--) {
                 if (i < count) {
                     dst[0] += dst[i];
                 }
             }
         }
    }
    ```

    ```text
    输出结果:
    [1023, 1, 1, 1 …]
    ```
