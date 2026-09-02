# \_\_cvta\_local\_to\_generic

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_local_to_generic_res.md#id1 -->

## 功能说明

将栈空间的地址值转换为对应的指针并返回。

## 函数原型

```cpp
void* __cvta_local_to_generic(size_t rawbits)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| rawbits | 输入 | 源操作数。 |

## 返回值说明

指向输入栈空间的指针。该接口不校验输入是否为有效栈空间地址。异常场景示例说明如下：

| 输入场景 | 返回值 |
| --- | --- |
| `rawbits`为`0` | 返回指针使用`__isLocal`判断为1，但该指针不一定是可安全访问的栈空间地址。 |
| `rawbits`为全1 | 返回指针使用`__isUbuf`判断为1，说明非法rawbits的分类结果不可依赖。 |

## 约束说明

SIMD与SIMT混合编程场景不支持使用该接口。

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

## 调用示例

SIMT编程场景：

```cpp
__global__ __launch_bounds__(1024) void kernel__cvta_local_to_generic(uint32_t* dst, uint64_t addr_val, uint32_t total_len)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_len) {
        return;
    }
    uint32_t* ptr = static_cast<uint32_t*>(__cvta_local_to_generic(addr_val));
    dst[idx] = ptr[0];
}
```
