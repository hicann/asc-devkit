# \_\_half2uint\_rd

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__half2uint_rd_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/asc_fp16.h"`。

遵循CAST\_FLOOR模式，将half类型数据转换为unsigned int类型数据，返回转换后的值。

## 函数原型

```cpp
inline unsigned int __half2uint_rd(const half x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

输入遵循CAST\_FLOOR模式转换成的无符号整数。特殊值如下：

| x值 | 返回值 |
| --- | --- |
| ±0 | 0 |
| nan | 0 |
| inf | 4294967295（uint32_t最大值） |
| -inf | 0 |
| ASCRT\_MAX\_NORMAL\_FP16 | 65504 |
| -ASCRT\_MAX\_NORMAL\_FP16 | 0 |
| ASCRT\_MIN\_DENORM\_FP16 | 0 |
| 0.5 | 0 |
| 1.5 | 1 |

## 约束说明

无

## 调用示例

-   SIMT编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __global__ __launch_bounds__(1024) void kernel__half2uint_rd(uint32_t* dst, half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2uint_rd(x[idx]);
    }
    ```

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_fp16.h"

    __simt_vf__ __launch_bounds__(1024) inline void kernel__half2uint_rd(__gm__ uint32_t* dst, __gm__ half* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __half2uint_rd(x[idx]);
    }
    ```
