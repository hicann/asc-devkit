# \_\_cvta\_generic\_to\_local

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_conversion_functions/__cvta_generic_to_local_res.md#id1 -->

## 功能说明

将输入的指针转换为其指向的栈空间的地址值并返回。

## 函数原型

```cpp
size_t __cvta_generic_to_local(const void* ptr)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ptr | 输入 | 源操作数。 |

## 返回值说明

输入指针指向栈空间的地址值。
该接口不校验输入地址是否为可安全访问的有效地址。只有当`ptr`实际指向栈空间时，返回值才是有效的栈空间地址值，特殊场景说明如下：
| 输入场景 | 返回值 |
| --- | --- |
| `ptr`为Global Memory指针 | 未定义行为，返回值不是有效栈空间地址。 |
| `ptr`为`nullptr` | 返回`0x00000000fff00000`。 |
| `ptr`为UB指针 | 编译阶段报错。 |

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
__global__ __launch_bounds__(1024) void kernel__cvta_generic_to_local(uint32_t* src)
{
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    uint32_t local_data[2] = {src[idx], src[idx]};
    size_t addr_val = __cvta_generic_to_local(local_data);
    if (addr_val > 1152) {  // 1152:SIMT栈空间大小
        printf("栈(local_data)溢出，偏移量为：%lu\n", addr_val);
    } else {
        printf("栈(local_data)的偏移量为：%lu\n", addr_val);
    }
}
```
