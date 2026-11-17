# get_squeeze_status

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

读取squeeze操作后保存至AR特殊寄存器的有效数据长度值，用于配合C API的`asc_squeeze_and_storeunalign`接口获取已连续搬出的有效数据量。

## 函数原型

```cpp
__aicore__ inline int64_t get_squeeze_status()
```

## 参数说明

无

## 返回值说明

返回int64_t类型的squeeze有效数据长度值。

## 流水类型

PIPE_S

## 约束说明

- 调用本接口前，需要先调用C API的`asc_squeeze_and_storeunalign_init`清空AR特殊寄存器，并调用C API的`asc_squeeze_and_storeunalign`完成数据筛选。

## 调用示例

以下示例调用`get_squeeze_status`接口。

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_squeeze_status_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_squeeze_status();
}
```

