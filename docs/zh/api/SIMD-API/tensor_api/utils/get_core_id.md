# get_core_id

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

获取当前AI Core的编号。一个AI Core中包含1个AIC和若干个AIV，本接口获取的是一个AI Core相对其余AI Core的索引。该接口常用于按核编号对任务进行划分，使不同的AI Core处理不同的数据分片。

## 函数原型

```cpp
__aicore__ inline int64_t get_core_id()
```

## 参数说明

无

## 返回值说明

返回当前AI Core的编号。

## 流水类型

PIPE_S

## 约束说明

- 无。

## 调用示例

以下示例调用`get_core_id`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_core_id_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_core_id();
}
```

