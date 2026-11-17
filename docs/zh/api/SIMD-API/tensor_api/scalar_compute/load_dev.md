# load_dev

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

不经过DCache直接从GM地址读取整型数据。当多核操作GM地址且数据无法对齐到Cache Line时，经过DCache读写可能引入Cache Line粒度的数据覆盖。此时，可使用本接口绕过DCache读取GM数据。

## 函数原型

```cpp
template <typename T>
__aicore__ inline T load_dev(__gm__ T* addr)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | GM地址指向的数据类型。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| addr | 输入 | 源GM地址。 |

## 返回值说明

返回从GM读取的数据，数据类型为模板参数T，与`addr`指向的数据类型一致。

## 流水类型

PIPE_S

## 约束说明

- 仅支持`int8_t`、`uint8_t`、`int16_t`、`uint16_t`、`int32_t`、`uint32_t`、`int64_t`和`uint64_t`整型数据，不支持浮点类型。
- `addr`起始地址须按读取的`T`字节数对齐，且须落在GM可访问地址空间内。

## 调用示例

以下示例调用`load_dev`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void load_dev_kernel(__gm__ int32_t* input, __gm__ int32_t* output)
{
    using namespace asc::te;
    const int32_t value = load_dev<int32_t>(input);
    output[0] = value;
}
```
