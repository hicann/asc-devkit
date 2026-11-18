# __asc\_bswap16

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__asc_bswap16_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/device_functions.h"`。

将输入数据x的2个字节按相反顺序重新排列，返回字节顺序反转后的16位结果。

## 函数原型

```cpp
inline unsigned short __asc_bswap16(unsigned short x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数 |

## 返回值说明

字节反转后的unsigned short类型结果。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __global__ __launch_bounds__(1024) void kernel___asc_bswap16(unsigned short* dst, const unsigned short* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __asc_bswap16(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/device_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___asc_bswap16(
        __gm__ unsigned short* dst, __gm__ const unsigned short* x)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __asc_bswap16(x[idx]);
    }
    ```

输入输出示例如下：

```text
x: 0b00010010 00110100
result: 0b00110100 00010010
```
