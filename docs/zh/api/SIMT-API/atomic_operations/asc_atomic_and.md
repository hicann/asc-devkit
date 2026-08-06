# asc\_atomic\_and

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

对Unified Buffer或Global Memory上address的数值与指定数值val进行原子与（&）操作，即将address数值与（&）val的结果赋值到Unified Buffer或Global Memory上。

## 函数原型

```cpp
inline int32_t asc_atomic_and(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_and(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_and(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_and(uint64_t *address, uint64_t val)
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
| int32_t、uint32_t | Unified Buffer、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

Unified Buffer或Global Memory上的初始数据。

## 约束说明

原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于依赖接口返回值判断线程先后顺序的场景，结果可能随线程调度变化而不同。

## 需要包含的头文件

使用该接口需要包含"simt\_api/device\_atomic\_functions.h"头文件。

```cpp
#include "simt_api/device_atomic_functions.h"
```

## 调用示例

示例场景为：多个线程根据各自检测结果清除共享状态字中的某些bit，使用`asc_atomic_and`接口保证不同线程清除不同bit时不会覆盖彼此的修改。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `flags` | Global Memory中的共享状态位。 |
| `clear_bits` | 每个元素表示需要清除的bit，kernel内部会转换为AND掩码。 |
| `n` | 掩码数量。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void clear_status_bits(uint32_t *flags,
                                                            uint32_t *clear_bits,
                                                            uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t mask = ~clear_bits[idx];
        asc_atomic_and(flags, mask);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void clear_status_bits(__gm__ uint32_t *flags,
                                                                     __gm__ uint32_t *clear_bits,
                                                                     uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t mask = ~clear_bits[idx];
        asc_atomic_and(flags, mask);
    }
    ```

输出结果示例如下：

```
flags before: 0xF
clear_bits: 0x2, 0x4
flags after: 0x9 // 表明bit1和bit2被并发清除
```
