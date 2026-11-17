# store_dev

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

不经过DCache向GM地址上写数据。当多核操作GM地址时，如果数据无法对齐到Cache Line，经过DCache的方式下，由于按照Cache Line大小进行读写，会导致多核数据随机覆盖的问题。此时，可以采用不经过DCache直接读写GM地址的方式。

## 函数原型

```cpp
template <typename T>
__aicore__ inline void store_dev(__gm__ T* addr, T value)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | GM地址指向的数据类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| addr | 输出 | 目标GM地址。 |
| value | 输入 | 待写入目标的数据。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 仅支持`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`int64_t`和`uint64_t`整型数据，不支持浮点类型。
- `addr`起始地址须按写入的`T`字节数对齐，且须落在GM可访问地址空间内。
- 本接口访问GM时绕过DCache，不维护缓存一致性。若其他核或其他通路通过缓存访问同一GM地址，调用方需使用[datacache_clean_and_invalid_line](../cache_control/datacache_clean_and_invalid_line.md)清理并失效对应Cache Line，并使用[sync_data_barrier](../sync/data_barrier.md)保证相关访存操作的执行顺序和数据可见性。

## 调用示例

以下示例调用`store_dev`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void store_dev_kernel(__gm__ int32_t* output)
{
    using namespace asc::te;
    store_dev<int32_t>(output, 7);
}
```
