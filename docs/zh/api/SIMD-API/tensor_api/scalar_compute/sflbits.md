# sflbits

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

对一个64位有符号整数，取其最高位（即符号位）作为参考位，统计从次高位（即最高数值位）开始与符号位相同的连续比特位的个数。当输入为全0或全1时，接口返回-1。

## 函数原型

```cpp
__aicore__ inline int64_t sflbits(int64_t value)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| value | 输入 | 待统计的64位有符号整数。 |

## 返回值说明

返回从最高数值位开始和符号位相同的连续比特位的个数，数据类型为`int64_t`。当输入是-1（比特位全1）或者0（比特位全0）时，返回-1；其余输入返回值的范围是[0, 62]。

## 流水类型

PIPE_S

## 约束说明

- 接口仅支持`int64_t`，传入其他位宽的值会按`int64_t`隐式转换后参与位运算，结果以`int64_t`返回。

## 调用示例

以下示例调用`sflbits`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void sflbits_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    constexpr int64_t value = 7;
    output[0] = sflbits(value);
    // output[0]为60。
}
```


