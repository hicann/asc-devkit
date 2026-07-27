# asc\_atomic\_exch

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

对Unified Buffer或Global Memory地址做原子赋值操作，即将指定数据赋值到Unified Buffer或Global Memory地址中。

## 函数原型

```cpp
inline float asc_atomic_exch(float *address, float val)
```

```cpp
inline int32_t asc_atomic_exch(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_exch(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_exch(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_exch(uint64_t *address, uint64_t val)
```

```cpp
inline half2 asc_atomic_exch(half2 *address, half2 val)
```

```cpp
inline bfloat16x2_t asc_atomic_exch(bfloat16x2_t *address, bfloat16x2_t val)
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
| int32_t、uint32_t、float、half2、bfloat16x2_t | Unified Buffer、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

Unified Buffer或Global Memory上的初始数据。

## 约束说明

原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。当多个线程向同一地址写入不同值时，最终保留的值取决于原子操作的串行化顺序，可能随线程调度变化而不同。

## 需要包含的头文件

使用除half2、bfloat16x2\_t类型之外的接口需要包含"simt\_api/device\_atomic\_functions.h"头文件，使用half2类型接口需要包含"simt\_api/asc\_fp16.h"头文件，使用bfloat16x2\_t类型接口需要包含"simt\_api/asc\_bf16.h"头文件。

```cpp
#include "simt_api/device_atomic_functions.h"
```

```cpp
#include "simt_api/asc_fp16.h"
```

```cpp
#include "simt_api/asc_bf16.h"
```

## 调用示例

示例场景为：多个线程扫描故障标志，检测到故障的线程使用`asc_atomic_exch`接口将共享状态置为故障状态，并记录替换前的旧状态。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `fault_flags` | 每个元素表示一条检测结果，0为无故障，非0为故障。 |
| `status` | Global Memory中的共享状态，0表示正常，1表示故障。 |
| `old_status` | 保存每个故障线程执行交换前读到的旧状态。 |
| `n` | 输入元素个数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void publish_fault_status(uint32_t *status,
                                                               uint32_t *old_status,
                                                               uint32_t *fault_flags,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (fault_flags[idx] != 0U) {
            old_status[idx] = asc_atomic_exch(status, 1U);
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void publish_fault_status(__gm__ uint32_t *status,
                                                                        __gm__ uint32_t *old_status,
                                                                        __gm__ uint32_t *fault_flags,
                                                                        uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (fault_flags[idx] != 0U) {
            old_status[idx] = asc_atomic_exch(status, 1U);
        }
    }
    ```

输出结果示例如下：

```
fault_flags: 0, 1, 0, 1
status before: 0
status after: 1 // 表明至少有一个线程检测到故障并发布故障状态
```
