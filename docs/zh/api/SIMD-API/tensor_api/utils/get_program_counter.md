# get_program_counter

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

获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。常用于调试时定位当前执行位置，也可配合[icache_preload](../cache_control/icache_preload.md)等接口，将当前指令地址作为预取的起始地址。

## 函数原型

```cpp
__aicore__ inline int64_t get_program_counter()
```

## 参数说明

无

## 返回值说明

返回int64_t类型的程序计数器指针。

## 流水类型

PIPE_S

## 约束说明

- 本接口返回64位程序计数器值，其中有效地址位为0~47位，高16位为无效位。使用时需通过掩码`0xFFFFFFFFFFFF`（低48位全1）将高位无效位清零，以得到当前指令在GM中的有效48位地址。

## 调用示例

以下示例调用`get_program_counter`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_program_counter_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_program_counter();
}
```

