# asc\_atomic\_sub

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_sub_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_atomic_functions.h"`（除half、half2、bfloat16_t、bfloat16x2_t类型之外的接口）、`"simt_api/asc_fp16.h"`（half、half2类型接口）、`"simt_api/asc_bf16.h"`（bfloat16_t、bfloat16x2_t类型接口）。

对Unified Buffer（UB）或Global Memory上的数据与指定数据执行原子减操作，即在这些内存区域的数据中减去指定数据。

## 函数原型

```cpp
inline int32_t asc_atomic_sub(int32_t* address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_sub(uint32_t* address, uint32_t val)
```

```cpp
inline float asc_atomic_sub(float* address, float val)
```

```cpp
inline int64_t asc_atomic_sub(int64_t* address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_sub(uint64_t* address, uint64_t val)
```

```cpp
inline half asc_atomic_sub(half* address, half val)
```

```cpp
inline half2 asc_atomic_sub(half2* address, half2 val)
```

```cpp
inline bfloat16_t asc_atomic_sub(bfloat16_t* address, bfloat16_t val)
```

```cpp
inline bfloat16x2_t asc_atomic_sub(bfloat16x2_t* address, bfloat16x2_t val)
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

接口执行前内存地址中的原始数据。

注意，由于底层硬件约束，half和bfloat16_t类型的返回值不准确，禁止直接使用这些类型的返回值。half2和bfloat16x2_t类型不受此限制。

## 约束说明

-   原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于浮点累减或依赖返回值顺序敏感的场景，结果可能随线程调度变化而不同。
-   本接口的性能受以下因素影响，相关原理请参见[原子操作机制](atomic_operations_intro.md#原子操作机制)。
    -   内存空间：UB的访问路径比Global Memory短，通常具有更低的访问开销。当使用的数据类型支持UB（即int32\_t、uint32\_t、float、half2、bfloat16x2\_t）时，建议优先在UB中完成原子操作。
    -   返回值：该接口无对应的性能优化指令，对于所有数据类型，程序中是否使用该接口返回值，接口性能基本一致。
    -   地址分布：Global Memory原子操作经过L2 Cache处理，L2 Cache以512B Cache Line为缓存管理单位，每条Cache Line包含4个128B扇区（Sector），Global Memory原子操作以128B Sector为处理粒度。目标地址集中在同一个Sector内时，处理效率较低；目标地址分布在更多Sector内时，处理效率较高。因此，业务允许时，建议将原子操作的目标地址分散到更多Sector中。

## 调用示例

示例场景为：多个线程处理资源申请量，使用`asc_atomic_sub`接口从共享剩余配额中扣减已消费数量。该用例假设申请总量不超过初始配额，避免无符号下溢。输入输出参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `requests` | 每个元素表示一条资源申请需要扣减的配额。 |
| `remaining` | Global Memory中的剩余配额，kernel启动前初始化。 |
| `n` | 申请条数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void consume_quota(uint32_t *remaining,
                                                         uint32_t *requests,
                                                         uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_sub(remaining, requests[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间，`__ubuf__`表示UB内存空间。

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void consume_quota(__gm__ uint32_t *remaining,
                                                                  __gm__ uint32_t *requests,
                                                                  uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_sub(remaining, requests[idx]);
    }
    ```

输出结果示例如下：

```
remaining before: 100
requests: 4, 8, 3
remaining after: 85 // 表明共享配额被3个线程原子扣减了15
```
