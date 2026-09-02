# \_\_uint2half\_rn\_sat

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/half_type/half_precision_conversion_functions/__uint2half_rn_sat_res.md#id1 -->

## 功能说明

将uint32类型数据转换为half类型数据，并遵循CAST\_RINT模式，返回转换后的值。

## 函数原型

```cpp
inline half __uint2half_rn_sat(const unsigned int x)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| x | 输入 | 源操作数。 |

## 返回值说明

遵循CAST\_RINT模式，将输入uint32数据转换成的half数据。特殊值如下：

| x值 | 返回值 |
| --- | --- |
| 0 | 0 |
| x>ASCRT\_MAX\_NORMAL\_FP16 | ASCRT\_MAX\_NORMAL\_FP16 |

## 约束说明

SIMT编程场景由于无法设置CTRL寄存器，本接口的饱和模式不生效。

## 需要包含的头文件

使用该接口需要包含`simt_api/asc_fp16.h`头文件。

```cpp
#include "simt_api/asc_fp16.h"
```

## 调用示例

- SIMD与SIMT混合编程场景：

    ```cpp
    __simt_vf__ __launch_bounds__(1024) inline void kernel__uint2half_rn_sat(__gm__ half* dst, __gm__ uint32_t* x, uint32_t total_length)
    {
        int idx = threadIdx.x + blockIdx.x * blockDim.x;
        if (idx >= total_length) {
            return;
        }
        dst[idx] = __uint2half_rn_sat(x[idx]);
    }
    ```
