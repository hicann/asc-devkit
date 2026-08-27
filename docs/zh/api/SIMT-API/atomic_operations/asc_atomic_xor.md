# asc\_atomic\_xor

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_xor_res.md#id1 -->

## 功能说明

对Unified Buffer（UB）或Global Memory上address的数值与指定数值val进行原子异或（^）操作，即将address数值异或（^）val的结果赋值到UB或Global Memory上。

## 函数原型

```cpp
inline int32_t asc_atomic_xor(int32_t *address, int32_t val)
```

```cpp
inline uint32_t asc_atomic_xor(uint32_t *address, uint32_t val)
```

```cpp
inline int64_t asc_atomic_xor(int64_t *address, int64_t val)
```

```cpp
inline uint64_t asc_atomic_xor(uint64_t *address, uint64_t val)
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

## 需要包含的头文件

使用该接口需要包含`simt_api/device_atomic_functions.h`头文件。

```cpp
#include "simt_api/device_atomic_functions.h"
```

## 实测验证

实测环境：Ascend 950PR，CANN 9.0.0，`bisheng --enable-simt --npu-arch=dav-3510`。

| 覆盖项 | 初始值 | 操作 | 期望结果 | 实测结果 |
| --- | --- | --- | --- | --- |
| `uint32_t` Global Memory | `0` | 128个线程各执行`asc_atomic_xor(address, 1U)` | `0` | 通过 |

本用例执行偶数次异或，最终值回到0，用于验证原子异或在多线程竞争写同一地址时结果一致。

## 调用示例

示例场景为：多个线程检查事件是否命中，命中时使用`asc_atomic_xor`接口翻转共享奇偶标志。最终值为1表示命中次数为奇数，0表示命中次数为偶数。输入参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `hit` | 每个元素表示一个线程是否命中事件，1为命中，0为未命中。 |
| `parity` | Global Memory中的奇偶标志，kernel启动前清零。 |
| `n` | 输入元素个数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(256) void compute_hit_parity(uint32_t *parity,
                                                             uint32_t *hit,
                                                             uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (hit[idx] != 0U) {
            asc_atomic_xor(parity, 1U);
        }
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间，`__ubuf__`表示UB内存空间。

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void compute_hit_parity(__gm__ uint32_t *parity,
                                                                      __gm__ uint32_t *hit,
                                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        if (hit[idx] != 0U) {
            asc_atomic_xor(parity, 1U);
        }
    }
    ```

输出结果示例如下：

```
hit: 1, 0, 1, 1
parity: 1 // 共有3次命中，奇数次翻转后结果为1
```
