# asc\_atomic\_add

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add_res.md#id1 -->

## 功能说明

对Unified Buffer（UB）或Global Memory中的数据与指定数据执行原子加操作，即将指定数据累加到这些内存区域的数据中。

## 函数原型

```cpp
inline int32_t asc_atomic_add(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_add(uint32_t *address, uint32_t val)
```

```cpp
inline float asc_atomic_add(float *address, float val)
```

```cpp
inline int64_t asc_atomic_add(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_add(uint64_t *address, uint64_t val)
```

```cpp
inline half asc_atomic_add(half *address, half val)
```

```cpp
inline bfloat16_t asc_atomic_add(bfloat16_t *address, bfloat16_t val)
```

```cpp
inline half2 asc_atomic_add(half2 *address, half2 val)
```

```cpp
inline bfloat16x2_t asc_atomic_add(bfloat16x2_t *address, bfloat16x2_t val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输出 | UB或Global Memory的地址。 |
| val | 输入 | 源操作数。 |

不同数据类型支持的内存范围说明如下：

**表2**  不同数据类型支持的内存范围

| 参数数据类型 | 支持的内存空间 |
| --- | --- |
| int32_t、uint32_t、float、half、bfloat16_t、half2、bfloat16x2_t | UB、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

UB或Global Memory上的初始数据。

注意，由于底层硬件约束，half和bfloat16\_t类型的返回值不准确，禁止直接使用这些类型的返回值。half2和bfloat16x2\_t类型不受此限制。

## 约束说明

-   原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于浮点累加顺序敏感场景，结果可能随线程调度变化而不同。
-   本接口的性能受以下因素影响，相关原理请参见[原子操作机制](atomic_operations_intro.md#原子操作机制)。具体性能对比和优化示例请参见[asc_atomic_add接口性能对比样例](../../../../../examples/03_simt_api/02_features/01_api_features/02_atomic_operation/atomic_add_perf)。
    -   内存空间：UB的访问路径比Global Memory短，通常具有更低的访问开销。当使用的数据类型支持UB（即int32\_t、uint32\_t、float、half、bfloat16\_t、half2、bfloat16x2\_t）时，建议优先在UB中完成原子操作。
    -   返回值：是否使用返回值可能影响编译器生成的原子加指令。各数据类型在不使用返回值时是否能生成更优指令的情况如下：

        | 数据类型 | 不使用返回值时是否生成更优指令 |
        | --- | --- |
        | int32\_t、uint32\_t、float、half、bfloat16\_t、half2、bfloat16x2\_t | 是 |
        | int64\_t、uint64\_t | 否 |

        业务场景允许时，建议不使用返回值。
    -   地址分布：Global Memory原子操作经过L2 Cache处理，L2 Cache以512B Cache Line为缓存管理单位，每条Cache Line包含4个128B扇区（Sector），Global Memory原子操作以128B Sector为处理粒度。目标地址集中在同一个Sector内时，处理效率较低；目标地址分布在更多Sector内时，处理效率较高。因此，业务允许时，建议将原子操作的目标地址分散到更多Sector中。

## 需要包含的头文件

使用除half、half2、bfloat16\_t、bfloat16x2\_t类型之外的接口需要包含`simt_api/device_atomic_functions.h`头文件，使用half和half2类型接口需要包含`simt_api/asc_fp16.h`头文件，使用bfloat16\_t和bfloat16x2\_t类型接口需要包含`simt_api/asc_bf16.h`头文件。

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

可参阅[字节频率直方图样例](../../../../../examples/03_simt_api/03_best_practices/03_instruction_optimizations/atomic_histogram)，该样例详细展示了如何利用`asc_atomic_add()`接口，高效统计输入字节序列中每个字节值的出现频率。

简单示例场景：多个线程扫描状态数组，状态非0表示一条异常记录，使用`asc_atomic_add()`接口统计异常状态的数量。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `status` | 每个元素表示一条状态记录，0为正常，非0为异常。 |
| `error_count` | Global Memory中的异常计数器，kernel启动前清零。 |
| `n` | 输入元素个数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void count_error_status(uint32_t *error_count,
                                                         uint32_t *status,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (status[idx] != 0U) {
            asc_atomic_add(error_count, 1U);
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间，`__ubuf__`表示UB内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void count_error_status(__gm__ uint32_t *error_count,
                                                         __gm__ uint32_t *status,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (status[idx] != 0U) {
            asc_atomic_add(error_count, 1U);
        }
    }
    ```

输出结果示例如下：

```
status: 0, 2, 0, 1, 3
error_count: 3 // 表明status中有3个数据是非0的
```
