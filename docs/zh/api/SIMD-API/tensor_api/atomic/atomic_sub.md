# atomic_sub

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

对Global Memory中`address`指向的单个元素执行`atomic_sub`原子操作。接口读取该地址中的旧值`old_value`，按以下公式计算新值`new_value`并写回该地址，返回`old_value`。整个读取、计算和写回过程为原子操作。

计算公式如下：

$$
new\_value = old\_value - val
$$

## 函数原型

```cpp
template <typename T>
__aicore__ inline T atomic_sub(__gm__ T* address, T val)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入/输出 | Global Memory的地址。 |
| val | 输入 | 源操作数。 |

## 数据类型

`T`支持：`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`。

## 返回值说明

`address`地址中计算前的原始数据。

## 流水类型

PIPE_S

## 约束说明

- 本接口运行在标量流水（`PIPE_S`）上。若本接口与`PIPE_MTE2`或`PIPE_MTE3`上的数据搬运指令访问同一Global Memory地址，且执行顺序影响结果，编译器无法自动完成跨流水同步，调用方需按实际依赖插入[sync_pipe](../sync/sync_pipe.md)。
- 本接口访问Global Memory时绕过DCache，不维护缓存一致性。若其他核或其他通路通过缓存访问同一Global Memory地址，调用方需使用[datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md)清理并失效对应Cache Line，并使用[sync_data_barrier](../sync/data_barrier.md)保证相关访存操作的执行顺序和数据可见性。详情可参考[Scalar原子操作与DCache一致性](../../../../guide/programming_guide/advanced_programming/memory_model/cache_coherence.md#scalar原子操作与dcache一致性)。

## 调用示例

以下示例对Global Memory中初始值为10的数据执行`atomic_sub`操作，操作后结果为7，操作前旧值为10。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void atomic_sub_kernel(__gm__ uint32_t* data)
{
    using namespace asc::te;

    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = atomic_sub<uint32_t>(data, 3U);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    data[1] = old_value;
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
}
```

