# datacache_preload

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

从Global Memory的指定地址预加载一个Cache Line（64字节）的数据到DCache中，为后续Scalar单元对该地址的读取操作提前填充缓存，降低首次访问延迟。

预取地址由`address`与`offset`相加得到，单位为字节。本接口需配合后续通过Scalar单元读取Global Memory的指令使用，预取才能带来命中收益。

## 函数原型

```cpp
__aicore__ inline void datacache_preload(__gm__ uint64_t* address, int64_t offset)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| address | 输入 | 源操作数在Global Memory中的地址，单位为字节。 |
| offset | 输入 | 相对`address`的有符号字节偏移，预取起始地址 = `address` + `offset`，单位为字节。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

频繁调用此接口可能导致保留站（Reservation Station，用于暂存待执行指令的硬件队列）发生拥塞。在此情况下，该指令将被视为NOP指令，进而阻塞Scalar流水线。因此，建议连续调用此接口的次数不要超过4次。

## 调用示例

以下示例预取Global Memory输入数据所在的Cache Line，再通过标量读取该数据。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void datacache_preload_kernel(__gm__ int32_t* input, __gm__ int32_t* output)
{
    using namespace asc::te;
    init_soc_state();
    datacache_preload(reinterpret_cast<__gm__ uint64_t*>(input), 0);
    output[0] = load_dev<int32_t>(input);
    sync_data_barrier(mem_dsb_t::DSB_ALL);
    datacache_clean_and_invalid_line(reinterpret_cast<__gm__ uint64_t*>(output));
}

```
