# datacache_clean_and_invalid_line

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

清理并失效Global Memory或Unified Buffer地址所在的Data Cache缓存行。

## 函数原型

```cpp
template <typename T>
__aicore__ inline void datacache_clean_and_invalid_line(T address)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | Global Memory或Unified Buffer地址，该地址所在的缓存行将被清理并失效。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 在调用本接口之前，必须先调用[sync_data_barrier](../sync/data_barrier.md)，并将参数设置为`mem_dsb_t::DSB_ALL`，等待此前的内存访问指令执行结束。
- 本接口根据传入指针所在的地址空间选择清理Global Memory或Unified Buffer地址所在的缓存行。

## 调用示例

以下示例将数据写入Global Memory后，清理并失效该地址所在的Data Cache缓存行。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void clean_gm_line_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    data[0] = 10;
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(reinterpret_cast<__gm__ uint64_t*>(data));
}
```

