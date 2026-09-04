# \_\_isUbuf

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/address_space_predicate_functions/__isUbuf_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

判断输入的指针是否指向Unified Buffer（UB）内存空间的地址。

## 函数原型

```cpp
unsigned int __isUbuf(const void* ptr)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| ptr | 输入 | 源操作数。 |

## 返回值说明

如果输入的指针指向UB内存空间的地址，则返回1，否则返回0。<br>
该接口根据输入指针的地址空间信息进行分类判断，不校验该指针是否为可安全访问的有效地址。`__isUbuf()`返回1仅表示该指针被分类为UB地址，不代表该地址一定可以安全访问。特殊场景说明如下：<br>

| 输入场景 | 返回值 |
| --- | --- |
| `ptr`为有效Global Memory指针 | 0 |
| `ptr`为有效栈空间指针 | 0 |
| `ptr`为`nullptr` | 0 |
| `ptr`为伪造的低位地址，如`(void*)0x1` | 0 |
| `ptr`为伪造的全1地址 | 0 |
| `ptr`由`__cvta_ubuf_to_generic(0)`或`__cvta_ubuf_to_generic(1)`返回 | 1 |
| `ptr`由`__cvta_ubuf_to_generic(全1)`返回 | 0 |
| `ptr`由`__cvta_local_to_generic(全1)`返回 | 1 |

因此，不能仅依据`__isUbuf(ptr) == 1`判断`ptr`是否为可安全访问的UB地址。

## 约束说明

SIMD与SIMT混合编程场景不支持使用该接口。

## 调用示例

SIMT编程场景：

```cpp
#include "simt_api/device_functions.h"

__global__ __launch_bounds__(1024) void kernel__isUbuf(uint32_t* dst,
uint32_t* src)
{
    __ubuf__ uint32_t ptr[10];
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if(__isUbuf(ptr) == 1) {
       dst[idx] = 1;
    } else {
      dst[idx] = 0;
    }
}
```
