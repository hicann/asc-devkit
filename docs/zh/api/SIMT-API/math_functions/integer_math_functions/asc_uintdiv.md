# asc_uintdiv

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_uintdiv_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

整数快除算法分为两步计算商`quotient`。第一步根据固定除数预计算`magic`和`shift`，可由配套预计算接口[asc_get_uintdiv_magic_and_shift](asc_get_uintdiv_magic_and_shift.md)完成；第二步调用本接口，根据被除数`dividend`、`magic`和`shift`计算商。计算公式如下：

$$
quotient = \left(\left\lfloor\frac{dividend \times magic}{2^N}\right\rfloor + dividend\right) \gg shift
$$

根据被除数数据类型的不同，公式中$N$的取值不同：当被除数为`uint32_t`数据类型时，$N$为32；当被除数为`uint64_t`数据类型时，$N$为64。

整数快除算法可以用于性能优化，适用于一个除数被用于多次除法计算的场景。

> [!NOTE]说明
>
> 整数快除算法使用方式：
> 1. 在标量计算逻辑中，调用[asc_get_uintdiv_magic_and_shift](asc_get_uintdiv_magic_and_shift.md)接口，计算除数的`magic`和`shift`值。
> 2. 在SIMT VF计算逻辑中，调用`asc_uintdiv`接口，完成指定被除数的除法运算。

## 函数原型

- 被除数为`uint32_t`类型

    ```cpp
    inline uint32_t asc_uintdiv(uint32_t dividend, uint32_t magic, uint32_t shift)
    ```

- 被除数为`uint64_t`类型

    ```cpp
    inline uint64_t asc_uintdiv(uint64_t dividend, uint64_t magic, uint64_t shift)
    ```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dividend | 输入 | 被除数。 |
| magic | 输入 | 根据指定除数预计算得到的快速除法中的乘法因子。 |
| shift | 输入 | 根据指定除数预计算得到的快速除法中的移位量。 |

## 返回值说明

返回`dividend`除以`magic`和`shift`所对应的除数得到的商。

## 约束说明

- 当被除数为`uint32_t`数据类型时，支持的被除数最大值为`INT32_MAX`，即2^31-1；当被除数为`uint64_t`数据类型时，支持的被除数最大值为`INT64_MAX`，即2^63-1。
- `magic`和`shift`必须对应同一非零除数；`uint32_t`数据和`uint64_t`数据对应除数的最大值分别为`INT32_MAX`和`INT64_MAX`。
- SIMT编程场景当前不支持使用该接口。

## 调用示例

完整样例请参考[SIMD与SIMT混合编程实现快速除法算子样例](../../../../../../examples/05_simd_simt_hybrid/02_best_practices/simd_simt_integer_fast_div/README.md)。

-   SIMD与SIMT混合编程场景：

    ```cpp
    #include "simt_api/asc_simt.h"
    #include "simt_api/math_functions.h"

    __simt_vf__ __launch_bounds__(1024) inline void simt_fast_div(
        __gm__ uint32_t* output, uint32_t value, uint32_t magic, uint32_t shift, uint32_t total_length)
    {
        uint32_t idx = threadIdx.x;
        if (idx >= total_length) {
            return;
        }
        output[idx] = asc_uintdiv(value, magic, shift);
    }

    __global__ __vector__ void integer_div_kernel(
        __gm__ uint32_t* output, uint32_t value, uint32_t divisor, uint32_t total_length)
    {
        // 在标量计算逻辑中，根据除数预计算magic和shift。
        uint32_t magic = 0;
        uint32_t shift = 0;
        asc_get_uintdiv_magic_and_shift(&magic, &shift, divisor);
        asc_vf_call<simt_fast_div>(dim3(1024), output, value, magic, shift, total_length);
    }
    ```
