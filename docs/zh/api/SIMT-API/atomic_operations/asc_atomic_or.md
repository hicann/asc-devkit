# asc\_atomic\_or

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_or_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_atomic_functions.h"`。

对Unified Buffer（UB）或Global Memory上address的数值与指定数值val进行原子或（|）操作，即将address数值或（|）val的结果赋值到UB或Global Memory上。

## 函数原型

```cpp
inline int32_t asc_atomic_or(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_or(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_or(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_or(uint64_t *address, uint64_t val)
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
| int32_t、uint32_t | UB、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

UB或Global Memory上的初始数据。

## 约束说明

-   原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。对于依赖接口返回值判断线程先后顺序的场景，结果可能随线程调度变化而不同。
-   本接口的性能受以下因素影响，相关原理请参见[原子操作机制](atomic_operations_intro.md#原子操作机制)。
    -   内存空间：UB的访问路径比Global Memory短，通常具有更低的访问开销。当使用的数据类型支持UB（即int32\_t、uint32\_t）时，建议优先在UB中完成原子操作。
    -   返回值：该接口无对应的性能优化指令，对于所有数据类型，程序中是否使用该接口返回值，接口性能基本一致。
    -   地址分布：Global Memory原子操作经过L2 Cache处理，L2 Cache以512B Cache Line为缓存管理单位，每条Cache Line包含4个128B扇区（Sector），Global Memory原子操作以128B Sector为处理粒度。目标地址集中在同一个Sector内时，处理效率较低；目标地址分布在更多Sector内时，处理效率较高。因此，业务允许时，建议将原子操作的目标地址分散到更多Sector中。

## 调用示例

示例场景为：多个线程分别检测到不同特征，使用`asc_atomic_or`接口将这些特征合并到同一个共享bitmask中。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `observed_flags` | 每个元素表示一个线程观察到的特征位。 |
| `flags` | Global Memory中的汇总bitmask，kernel启动前清零。 |
| `n` | 特征来源数量。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void merge_observed_flags(uint32_t *flags,
                                                               uint32_t *observed_flags,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_or(flags, observed_flags[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间，`__ubuf__`表示UB内存空间。

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void merge_observed_flags(__gm__ uint32_t *flags,
                                                                        __gm__ uint32_t *observed_flags,
                                                                        uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        asc_atomic_or(flags, observed_flags[idx]);
    }
    ```

输出结果示例如下：

```
observed_flags: 0x1, 0x4, 0x2
flags: 0x7 // 表明3个线程观察到的特征位被合并
```
