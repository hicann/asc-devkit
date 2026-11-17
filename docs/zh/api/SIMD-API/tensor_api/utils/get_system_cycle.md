# get_system_cycle

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

读取当前系统cycle计数器，返回int64_t类型的当前计数值。该计数器为只读系统计数器，宽度64bit，复位值为0，随系统时钟持续递增，反映硬件运行以来的累计cycle数，常用于性能统计、耗时测量与执行序的时序判断。

## 函数原型

```cpp
__aicore__ inline int64_t get_system_cycle()
```

## 参数说明

无

## 返回值说明

返回系统Cycle数。

## 流水类型

PIPE_S

## 约束说明

- 本接口是PIPE_S流水，若需要测试其他流水的指令时间，需要在调用该接口前插入对应流水的同步。
- 本接口为只读查询接口，读取的是只读系统计数器，不修改任何寄存器或存储状态，可在AIC与AIV上下文中调用。

## 调用示例

以下示例调用`get_system_cycle`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_system_cycle_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_system_cycle();
}
```

