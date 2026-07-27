# asc\_atomic\_inc

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

## 功能说明

对Unified Buffer或Global Memory上address的数值进行原子加1操作，如果address上的数值大于等于指定数值val，则对address赋值为0，否则将address上数值加1。

## 函数原型

```cpp
inline uint32_t asc_atomic_inc(uint32_t *address, uint32_t val)
```

```cpp
inline uint64_t asc_atomic_inc(uint64_t *address, uint64_t val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输出 | Unified Buffer或Global Memory的地址。 |
| val | 输入 | 源操作数。 |

不同数据类型支持的内存范围说明如下：

**表2**  不同数据类型支持的内存范围

| 参数数据类型 | 支持的内存空间 |
| --- | --- |
| uint32_t | Unified Buffer、Global Memory |
| uint64_t | Global Memory |

## 返回值说明

Unified Buffer或Global Memory上的初始数据。

## 约束说明

原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于依赖返回值分配序号或槽位的场景，返回值对应的序号唯一，但分配给具体线程的顺序可能随线程调度变化而不同。

## 需要包含的头文件

使用该接口需要包含"simt\_api/device\_atomic\_functions.h"头文件。

```cpp
#include "simt_api/device_atomic_functions.h"
```

## 调用示例

示例场景为：多个线程向固定容量的环形缓冲区写入任务，使用`asc_atomic_inc`接口获取递增并自动回绕的槽位编号。返回值是更新前的旧计数，可作为本线程获得的槽位。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `ticket` | Global Memory中的环形计数器，kernel启动前初始化。 |
| `slots` | 保存每个线程获得的槽位编号。 |
| `capacity` | 环形队列容量。 |
| `n` | 需要分配槽位的线程数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void allocate_ring_slot(uint32_t *ticket,
                                                             uint32_t *slots,
                                                             uint32_t capacity,
                                                             uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_inc(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void allocate_ring_slot(__gm__ uint32_t *ticket,
                                                                      __gm__ uint32_t *slots,
                                                                      uint32_t capacity,
                                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_ticket = asc_atomic_inc(ticket, capacity - 1U);
        slots[idx] = old_ticket;
    }
    ```

输出结果示例如下：

```
ticket before: 0
capacity: 4
n: 6
slots: 0, 1, 2, 3, 0, 1 // 顺序由实际原子执行顺序决定
ticket after: 2
```
