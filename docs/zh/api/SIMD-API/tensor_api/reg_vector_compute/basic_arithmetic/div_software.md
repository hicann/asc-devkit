# div

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->

## 功能说明

头文件路径：`"tensor_api/experimental/arch/vector/basic_arithmetic.h"`。

该接口根据src0携带的mask，对两个源操作数按元素相除，并通过编译期配置选择除法精度算法。计算公式如下：

$$
dst_i = \frac{src0_i}{src1_i}
$$

`operator/`直接使用硬件除法指令，不提供精度模式和算法配置；`div`支持通过模板参数配置除法精度和算法，并根据所选配置处理Subnormal。

## 函数原型

```cpp
enum class div_algo {
    INTRINSIC,
    PRECISION_0ULP_FTZ_TRUE,
    DIFF_COMPENSATION,
    PRECISION_1ULP_FTZ_TRUE,
    PRECISION_0ULP_FTZ_FALSE,
    PRECISION_1ULP_FTZ_FALSE
};

template <bool precisionMode = false, div_algo algo = div_algo::INTRINSIC, typename T>
__simd_callee__ inline reg_tensor<T> div(
    const reg_tensor<T>& src0, const reg_tensor<T>& src1)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| --- | --- |
| precisionMode | 是否启用高精度除法路径。为`true`时使用差值补偿算法，仅支持`float`。精度说明请参考[关键特性说明](#关键特性说明)。 |
| algo | 除法算法，支持上述枚举值。算法与Subnormal模式的关系请参考[关键特性说明](#关键特性说明)。 |
| T | 操作数数据类型。支持的数据类型请参考[数据类型](#数据类型)。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| src0 | 输入 | 被除数，类型为`reg_tensor<T>`。src0.reg保存矢量数据，src0.mask控制有效元素。 |
| src1 | 输入 | 除数，类型为`reg_tensor<T>`。src1.mask不参与本次计算。 |

## 数据类型

| algo | precisionMode=false时支持的数据类型 |
| --- | --- |
| `INTRINSIC`、`PRECISION_1ULP_FTZ_TRUE` | int16_t、uint16_t、half、int32_t、uint32_t、float |
| `PRECISION_1ULP_FTZ_FALSE` | half、float |
| `DIFF_COMPENSATION`、`PRECISION_0ULP_FTZ_TRUE`、`PRECISION_0ULP_FTZ_FALSE` | float |

`precisionMode=true`时，各算法均仅支持float。非法数据类型或算法组合会产生编译错误。

## 返回值说明

返回按元素相除的结果，类型为`reg_tensor<T>`。返回值mask与src0.mask相同；src0.mask对应位置为0时，返回值的对应元素置零。

## 约束说明

- `src0.mask`用于指定参与计算的有效元素。调用接口前需通过`with_mask`预先设置；mask为0的元素不参与计算，输出对应元素置零。
- `INTRINSIC`、`DIFF_COMPENSATION`是否保留Subnormal由`--cce-ftz`决定；高精度模式不表示自动关闭FTZ，详见[配置Subnormal模式](#配置subnormal模式)。
- 使用整数类型时，除数不能为0，结果为按元素进行整数除法得到的商。
- 使用浮点类型时，除数为`0.0`或`-0.0`且被除数为非零有限值时，结果为带相应符号的`inf`；被除数和除数均为零时，结果为`nan`。
- 使用浮点类型时，被除数或除数为`nan`时，结果为`nan`；有限值除以`inf`或`-inf`时，结果为带相应符号的零；`inf`或`-inf`除以有限非零值时，结果为带相应符号的`inf`；`inf`或`-inf`除以`inf`或`-inf`时，结果为`nan`。
- 浮点运算结果溢出时返回带相应符号的`inf`；结果下溢为Subnormal时，按照所选算法及`--cce-ftz`配置处理。

## 关键特性说明

### 最大精度误差

对于浮点类型，支持的配置及最大精度误差如下。整数类型不涉及浮点精度误差和Subnormal处理。

| precisionMode | algo | 支持的浮点类型 | 最大精度误差 |
| --- | --- | --- | --- |
| `false`（默认值） | `div_algo::INTRINSIC`（默认值） | half、float | 1 ULP |
| `false` | `div_algo::PRECISION_1ULP_FTZ_TRUE`、`div_algo::PRECISION_1ULP_FTZ_FALSE` | half、float | 1 ULP |
| `false` | `div_algo::DIFF_COMPENSATION`、`div_algo::PRECISION_0ULP_FTZ_TRUE`、`div_algo::PRECISION_0ULP_FTZ_FALSE` | float | 0 ULP |
| `true` | 所有支持的算法 | float | 0 ULP |

`precisionMode=true`时通过差值补偿提高除法精度，但不表示自动保留Subnormal。Subnormal的处理方式由`algo`与编译选项共同决定。

### 配置Subnormal模式

FTZ（Flush-To-Zero）是一种浮点运算模式，当结果为[Subnormal](../../../basic_api/data_structures/builtin_data_types.md#p7381131713310)时，将其直接清零（近似为0），而非保留其精确的微小数值。上述精度误差应结合所选Subnormal模式理解；0 ULP不表示FTZ模式下仍保留Subnormal结果。

#### 默认算法

`div_algo::INTRINSIC`为默认算法，浮点类型的Subnormal处理受编译选项`--cce-ftz`控制：

- `--cce-ftz=true`（默认值）：采用FTZ模式。
- `--cce-ftz=false`：支持Subnormal数据计算并保留Subnormal结果。

上述规则同时适用于默认精度和高精度配置。两者的差别在于是否启用差值补偿，而不是是否启用FTZ。

#### 显式指定的算法

下表列出不同`precisionMode`、`algo`与`--cce-ftz`编译选项组合下的浮点配置。表中枚举值省略`div_algo::`前缀，ULP表示对应Subnormal模式下的最大精度误差。

| precisionMode | algo | 支持的浮点类型 | `--cce-ftz=true`（默认值）：Subnormal处理及最大精度误差 | `--cce-ftz=false`：Subnormal处理及最大精度误差 |
| --- | --- | --- | --- | --- |
| `false` | `INTRINSIC`（默认值） | half、float | FTZ，1 ULP | 保留Subnormal，1 ULP |
| `false` | `DIFF_COMPENSATION` | float | FTZ，0 ULP | 保留Subnormal，0 ULP |
| `false` | `PRECISION_0ULP_FTZ_TRUE` | float | FTZ，0 ULP | FTZ，0 ULP |
| `false` | `PRECISION_0ULP_FTZ_FALSE` | float | 保留Subnormal，0 ULP | 保留Subnormal，0 ULP |
| `false` | `PRECISION_1ULP_FTZ_TRUE` | half、float | FTZ，1 ULP | FTZ，1 ULP |
| `false` | `PRECISION_1ULP_FTZ_FALSE` | half、float | 保留Subnormal，1 ULP | 保留Subnormal，1 ULP |
| `true` | `INTRINSIC`（默认值） | float | FTZ，0 ULP | 保留Subnormal，0 ULP |
| `true` | `DIFF_COMPENSATION` | float | FTZ，0 ULP | 保留Subnormal，0 ULP |
| `true` | `PRECISION_0ULP_FTZ_TRUE` | float | FTZ，0 ULP | FTZ，0 ULP |
| `true` | `PRECISION_0ULP_FTZ_FALSE` | float | 保留Subnormal，0 ULP | 保留Subnormal，0 ULP |
| `true` | `PRECISION_1ULP_FTZ_TRUE` | float | FTZ，0 ULP | 保留Subnormal，0 ULP |
| `true` | `PRECISION_1ULP_FTZ_FALSE` | float | FTZ，0 ULP | 保留Subnormal，0 ULP |

当`precisionMode=true`时，始终使用0 ULP算法。`PRECISION_0ULP_FTZ_TRUE`始终采用FTZ路径，`PRECISION_0ULP_FTZ_FALSE`始终保留Subnormal；其余算法的Subnormal处理跟随`--cce-ftz`。

`precisionMode`不是FTZ开关，设置为`false`也不表示只能达到1 ULP精度。需要显式控制FTZ行为时，建议设置`precisionMode=false`，通过`algo`同时选择精度和FTZ策略，避免高精度模式覆盖显式1 ULP算法的FTZ语义。显式0 ULP算法的精度和FTZ策略不受`precisionMode`影响。

#### 使用建议

- 一般场景可使用div(src0, src1)，采用默认的1 ULP精度算法，Subnormal处理跟随--cce-ftz编译选项。
- 需要0 ULP精度且Subnormal处理跟随编译选项时，使用`div<true>(src0, src1)`。
- 需要0 ULP精度且始终采用FTZ模式时，使用`div<true, div_algo::PRECISION_0ULP_FTZ_TRUE>(src0, src1)`。
- 需要保留Subnormal结果且不依赖编译选项时，使用`div<false, div_algo::PRECISION_1ULP_FTZ_FALSE>(src0, src1)`；若同时要求0 ULP精度，使用`div<false, div_algo::PRECISION_0ULP_FTZ_FALSE>(src0, src1)`。

## 调用示例

```cpp
#include "tensor_api/tensor.h"
#include "tensor_api/experimental/arch/vector/basic_arithmetic.h"

using asc::te::experimental::div;
using asc::te::experimental::div_algo;

auto default_result = div(src0, src1);
auto precise_result = div<true>(src0, src1);
auto ftz_result = div<true, div_algo::PRECISION_0ULP_FTZ_TRUE>(src0, src1);
auto compensation_result = div<false, div_algo::DIFF_COMPENSATION>(src0, src1);
auto explicit_ftz_result = div<false, div_algo::PRECISION_1ULP_FTZ_TRUE>(src0, src1);
auto subnormal_result = div<false, div_algo::PRECISION_1ULP_FTZ_FALSE>(src0, src1);
auto precise_subnormal_result = div<false, div_algo::PRECISION_0ULP_FTZ_FALSE>(src0, src1);
```
