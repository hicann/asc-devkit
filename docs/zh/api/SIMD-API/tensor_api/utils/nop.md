# nop

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

生成一条空操作指令，不执行实际计算。每调用一次阻塞一拍流水。

## 函数原型

```cpp
__aicore__ inline void nop()
```

## 参数说明

无

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 如果需要精确阻塞n拍流水，需要在n个`nop`接口调用前后插入同步。

## 调用示例

以下示例调用`nop`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void nop_kernel()
{
    using namespace asc::te;
    nop();
}
```

