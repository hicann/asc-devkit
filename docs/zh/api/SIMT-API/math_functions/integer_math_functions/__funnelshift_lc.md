# \_\_funnelshift\_lc

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/__funnelshift_lc_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

将输入数据hi:lo拼接为64位源值，其中lo保存低32位、hi保存高32位；向左移动min(shift, 32)位，返回移位后结果的最高有效32位。

## 函数原型

```cpp
inline unsigned int __funnelshift_lc(unsigned int lo, unsigned int hi, unsigned int shift)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| lo | 输入 | 拼接后的低32位 |
| hi | 输入 | 拼接后的高32位 |
| shift | 输入 | 移位数 |

## 返回值说明

返回拼接移位后最高有效的32位。

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __global__ __launch_bounds__(1024) void kernel___funnelshift_lc(
        unsigned int* dst, const unsigned int* lo, const unsigned int* hi, const unsigned int* shift)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __funnelshift_lc(lo[idx], hi[idx], shift[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel___funnelshift_lc(
        __gm__ unsigned int* dst, __gm__ const unsigned int* lo, __gm__ const unsigned int* hi,
        __gm__ const unsigned int* shift)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        dst[idx] = __funnelshift_lc(lo[idx], hi[idx], shift[idx]);
    }
    ```

输入输出示例如下：

```text
lo: 0b00010010 00110100 01010110 01111000
hi: 0b10001001 10101011 11001101 11101111
shift: 8
result: 0b10101011 11001101 11101111 00010010
```
