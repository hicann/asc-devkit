# clear_nthbit

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

对一个64位无符号整数执行位清除操作，将`bits`中第`bit_index`位清零，其余位保持不变。返回修改后的64位无符号整数。

## 函数原型

```cpp
__aicore__ inline uint64_t clear_nthbit(uint64_t bits, uint32_t bit_index)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| bits | 输入 | 待处理的64位无符号整数。 |
| bit_index | 输入 | 位索引。实际位索引按`(bit_index % 64 + 64) % 64`计算，范围为[0, 63]。 |

## 返回值说明

返回位清零后的结果，数据类型为`uint64_t`。

## 流水类型

PIPE_S

## 约束说明

- 当`bit_index`大于63时，实际清除的位索引按`bit_index % 64`计算。

## 调用示例

以下示例调用`clear_nthbit`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void clear_nthbit_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr uint64_t value = 7U;
    output[0] = clear_nthbit(value, 1);
    // output[0]为5。
}
```


