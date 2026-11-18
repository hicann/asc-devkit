# __asc\_bswap32

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
- Atlas推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__asc_bswap32_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

将输入数据x的4个字节按相反顺序重新排列，返回字节顺序反转后的32位结果。

## 函数原型

```cpp
inline unsigned int __asc_bswap32(unsigned int x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数 |

## 返回值说明

字节反转后的unsigned int类型结果。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel___asc_bswap32(unsigned int* dst, const unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __asc_bswap32(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___asc_bswap32(
        __gm__ unsigned int* dst, __gm__ const unsigned int* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __asc_bswap32(x[idx]);
    }
    ```

输入输出示例如下：

```text
x: 0b00010010 00110100 01010110 01111000
result: 0b01111000 01010110 00110100 00010010
```
