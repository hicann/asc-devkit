# SIMT接口精度标准汇总

本节按照数据类型和接口类型对SIMT浮点计算接口进行精度标准的分类汇总。对于非软仿接口，给出对应硬件指令的精度标准；对于软仿接口，由于软仿程度较低且使用到的硬件指令较少，给出其所用硬件指令的综合性精度标准。

## bf16数据类型计算API精度标准

### 数学库函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [hexp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | - | 1-ulp，Not Support denormalized numbers |
| [htanh](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | - | 1-ulp，Not Support denormalized numbers |
| [hrsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2tanh](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2sqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2rsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | - | 1-ulp，Not Support denormalized numbers |

### 算术函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [\_\_habs](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__habs.md) | - | 0-ulp, Support denormalized numbers |
| [\_\_hadd](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hadd.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmul](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hmul.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdiv](../SIMT-API/math_functions/bfloat16_type/bfloat16_arithmetic_functions/__hdiv.md) | 1-ulp，Not Support denormalized numbers | - |
| [\_\_haddx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__haddx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmulx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hmulx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdivx2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_arithmetic_functions/__hdivx2.md) | 1-ulp，Not Support denormalized numbers | - |

## fp16数据类型计算API精度标准

### 数学库函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [hexp](../SIMT-API/math_functions/half_type/half_math_functions/hexp.md) | 1-ulp | - |
| [hexp2](../SIMT-API/math_functions/half_type/half_math_functions/hexp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hexp10](../SIMT-API/math_functions/half_type/half_math_functions/hexp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog](../SIMT-API/math_functions/half_type/half_math_functions/hlog.md) | 1-ulp，Not Support denormalized numbers | - |
| [hlog2](../SIMT-API/math_functions/half_type/half_math_functions/hlog2.md) | - | 1-ulp，Not Support denormalized numbers |
| [hlog10](../SIMT-API/math_functions/half_type/half_math_functions/hlog10.md) | - | 1-ulp，Not Support denormalized numbers |
| [hsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hsqrt.md) | 1-ulp，Not Support denormalized numbers | - |
| [hrsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hrsqrt.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2tanh](../SIMT-API/math_functions/half_type/half2_math_functions/h2tanh.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp2](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2exp10](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log](../SIMT-API/math_functions/half_type/half2_math_functions/h2log.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log2](../SIMT-API/math_functions/half_type/half2_math_functions/h2log2.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2log10](../SIMT-API/math_functions/half_type/half2_math_functions/h2log10.md) | - | 1-ulp，Not Support denormalized numbers |
| [h2sqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2sqrt.md) | 1-ulp，Not Support denormalized numbers | - |
| [h2rsqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2rsqrt.md) | - | 1-ulp，Not Support denormalized numbers |

### 算术函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [\_\_hadd](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hadd.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmul](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hmul.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdiv](../SIMT-API/math_functions/half_type/half_arithmetic_functions/__hdiv.md) | 1-ulp，Not Support denormalized numbers | - |
| [\_\_haddx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__haddx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hmulx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hmulx2.md) | 0-ulp, Support denormalized numbers | - |
| [\_\_hdivx2](../SIMT-API/math_functions/half_type/half2_arithmetic_functions/__hdivx2.md) | 1-ulp，Not Support denormalized numbers | - |

## fp32数据类型计算API精度标准

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [expf](../SIMT-API/math_functions/float_math_functions/expf.md) | 1-ulp，Not Support denormalized numbers | - |
| [tanhf](../SIMT-API/math_functions/float_math_functions/tanhf.md) | - | 1-ulp，Not Support denormalized numbers |
| [tanpif](../SIMT-API/math_functions/float_math_functions/tanpif.md) | - | 1-ulp，Not Support denormalized numbers |
| [atanf](../SIMT-API/math_functions/float_math_functions/atanf.md) | - | 1-ulp，Not Support denormalized numbers |
| [atan2f](../SIMT-API/math_functions/float_math_functions/atan2f.md) | - | 1-ulp，Not Support denormalized numbers |
| [atanhf](../SIMT-API/math_functions/float_math_functions/atanhf.md) | - | 1-ulp，Not Support denormalized numbers |
| [expm1f](../SIMT-API/math_functions/float_math_functions/expm1f.md) | - | 1-ulp，Not Support denormalized numbers |
| [logf](../SIMT-API/math_functions/float_math_functions/logf.md) | - | 1-ulp，Not Support denormalized numbers |
| [log2f](../SIMT-API/math_functions/float_math_functions/log2f.md) | - | 1-ulp，Not Support denormalized numbers |
| [log10f](../SIMT-API/math_functions/float_math_functions/log10f.md) | - | 1-ulp，Not Support denormalized numbers |
| [log1pf](../SIMT-API/math_functions/float_math_functions/log1pf.md) | - | 1-ulp，Not Support denormalized numbers |
| [cosf](../SIMT-API/math_functions/float_math_functions/cosf.md) | - | 1-ulp，Not Support denormalized numbers |
| [acoshf](../SIMT-API/math_functions/float_math_functions/acoshf.md) | - | 1-ulp，Not Support denormalized numbers |
| [sqrtf](../SIMT-API/math_functions/float_math_functions/sqrtf.md) | 1-ulp，Not Support denormalized numbers | - |
| [rsqrtf](../SIMT-API/math_functions/float_math_functions/rsqrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [cbrtf](../SIMT-API/math_functions/float_math_functions/cbrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [rcbrtf](../SIMT-API/math_functions/float_math_functions/rcbrtf.md) | - | 1-ulp，Not Support denormalized numbers |
| [remquof](../SIMT-API/math_functions/float_math_functions/remquof.md) | - | 1-ulp，Not Support denormalized numbers |
| [fdividef](../SIMT-API/math_functions/float_math_functions/fdividef.md) | 1-ulp，Not Support denormalized numbers | - |
