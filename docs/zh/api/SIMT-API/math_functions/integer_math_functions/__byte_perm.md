# \_\_byte\_perm

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__byte_perm_res.md#id1 -->

## 功能说明

由输入的两个4字节的uint32\_t类型数据组成一个8个字节的64比特位的整数，通过选择器s指定选取其中的4个字节，将这4个字节从低位到高位拼成一个uint32\_t类型的整数。具体实现逻辑如下：

```cpp
// 以下为C++表示的BytePerm(x, y, s)计算逻辑
uint64_t tmp64 = ((uint64_t)y << 32) | x; // x,y拼接成uint64整数

uint8_t selector0 = (s >> 0) & 0x7; // selector0取值范围在[0, 7]之间
uint8_t selector1 = (s >> 4) & 0x7;
uint8_t selector2 = (s >> 8) & 0x7;
uint8_t selector3 = (s >> 12) & 0x7;

uint8_t byte0 = (tmp64 >> (selector0 * 8)) & 0xFF; // 选取tmp64中的第selector0个字节
uint8_t byte1 = (tmp64 >> (selector1 * 8)) & 0xFF;
uint8_t byte2 = (tmp64 >> (selector2 * 8)) & 0xFF;
uint8_t byte3 = (tmp64 >> (selector3 * 8)) & 0xFF;

// result为BytePerm返回值，其结果由对应字节按照顺序拼接而成
uint32_t result = byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24);
```

## 函数原型

```cpp
unsigned int __byte_perm(unsigned int x, unsigned int y, unsigned int s)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数，uint32_t类型，与y拼接成64比特位的整数，该整数的[0:31]位为x。 |
| y | 输入 | 源操作数，uint32_t类型，与x拼接成64比特位的整数，该整数的[32:63]位为y。 |
| s | 输入 | 选择器，uint32_t类型，用于指定如何从x和y组成的8个字节64比特位的整数中提取4字节数据。其中，s[0:3]、s[4:7]、s[8:11]、s[12:15]分别对应4个要选取的字节，其值表示所选字节在8字节整数中的索引，索引范围为0到7。 |

## 返回值说明

通过选择器s选取出的uint32\_t类型的整数。

-   当x为0，y为0，s为0时，返回值为0。
-   当x为1，y为1，s为1时，返回值为16843008。

## 约束说明

无

## 需要包含的头文件

使用该接口需要包含`simt_api/device_functions.h`头文件。

```cpp
#include "simt_api/device_functions.h"
```

## 调用示例

-   SIMT编程场景：

    ```cpp
    __global__ __launch_bounds__(1024) void KernelByte_perm(unsigned int* dst, unsigned int* x, unsigned int* y, unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void KernelByte_perm(__gm__ unsigned int* dst, __gm__ unsigned int* x, __gm__ unsigned int* y, __gm__ unsigned int* s)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __byte_perm(x[idx], y[idx], s[idx]);
    }
    ```
