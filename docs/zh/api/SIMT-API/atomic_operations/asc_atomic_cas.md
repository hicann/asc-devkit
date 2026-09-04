# asc\_atomic\_cas

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/atomic_operations/asc_atomic_cas_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_atomic_functions.h"`（除half2、bfloat16x2\_t类型之外的接口）、`"simt_api/asc_fp16.h"`（half2类型接口）、`"simt_api/asc_bf16.h"`（bfloat16x2\_t类型接口）。

对Unified Buffer（UB）或Global Memory上address的数值进行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。

## 函数原型

```cpp
inline float asc_atomic_cas(float *address, float compare, float val)
```

```cpp
inline int32_t asc_atomic_cas(int32_t *address, int32_t compare, int32_t val)
```

```cpp
inline uint32_t asc_atomic_cas(uint32_t *address, uint32_t compare, uint32_t val)
```

```cpp
inline int64_t asc_atomic_cas(int64_t *address, int64_t compare, int64_t val)
```

```cpp
inline uint64_t asc_atomic_cas(uint64_t *address, uint64_t compare, uint64_t val)
```

```cpp
inline half2 asc_atomic_cas(half2 *address, half2 compare, half2 val)
```

```cpp
inline bfloat16x2_t asc_atomic_cas(bfloat16x2_t *address, bfloat16x2_t compare, bfloat16x2_t val)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输出 | UB或Global Memory的地址。 |
| compare | 输入 | 源操作数，做比较的值。 |
| val | 输入 | 源操作数，用于赋值的值。 |

不同数据类型支持的内存范围说明如下：

**表2**  不同数据类型支持的内存范围

| 参数数据类型 | 支持的内存空间 |
| --- | --- |
| int32_t、uint32_t、float、half2、bfloat16x2_t | UB、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

UB或Global Memory上的初始数据。

## 约束说明

-   原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。使用`asc_atomic_cas`实现抢占逻辑时，可保证最多一个线程抢占成功，但不保证固定由哪个线程抢占成功。
-   本接口的性能受以下因素影响，相关原理请参见[原子操作机制](atomic_operations_intro.md#原子操作机制)。
    -   内存空间：UB的访问路径比Global Memory短，通常具有更低的访问开销。当使用的数据类型支持UB（即int32\_t、uint32\_t、float、half2、bfloat16x2\_t）时，建议优先在UB中完成原子操作。
    -   返回值：该接口无对应的性能优化指令，对于所有数据类型，程序中是否使用该接口返回值，接口性能基本一致。
    -   地址分布：Global Memory原子操作经过L2 Cache处理，L2 Cache以512B Cache Line为缓存管理单位，每条Cache Line包含4个128B扇区（Sector），Global Memory原子操作以128B Sector为处理粒度。目标地址集中在同一个Sector内时，处理效率较低；目标地址分布在更多Sector内时，处理效率较高。因此，业务允许时，建议将原子操作的目标地址分散到更多Sector中。

## 调用示例

完整样例请参考[InsertHashTable算子样例](../../../../../examples/03_simt_api/02_features/01_api_features/00_memory_access/insert_hash_table/README.md)。

简单示例场景为：多个线程尝试抢占同一个任务，任务初始拥有者ID为0。使用`asc_atomic_cas`接口实现只有一个线程抢占成功，其它线程读到非0后抢占失败。输入输出参数说明如下：

| 名称 | 说明 |
| --- | --- |
| `worker_ids` | 每个元素表示一个工作者ID。 |
| `owner` | Global Memory中的任务拥有者，0表示无人占用。 |
| `claim_result` | 保存每个线程是否抢占成功。 |
| `n` | 参与抢占的线程数。 |

核心代码实现如下：

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __global__ __launch_bounds__(256) void claim_task(uint32_t *owner,
                                                      uint32_t *claim_result,
                                                      uint32_t *worker_ids,
                                                      uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_owner = asc_atomic_cas(owner, 0U, worker_ids[idx]);
        claim_result[idx] = (old_owner == 0U) ? 1U : 0U;
    }
    ```

-   SIMD与SIMT混合编程场景：

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：`__gm__`表示Global Memory内存空间，`__ubuf__`表示UB内存空间。

    ```cpp
    #include "simt_api/device_atomic_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void claim_task(__gm__ uint32_t *owner,
                                                               __gm__ uint32_t *claim_result,
                                                               __gm__ uint32_t *worker_ids,
                                                               uint32_t n)
    {
        uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
        if (idx >= n) {
            return;
        }

        uint32_t old_owner = asc_atomic_cas(owner, 0U, worker_ids[idx]);
        claim_result[idx] = (old_owner == 0U) ? 1U : 0U;
    }
    ```

输出结果示例如下：

```
worker_ids: 101, 102, 103
owner before: 0
owner after: 101/102/103中的一个 // 表明只有一个线程抢占成功
claim_result: 仅一个元素为1
```
