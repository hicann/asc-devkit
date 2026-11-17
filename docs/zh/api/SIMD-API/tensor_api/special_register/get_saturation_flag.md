# get_saturation_flag

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

按饱和模式类型读取当前是否开启饱和，并返回布尔类型的结果。

## 函数原型

```cpp
__aicore__ inline bool get_saturation_flag(saturation_mode mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mode | 输入 | 饱和模式类型，类型为`saturation_mode`。 |

## 返回值说明

`true`表示当前该模式下饱和已开启；`false`表示未开启（对`int_mode`为截断；对`float_mode`/`float8_mode`/`cast_mode`为非饱和或截断）。

## 流水类型

PIPE_S

## 约束说明

- 返回值是API语义层结果：`true`表示该模式下饱和已开启，`false`表示未开启（对`int_mode`为截断；对`float_mode`/`float8_mode`/`cast_mode`为非饱和或截断）。各模式语义差异见[set_saturation_flag](set_saturation_flag.md)表1。
- 对`cast_mode`，即使本接口返回`true`，也需同时满足策略为`override_strategy::use_global`，CAST饱和配置才会在精度转换中真正生效。

## 调用示例

以下示例调用`get_saturation_flag`接口。

```cpp
#include "tensor_api/tensor.h"

__global__ __vector__ void get_saturation_flag_kernel(__gm__ bool* output)
{
    using namespace asc::te;
    output[0] = get_saturation_flag(saturation_mode::float_mode);
}
```

