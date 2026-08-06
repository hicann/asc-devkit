# 内置宏

下表列出了一些常用的内置宏，涵盖了bfloat16、half和float三种浮点类型的特殊值，以及常用的数学常数、对数和边界值。当开发者需要处理浮点边界或数学常量时，可以直接参考并使用这些内置宏，无需手动定义或查阅具体的数值，从而可以将更多精力集中在算法逻辑上。

**表1**  常用内置宏列表

| 宏名 | 描述 | 需要包含的头文件 |
| --- | --- | --- |
| ASCRT_INF_BF16 | 定义bfloat16类型的正无穷大值。 | asc_bf16.h |
| ASCRT_MAX_NORMAL_BF16 | 定义bfloat16类型的最大表示值。 | asc_bf16.h |
| ASCRT_MIN_DENORM_BF16 | 定义bfloat16类型的最小表示值。 | asc_bf16.h |
| ASCRT_NAN_BF16 | 定义bfloat16类型的nan值。 | asc_bf16.h |
| ASCRT_NEG_ZERO_BF16 | 定义bfloat16类型的负零值。 | asc_bf16.h |
| ASCRT_ONE_BF16 | 定义bfloat16类型的1.0值。 | asc_bf16.h |
| ASCRT_ZERO_BF16 | 定义bfloat16类型的正零值。 | asc_bf16.h |
| ASCRT_INF_FP16 | 定义half类型的正无穷大值。 | asc_fp16.h |
| ASCRT_MAX_NORMAL_FP16 | 定义half类型的最大表示值。 | asc_fp16.h |
| ASCRT_MIN_DENORM_FP16 | 定义half类型的最小表示值。 | asc_fp16.h |
| ASCRT_NAN_FP16 | 定义half类型的nan值。 | asc_fp16.h |
| ASCRT_NEG_ZERO_FP16 | 定义half类型的负零值。 | asc_fp16.h |
| ASCRT_ONE_FP16 | 定义half类型的1.0值。 | asc_fp16.h |
| ASCRT_ZERO_FP16 | 定义half类型的正零值。 | asc_fp16.h |
| ASCRT_INF_F | 定义float类型的正无穷大值。 | math_constants.h |
| ASCRT_NAN_F | 定义float类型的nan值。 | math_constants.h |
| ASCRT_MIN_DENORM_F | 定义float类型的最小表示值。 | math_constants.h |
| ASCRT_MAX_NORMAL_F | 定义float类型的最大表示值。 | math_constants.h |
| ASCRT_NEG_ZERO_F | 定义float类型的负零值。 | math_constants.h |
| ASCRT_ZERO_F | 定义float类型的正零值。 | math_constants.h |
| ASCRT_ONE_F | 定义float类型的1.0值。 | math_constants.h |
| ASCRT_SQRT_HALF_F | 定义$\sqrt{1/2}$的浮点数值。 | math_constants.h |
| ASCRT_SQRT_HALF_HI_F | 定义$\sqrt{1/2}$的高位部分浮点数值。 | math_constants.h |
| ASCRT_SQRT_HALF_LO_F | 定义$\sqrt{1/2}$的低位部分浮点数值。 | math_constants.h |
| ASCRT_SQRT_TWO_F | 定义$\sqrt{2}$的浮点数值。 | math_constants.h |
| ASCRT_THIRD_F | 定义$1/3$的浮点数值。 | math_constants.h |
| ASCRT_PIO4_F | 定义$\pi/4$的浮点数值。 | math_constants.h |
| ASCRT_PIO2_F | 定义$\pi/2$的浮点数值。 | math_constants.h |
| ASCRT_3PIO4_F | 定义$3\pi/4$的浮点数值。 | math_constants.h |
| ASCRT_2_OVER_PI_F | 定义$2/\pi$的浮点数值。 | math_constants.h |
| ASCRT_SQRT_2_OVER_PI_F | 定义$\sqrt{2/\pi}$的浮点数值。 | math_constants.h |
| ASCRT_PI_F | $\pi$的浮点数值。 | math_constants.h |
| ASCRT_L2E_F | 定义$\log_2(e)$的浮点数值。 | math_constants.h |
| ASCRT_L2T_F | 定义$\log_2(10)$的浮点数值。 | math_constants.h |
| ASCRT_LG2_F | 定义$\log_{10}(2)$的浮点数值。 | math_constants.h |
| ASCRT_LGE_F | 定义$\log_{10}(e)$的浮点数值。 | math_constants.h |
| ASCRT_LN2_F | 定义$\ln(2)$的浮点数值。 | math_constants.h |
| ASCRT_LNT_F | 定义$\ln(10)$的浮点数值。 | math_constants.h |
| ASCRT_LNPI_F | 定义$\ln(\pi)$的浮点数值。 | math_constants.h |
| ASCRT_TWO_TO_M126_F | 定义$2^{-126}$的浮点数值。 | math_constants.h |
| ASCRT_TWO_TO_126_F | 定义$2^{126}$的浮点数值。 | math_constants.h |
| ASCRT_NORM_HUGE_F | 定义float类型的最大有限值，超过该值即溢出到inf。 | math_constants.h |
| ASCRT_TWO_TO_23_F | 定义$2^{23}$的浮点数值。 | math_constants.h |
| ASCRT_TWO_TO_24_F | 定义$2^{24}$的浮点数值。 | math_constants.h |
| ASCRT_TWO_TO_31_F | 定义$2^{31}$的浮点数值。 | math_constants.h |
| ASCRT_TWO_TO_32_F | 定义$2^{32}$的浮点数值。 | math_constants.h |
| ASCRT_REMQUO_BITS_F | 在remquof函数中提取商的最低3位有效位，即数值为3。 | math_constants.h |
| ASCRT_REMQUO_MASK_F | 用于提取remquof结果中低3位比特的位掩码。 | math_constants.h |
| ASCRT_TRIG_PLOSS_F | 105615.0F，表示三角函数计算中精度损失的临界阈值。 | math_constants.h |
| ASCRT_MIN_VAL_S | 定义int32类型的最小值。 | math_constants.h |
| ASCRT_MAX_VAL_S | 定义int32类型的最大值。 | math_constants.h |
