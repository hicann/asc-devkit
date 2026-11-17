# atomic_cas

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

对Global Memory中`address`指向的单个元素执行原子比较交换操作。读取该元素的旧值`old_value`，当`old_value`等于`compare`时，将`desired`写入该地址；否则保持原值不变。接口返回`old_value`。整个读取、比较和条件写回过程为原子操作。

计算公式如下：

$$
new\_value = (old\_value == compare) ? desired : old\_value
$$

## 函数原型

```cpp
template <typename T>
__aicore__ inline T atomic_cas(__gm__ T* address, T compare, T desired)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入/输出 | Global Memory的地址。 |
| compare | 输入 | 源操作数，做比较的值。 |
| desired | 输入 | 源操作数，用于赋值的值。 |

## 数据类型

`T`支持：`int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t`。

## 返回值说明

`address`地址中计算前的原始数据。

## 流水类型

PIPE_S

## 约束说明

- 本接口运行在标量流水（`PIPE_S`）上。若本接口与`PIPE_MTE2`或`PIPE_MTE3`上的数据搬运指令访问同一Global Memory地址，且执行顺序影响结果，编译器无法自动完成跨流水同步，调用方需按实际依赖插入[sync_pipe](../sync/sync_pipe.md)。
- 本接口访问Global Memory时绕过DCache，不维护缓存一致性。若其他核或其他通路通过缓存访问同一Global Memory地址，调用方需使用[datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md)清理并失效对应Cache Line，并使用[sync_data_barrier](../sync/data_barrier.md)保证相关访存操作的执行顺序和数据可见性。

## 调用示例

以下示例比较Global Memory中初始值为10的数据，相等时将其替换为20，操作后结果为20，操作前旧值为10。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void atomic_cas_kernel(__gm__ uint32_t* data)
{
    using namespace asc::te;

    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    const uint32_t old_value = atomic_cas<uint32_t>(data, 10U, 20U);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    data[1] = old_value;
    datacache_clean_and_invalid_line(data);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
}
```

