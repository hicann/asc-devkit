# popc

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

获取输入`uint64_t`数据的二进制表示中值为1的位数。

## 函数原型

```cpp
__aicore__ inline int64_t popc(uint64_t value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| value | 输入 | 待统计的64位无符号整数。 |

## 返回值说明

`int64_t`类型，表示`value`的二进制表示中值为1的位数。取值范围为[0, 64]。

## 流水类型

PIPE_S

## 约束说明

- 本接口运行在标量流水（`PIPE_S`）上，同一流水内的数据依赖由指令执行顺序保证，无需额外同步。

## 调用示例

以下示例调用`popc`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void popc_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = popc(value);
    // output[0]为3。
}
```


