# \_\_cvta\_ubuf\_to\_generic

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_ubuf_to_generic_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

将Unified Buffer（UB）内存空间的地址值转换为对应的指针并返回。

## 函数原型

```cpp
void* __cvta_ubuf_to_generic(size_t rawbits)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| rawbits | 输入 | 源操作数。 |

## 返回值说明

指向UB内存空间的指针。该接口不校验输入是否为有效UB地址。异常场景示例说明如下：

| 输入场景 | 返回值 |
| --- | --- |
| rawbits为0或1 | 返回指针使用__isUbuf()判断为1，但该指针不一定是可安全访问的UB地址。 |
| rawbits为全1 | 返回指针使用__isGlobal()、__isUbuf()、__isLocal()均判断为0。 |

## 约束说明

SIMD与SIMT混合编程场景不支持使用该接口。

## 调用示例

SIMT编程场景：

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_ubuf_to_generic(uint32_t* dst, uint64_t addr_val)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t* ptr = static_cast<uint32_t*>(__cvta_ubuf_to_generic(addr_val));
    dst[idx] = ptr[0];
}
```
