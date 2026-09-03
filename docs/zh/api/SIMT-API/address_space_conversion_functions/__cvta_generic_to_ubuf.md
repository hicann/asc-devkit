# \_\_cvta\_generic\_to\_ubuf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_generic_to_ubuf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

将输入的指针转换为其指向的Unified Buffer（UB）内存空间的地址值并返回。

## 函数原型

```cpp
size_t __cvta_generic_to_ubuf(const void* ptr)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ptr | 输入 | 源操作数。 |

## 返回值说明

输入指针指向UB内存空间的地址值。
该接口不校验输入地址是否为可安全访问的有效地址。只有当`ptr`实际指向UB内存空间时，返回值是有效的UB地址值。特殊场景说明如下：

| 输入场景 | 返回值 |
| --- | --- |
| `ptr`为Global Memory指针 | 未定义行为，返回值不是有效UB地址。 |
| `ptr`为`nullptr` | 返回`0x00000000fff80000`。 |
| `ptr`为栈空间指针 | 编译阶段报错。 |

## 约束说明

SIMD与SIMT混合编程场景不支持使用该接口。

## 调用示例

SIMT编程场景：

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_ubuf(uint32_t* src, uint32_t total_length)
{
    __ubuf__ uint32_t ubuf_data[1024];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx >= total_length) {
        return;
    }
    ubuf_data[threadIdx.x] = src[idx];
    size_t addr_val = __cvta_generic_to_ubuf(ubuf_data + threadIdx.x);
    printf("UB(ubuf_data)的地址值为：%lu\n", addr_val);
}
```
