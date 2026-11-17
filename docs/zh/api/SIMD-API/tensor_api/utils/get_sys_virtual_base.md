# get_sys_virtual_base

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

读取系统虚拟基地址特殊寄存器的值并按`int64_t`返回，该寄存器为只读特殊寄存器，存储当前核上下文的系统虚拟基地址，由系统在核函数启动前按核配置完成。系统虚拟基地址用于在核函数内将相对偏移量转换为系统虚拟地址，常用于地址推导、跨核地址对齐等需要系统虚拟基地址参与的场景。

## 函数原型

```cpp
__aicore__ inline int64_t get_sys_virtual_base()
```

## 参数说明

无

## 返回值说明

系统虚拟基地址。

## 流水类型

PIPE_S

## 约束说明

- 本接口为只读查询接口，仅读取系统虚拟基地址寄存器当前值，不修改任何寄存器或存储状态，可在AIC与AIV上下文中调用。
- 本接口不支持纯SIMT编译模式。

## 调用示例

以下示例调用`get_sys_virtual_base`接口。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void get_sys_virtual_base_kernel(__gm__ int64_t* output)
{
    using namespace asc::te;
    output[0] = get_sys_virtual_base();
}
```

