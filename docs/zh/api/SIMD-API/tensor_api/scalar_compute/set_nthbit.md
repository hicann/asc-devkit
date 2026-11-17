# set_nthbit

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

将`uint64_t`数据的指定二进制位置为1，其余位保持不变。返回修改后的`uint64_t`整数。

## 函数原型

```cpp
__aicore__ inline uint64_t set_nthbit(uint64_t bits, uint32_t bit_index)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| bits | 输入 | 待置位的64位无符号整数。执行此计算后输入数据不变。 |
| bit_index | 输入 | 位索引，指定需要设置为1的二进制位的位置。实际位索引按`bit_index % 64`计算，索引范围为[0, 63]。 |

## 返回值说明

返回执行计算得到的结果，数据类型为`uint64_t`。

## 流水类型

PIPE_S

## 约束说明

- 当参数`bit_index`大于63时，位索引的计算逻辑为`bit_index % 64`。

## 调用示例

以下示例调用`set_nthbit`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void set_nthbit_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = set_nthbit(value, 3);
    // output[0]为15。
}
```


