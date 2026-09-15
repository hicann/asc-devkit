# SIMT接口精度标准汇总

本节按照数据类型和接口类型对SIMT浮点计算接口进行精度标准的分类汇总。对于非软仿接口，给出对应硬件指令的精度标准；对于软仿接口，由于软仿程度较低且使用到的硬件指令较少，给出其所用硬件指令的综合性精度标准。以下接口的软仿精度标准均在编译选项[--cce-use-fast-math](../../guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#常用的编译选项)为`false`时获得。

## bf16数据类型计算API精度标准

### 数学库函数

| 接口 | 硬件指令精度标准 | 软仿精度标准 |
| --- | --- | --- |
| [hexp](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp.md) | - | 0-ulp, Support denormalized numbers |
| [htanh](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/htanh.md) | - | 0-ulp, Support denormalized numbers|
| [hexp2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp2.md) | - | 0-ulp, Support denormalized numbers |
| [hexp10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hexp10.md) | - | 0-ulp, Support denormalized numbers |
| [hlog](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog.md) | - | 0-ulp, Support denormalized numbers |
| [hlog2](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog2.md) | - | 0-ulp, Support denormalized numbers |
| [hlog10](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hlog10.md) | - | 0-ulp, Support denormalized numbers |
| [hsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hsqrt.md) | - | 0-ulp, Support denormalized numbers |
| [hrsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16_math_functions/hrsqrt.md) | - | 0-ulp, Support denormalized numbers |
| [h2tanh](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2tanh.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp2.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2exp10.md) | - | 0-ulp, Support denormalized numbers |
| [h2log](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log.md) | - | 0-ulp, Support denormalized numbers |
| [h2log2](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log2.md) | - | 0-ulp, Support denormalized numbers |
| [h2log10](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2log10.md) | - | 0-ulp, Support denormalized numbers |
| [h2sqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2sqrt.md) | - | 0-ulp, Support denormalized numbers |
| [h2rsqrt](../SIMT-API/math_functions/bfloat16_type/bfloat16x2_math_functions/h2rsqrt.md) | - | 0-ulp, Support denormalized numbers |

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
| [hexp](../SIMT-API/math_functions/half_type/half_math_functions/hexp.md) | - | 0-ulp, Support denormalized numbers |
| [hexp2](../SIMT-API/math_functions/half_type/half_math_functions/hexp2.md) | - | 0-ulp, Support denormalized numbers |
| [hexp10](../SIMT-API/math_functions/half_type/half_math_functions/hexp10.md) | - | 0-ulp, Support denormalized numbers |
| [hlog](../SIMT-API/math_functions/half_type/half_math_functions/hlog.md) | - | 0-ulp, Support denormalized numbers |
| [hlog2](../SIMT-API/math_functions/half_type/half_math_functions/hlog2.md) | - | 0-ulp, Support denormalized numbers |
| [hlog10](../SIMT-API/math_functions/half_type/half_math_functions/hlog10.md) | - | 0-ulp, Support denormalized numbers |
| [hsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hsqrt.md) | - | 0-ulp, Support denormalized numbers |
| [hrsqrt](../SIMT-API/math_functions/half_type/half_math_functions/hrsqrt.md) | - | 0-ulp, Support denormalized numbers |
| [h2tanh](../SIMT-API/math_functions/half_type/half2_math_functions/h2tanh.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp2](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp2.md) | - | 0-ulp, Support denormalized numbers |
| [h2exp10](../SIMT-API/math_functions/half_type/half2_math_functions/h2exp10.md) | - | 0-ulp, Support denormalized numbers |
| [h2log](../SIMT-API/math_functions/half_type/half2_math_functions/h2log.md) | - | 0-ulp, Support denormalized numbers |
| [h2log2](../SIMT-API/math_functions/half_type/half2_math_functions/h2log2.md) | - | 0-ulp, Support denormalized numbers |
| [h2log10](../SIMT-API/math_functions/half_type/half2_math_functions/h2log10.md) | - | 0-ulp, Support denormalized numbers |
| [h2sqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2sqrt.md) | - | 0-ulp, Support denormalized numbers |
| [h2rsqrt](../SIMT-API/math_functions/half_type/half2_math_functions/h2rsqrt.md) | - | 0-ulp, Support denormalized numbers |

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
| [acosf](../SIMT-API/math_functions/float_math_functions/acosf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [acoshf](../SIMT-API/math_functions/float_math_functions/acoshf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [asinf](../SIMT-API/math_functions/float_math_functions/asinf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [asinhf](../SIMT-API/math_functions/float_math_functions/asinhf.md) | - | 3-ulp (full range), Support denormalized numbers |
| [atan2f](../SIMT-API/math_functions/float_math_functions/atan2f.md) | - | 3-ulp (full range), Support denormalized numbers |
| [atanf](../SIMT-API/math_functions/float_math_functions/atanf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [atanhf](../SIMT-API/math_functions/float_math_functions/atanhf.md) | - | 3-ulp (full range), Support denormalized numbers |
| [cbrtf](../SIMT-API/math_functions/float_math_functions/cbrtf.md) | - | 1-ulp (full range), Support denormalized numbers |
| [ceilf](../SIMT-API/math_functions/float_math_functions/ceilf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [cosf](../SIMT-API/math_functions/float_math_functions/cosf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [coshf](../SIMT-API/math_functions/float_math_functions/coshf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [cospif](../SIMT-API/math_functions/float_math_functions/cospif.md) | - | 1-ulp (full range), Support denormalized numbers |
| [cyl_bessel_i0f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i0f.md) | - | 6-ulp (full range), Support denormalized numbers |
| [cyl_bessel_i1f](../SIMT-API/math_functions/float_math_functions/cyl_bessel_i1f.md) | - | 6-ulp (full range), Support denormalized numbers |
| [erfcf](../SIMT-API/math_functions/float_math_functions/erfcf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [erfcinvf](../SIMT-API/math_functions/float_math_functions/erfcinvf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [erfcxf](../SIMT-API/math_functions/float_math_functions/erfcxf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [erff](../SIMT-API/math_functions/float_math_functions/erff.md) | - | 2-ulp (full range), Support denormalized numbers |
| [erfinvf](../SIMT-API/math_functions/float_math_functions/erfinvf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [exp10f](../SIMT-API/math_functions/float_math_functions/exp10f.md) | - | 2-ulp (full range), Support denormalized numbers |
| [exp2f](../SIMT-API/math_functions/float_math_functions/exp2f.md) | - | 2-ulp (full range), Support denormalized numbers |
| [expf](../SIMT-API/math_functions/float_math_functions/expf.md) | - | 1-ulp (full range), Support denormalized numbers |
| [expm1f](../SIMT-API/math_functions/float_math_functions/expm1f.md) | - | 1-ulp (full range), Support denormalized numbers |
| [fdimf](../SIMT-API/math_functions/float_math_functions/fdimf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [fdividef](../SIMT-API/math_functions/float_math_functions/fdividef.md) | - | 0-ulp (full range), Support denormalized numbers |
| [floorf](../SIMT-API/math_functions/float_math_functions/floorf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [fmaf](../SIMT-API/math_functions/float_math_functions/fmaf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [fmodf](../SIMT-API/math_functions/float_math_functions/fmodf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [frexpf](../SIMT-API/math_functions/float_math_functions/frexpf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [hypotf](../SIMT-API/math_functions/float_math_functions/hypotf.md) | - | 3-ulp (full range), Support denormalized numbers |
| [ilogbf](../SIMT-API/math_functions/float_math_functions/ilogbf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [j0f](../SIMT-API/math_functions/float_math_functions/j0f.md) | - | 9-ulp for abs(x) < 8; otherwise, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
| [j1f](../SIMT-API/math_functions/float_math_functions/j1f.md) | - | 9-ulp for abs(x) < 8; otherwise, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
| [jnf](../SIMT-API/math_functions/float_math_functions/jnf.md) | - | For n = 128, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
| [ldexpf](../SIMT-API/math_functions/float_math_functions/ldexpf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [lgammaf](../SIMT-API/math_functions/float_math_functions/lgammaf.md) | - | 6-ulp (outside interval -10.001 ... -2.264; larger inside), Support denormalized numbers |
| [llrintf](../SIMT-API/math_functions/float_math_functions/llrintf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [llroundf](../SIMT-API/math_functions/float_math_functions/llroundf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [log10f](../SIMT-API/math_functions/float_math_functions/log10f.md) | - | 2-ulp (full range), Support denormalized numbers |
| [log1pf](../SIMT-API/math_functions/float_math_functions/log1pf.md) | - | 1-ulp (full range), Support denormalized numbers |
| [log2f](../SIMT-API/math_functions/float_math_functions/log2f.md) | - | 1-ulp (full range), Support denormalized numbers |
| [logbf](../SIMT-API/math_functions/float_math_functions/logbf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [logf](../SIMT-API/math_functions/float_math_functions/logf.md) | - | 1-ulp (full range), Support denormalized numbers |
| [lrintf](../SIMT-API/math_functions/float_math_functions/lrintf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [lroundf](../SIMT-API/math_functions/float_math_functions/lroundf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [modff](../SIMT-API/math_functions/float_math_functions/modff.md) | - | 0-ulp (full range), Support denormalized numbers |
| [nearbyintf](../SIMT-API/math_functions/float_math_functions/nearbyintf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [norm3df](../SIMT-API/math_functions/float_math_functions/norm3df.md) | - | 3-ulp (full range), Support denormalized numbers |
| [norm4df](../SIMT-API/math_functions/float_math_functions/norm4df.md) | - | 3-ulp (full range), Support denormalized numbers |
| [normcdff](../SIMT-API/math_functions/float_math_functions/normcdff.md) | - | 5-ulp (full range), Support denormalized numbers |
| [normcdfinvf](../SIMT-API/math_functions/float_math_functions/normcdfinvf.md) | - | 5-ulp (full range), Support denormalized numbers |
| [powf](../SIMT-API/math_functions/float_math_functions/powf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [rcbrtf](../SIMT-API/math_functions/float_math_functions/rcbrtf.md) | - | 1-ulp (full range), Support denormalized numbers |
| [remainderf](../SIMT-API/math_functions/float_math_functions/remainderf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [remquof](../SIMT-API/math_functions/float_math_functions/remquof.md) | - | 0-ulp (full range), Support denormalized numbers |
| [rhypotf](../SIMT-API/math_functions/float_math_functions/rhypotf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [rintf](../SIMT-API/math_functions/float_math_functions/rintf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [rnorm3df](../SIMT-API/math_functions/float_math_functions/rnorm3df.md) | - | 2-ulp (full range), Support denormalized numbers |
| [rnorm4df](../SIMT-API/math_functions/float_math_functions/rnorm4df.md) | - | 2-ulp (full range), Support denormalized numbers |
| [roundf](../SIMT-API/math_functions/float_math_functions/roundf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [rsqrtf](../SIMT-API/math_functions/float_math_functions/rsqrtf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [scalblnf](../SIMT-API/math_functions/float_math_functions/scalblnf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [scalbnf](../SIMT-API/math_functions/float_math_functions/scalbnf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [sincosf](../SIMT-API/math_functions/float_math_functions/sincosf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [sincospif](../SIMT-API/math_functions/float_math_functions/sincospif.md) | - | 2-ulp (full range), Support denormalized numbers |
| [sinf](../SIMT-API/math_functions/float_math_functions/sinf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [sinhf](../SIMT-API/math_functions/float_math_functions/sinhf.md) | - | 3-ulp (full range), Support denormalized numbers |
| [sinpif](../SIMT-API/math_functions/float_math_functions/sinpif.md) | - | 2-ulp (full range), Support denormalized numbers |
| [sqrtf](../SIMT-API/math_functions/float_math_functions/sqrtf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [tanf](../SIMT-API/math_functions/float_math_functions/tanf.md) | - | 4-ulp (full range), Support denormalized numbers |
| [tanhf](../SIMT-API/math_functions/float_math_functions/tanhf.md) | - | 2-ulp (full range), Support denormalized numbers |
| [tanpif](../SIMT-API/math_functions/float_math_functions/tanpif.md) | - | 2-ulp (full range), Support denormalized numbers |
| [tgammaf](../SIMT-API/math_functions/float_math_functions/tgammaf.md) | - | 5-ulp (full range), Support denormalized numbers |
| [truncf](../SIMT-API/math_functions/float_math_functions/truncf.md) | - | 0-ulp (full range), Support denormalized numbers |
| [y0f](../SIMT-API/math_functions/float_math_functions/y0f.md) | - | 9-ulp for abs(x) < 8; otherwise, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
| [y1f](../SIMT-API/math_functions/float_math_functions/y1f.md) | - | 9-ulp for abs(x) < 8; otherwise, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
| [ynf](../SIMT-API/math_functions/float_math_functions/ynf.md) | - | ceil(2 + 2.5n)-ulp for abs(x) < n; otherwise, the maximum absolute error is 2.2 x 10^-6; Support denormalized numbers |
