# asc\_atomic\_min

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

对Unified Buffer或Global Memory数据做原子求最小值操作，即将Unified Buffer或Global Memory的数据与指定数据中的最小值赋值到Unified Buffer或Global Memory地址中。

## 函数原型

```cpp
inline int32_t asc_atomic_min(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_min(uint32_t *address, uint32_t val)
```

```cpp
inline float asc_atomic_min(float *address, float val)
```

```cpp
inline int64_t asc_atomic_min(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_min(uint64_t *address, uint64_t val)
```

```cpp
inline half asc_atomic_min(half *address, half val)
```

```cpp
inline bfloat16_t asc_atomic_min(bfloat16_t *address, bfloat16_t val)
```

```cpp
inline half2 asc_atomic_min(half2 *address, half2 val)
```

```cpp
inline bfloat16x2_t asc_atomic_min(bfloat16x2_t *address, bfloat16x2_t val)
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
| int32_t、uint32_t、float、half、bfloat16_t、half2、bfloat16x2_t | Unified Buffer、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

Unified Buffer或Global Memory上的初始数据。

注意，由于底层硬件约束，half和bfloat16\_t类型的返回值不准确，避免直接使用这些类型的返回值。

## 约束说明

原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于依赖接口返回值判断线程先后顺序的场景，结果可能随线程调度变化而不同。

## 需要包含的头文件

使用除half、half2、bfloat16\_t、bfloat16x2\_t类型之外的接口需要包含"simt\_api/device\_atomic\_functions.h"头文件，使用half和half2类型接口需要包含"simt\_api/asc\_fp16.h"头文件，使用bfloat16\_t和bfloat16x2\_t类型接口需要包含"simt\_api/asc\_bf16.h"头文件。

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

示例场景为：多个线程扫描延迟数组，使用`asc_atomic_min`接口将全局最低延迟写入同一个结果地址。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `latency` | 每个元素表示一次请求的延迟值。 |
| `min_latency` | Global Memory中的最小值结果，kernel启动前初始化为足够大的值。 |
| `n` | 延迟样本数量。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void find_min_latency(uint32_t *min_latency,
                                                           uint32_t *latency,
                                                           uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_min(min_latency, latency[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void find_min_latency(__gm__ uint32_t *min_latency,
                                                                    __gm__ uint32_t *latency,
                                                                    uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_min(min_latency, latency[idx]);
    }
    ```

输出结果示例如下：

```
latency: 31, 20, 45
min_latency: 20 // 表明所有线程并发更新后得到最小值
```
