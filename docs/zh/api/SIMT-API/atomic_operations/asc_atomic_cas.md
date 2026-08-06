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

## 功能说明

对Unified Buffer或Global Memory上address的数值进行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。

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
| address | 输出 | Unified Buffer或Global Memory的地址。 |
| compare | 输入 | 源操作数，做比较的值。 |
| val | 输入 | 源操作数，用于赋值的值。 |

不同数据类型支持的内存范围说明如下：

**表2**  不同数据类型支持的内存范围

| 参数数据类型 | 支持的内存空间 |
| --- | --- |
| int32_t、uint32_t、float、half2、bfloat16x2_t | Unified Buffer、Global Memory |
| int64_t、uint64_t | Global Memory |

## 返回值说明

Unified Buffer或Global Memory上的初始数据。

## 约束说明

原子操作保证对同一地址的读改写过程具有原子性，但不保证多个线程之间的执行顺序。使用`asc_atomic_cas`实现抢占逻辑时，可保证最多一个线程抢占成功，但不保证固定由哪个线程抢占成功。

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

    SIMD与SIMT混合编程场景，需要显式使用地址空间限定符表示地址空间：\_\_gm\_\_表示Global Memory内存空间，\_\_ubuf\_\_表示Unified Buffer内存空间。

    ```cpp
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
