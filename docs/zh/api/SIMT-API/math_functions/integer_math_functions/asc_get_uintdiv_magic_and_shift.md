# asc_get_uintdiv_magic_and_shift

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMT-API/math_functions/integer_math_functions/asc_get_uintdiv_magic_and_shift_res.md#id1 -->

## 功能说明

头文件路径为：`"simt_api/math_functions.h"`。

整数快除用于同除数场景的快速计算，算法分为两步计算商，本接口用于第一步计算，获取乘法因子`magic`和移位量`shift`，生成的参数供SIMT VF侧快除接口[asc_uintdiv](asc_uintdiv.md)使用。本接口的计算过程如下：

$$
\begin{aligned}
shift &= \left\lceil \log_2 divisor \right\rceil \\
magic &= \left\lfloor 2^N \times \frac{2^{shift} - divisor}{divisor} \right\rfloor + 1
\end{aligned}
$$

根据除数数据类型的不同，公式中$N$的取值不同：当除数为`uint32_t`数据类型时，$N$为32；当除数为`uint64_t`数据类型时，$N$为64。

整数快除算法可以用于性能优化，适用于一个除数被用于多次除法计算的场景。

> [!NOTE]说明
>
> 整数快除算法使用方式：
> 1. 在标量计算逻辑中，调用`asc_get_uintdiv_magic_and_shift`接口，计算除数的`magic`和`shift`值。
> 2. 在SIMT VF计算逻辑中，调用[asc_uintdiv](asc_uintdiv.md)接口，完成指定被除数的除法运算。

> [!CAUTION]注意
>
> 本接口不能在SIMT VF中调用，只能在标量计算逻辑中调用。

## 函数原型

- 除数为`uint32_t`类型

    ```cpp
    inline void asc_get_uintdiv_magic_and_shift(uint32_t* magic, uint32_t* shift, uint32_t divisor)
    ```

- 除数为`uint64_t`类型

    ```cpp
    inline void asc_get_uintdiv_magic_and_shift(uint64_t* magic, uint64_t* shift, uint64_t divisor)
    ```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| magic | 输出 | 根据指定除数计算得到的快速除法中的乘法因子。 |
| shift | 输出 | 根据指定除数计算得到的快速除法中的移位量。 |
| divisor | 输入 | 除数。 |

## 返回值说明

无

## 约束说明

- 当除数为`uint32_t`数据类型时，支持的除数最大值为`INT32_MAX`，即2^31-1；当除数为`uint64_t`数据类型时，支持的除数最大值为`INT64_MAX`，即2^63-1。
- 除数不支持为0，否则计算结果未定义。
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
