# Built-in Macros

<!-- md-trans-meta sourceCommit=2c97acf47d28b2c9fa27e8b745084f6e4f3eedea translatedAt=2026-09-07T10:06:35.687Z pushedAt=2026-09-09T01:49:40.950Z -->

The following table lists some commonly used built-in macros, covering special values of the bfloat16, half, and float floating-point types, as well as commonly used mathematical constants, logarithms, and boundary values. When developers need to handle floating-point boundaries or mathematical constants, they can directly refer to and use these built-in macros without manually defining or looking up specific values, allowing them to focus more on the algorithm logic.

**Table 1**  List of commonly used built-in macros

| Macro Name | Description | Required Header Files |
| --- | --- | --- |
| ASCRT_INF_BF16 | Defines the positive infinity value of type bfloat16. | asc_bf16.h |
| ASCRT_MAX_NORMAL_BF16 | Defines the maximum representable value of type bfloat16. | asc_bf16.h |
| ASCRT_MIN_DENORM_BF16 | Defines the minimum representable value of type bfloat16. | asc_bf16.h |
| ASCRT_NAN_BF16 | Defines the NaN value of type bfloat16. | asc_bf16.h |
| ASCRT_NEG_ZERO_BF16 | Defines the negative zero value of type bfloat16. | asc_bf16.h |
| ASCRT_ONE_BF16 | Defines the 1.0 value of type bfloat16. | asc_bf16.h |
| ASCRT_ZERO_BF16 | Defines the positive zero value of type bfloat16. | asc_bf16.h |
| ASCRT_INF_FP16 | Defines the positive infinity value of type half. | asc_fp16.h |
| ASCRT_MAX_NORMAL_FP16 | Defines the maximum representable value of type half. | asc_fp16.h |
| ASCRT_MIN_DENORM_FP16 | Defines the minimum representable value of type half. | asc_fp16.h |
| ASCRT_NAN_FP16 | Defines the NaN value of type half. | asc_fp16.h |
| ASCRT_NEG_ZERO_FP16 | Defines the negative zero value of type half. | asc_fp16.h |
| ASCRT_ONE_FP16 | Defines the 1.0 value of type half. | asc_fp16.h |
| ASCRT_ZERO_FP16 | Defines the positive zero value of type half. | asc_fp16.h |
| ASCRT_INF_F | Defines the positive infinity value of type float. | math_constants.h |
| ASCRT_NAN_F | Defines the NaN value of type float. | math_constants.h |
| ASCRT_MIN_DENORM_F | Defines the minimum representable value of type float. | math_constants.h |
| ASCRT_MAX_NORMAL_F | Defines the maximum representable value of type float. | math_constants.h |
| ASCRT_NEG_ZERO_F | Defines the negative zero value of type float. | math_constants.h |
| ASCRT_ZERO_F | Defines the positive zero value of type float. | math_constants.h |
| ASCRT_ONE_F | Defines the 1.0 value of type float. | math_constants.h |
| ASCRT_SQRT_HALF_F | Defines the floating-point value of $\sqrt{1/2}$. | math_constants.h |
| ASCRT_SQRT_HALF_HI_F | Defines the floating-point value of the high-order part of $\sqrt{1/2}$. | math_constants.h |
| ASCRT_SQRT_HALF_LO_F | Defines the floating-point value of the low-order part of $\sqrt{1/2}$. | math_constants.h |
| ASCRT_SQRT_TWO_F | Defines the floating-point value of $\sqrt{2}$. | math_constants.h |
| ASCRT_THIRD_F | Defines the floating-point value of $1/3$. | math_constants.h |
| ASCRT_PIO4_F | Defines the floating-point value of $\pi/4$. | math_constants.h |
| ASCRT_PIO2_F | Defines the floating-point value of $\pi/2$. | math_constants.h |
| ASCRT_3PIO4_F | Defines the floating-point value of $3\pi/4$. | math_constants.h |
| ASCRT_2_OVER_PI_F | Defines the floating-point value of $2/\pi$. | math_constants.h |
| ASCRT_SQRT_2_OVER_PI_F | Defines the floating-point value of $\sqrt{2/\pi}$. | math_constants.h |
| ASCRT_PI_F | Floating-point value of $\pi$. | math_constants.h |
| ASCRT_L2E_F | Defines the floating-point value of $\log_2(e)$. | math_constants.h |
| ASCRT_L2T_F | Defines the floating-point value of $\log_2(10)$. | math_constants.h |
| ASCRT_LG2_F | Defines the floating-point value of $\log_{10}(2)$. | math_constants.h |
| ASCRT_LGE_F | Defines the floating-point value of $\log_{10}(e)$. | math_constants.h |
| ASCRT_LN2_F | Defines the floating-point value of $\ln(2)$. | math_constants.h |
| ASCRT_LNT_F | Defines the floating-point value of $\ln(10)$. | math_constants.h |
| ASCRT_LNPI_F | Defines the floating-point value of $\ln(\pi)$. | math_constants.h |
| ASCRT_TWO_TO_M126_F | Defines the floating-point value of $2^{-126}$. | math_constants.h |
| ASCRT_TWO_TO_126_F | Defines the floating-point value of $2^{126}$. | math_constants.h |
| ASCRT_NORM_HUGE_F | Defines the maximum finite value of type float; values exceeding it overflow to inf. | math_constants.h |
| ASCRT_TWO_TO_23_F | Defines the floating-point value of 2^23. | math_constants.h |
| ASCRT_TWO_TO_24_F | Defines the floating-point value of 2^24. | math_constants.h |
| ASCRT_TWO_TO_31_F | Defines the floating-point value of 2^31. | math_constants.h |
| ASCRT_TWO_TO_32_F | Defines the floating-point value of 2^32. | math_constants.h |
| ASCRT_REMQUO_BITS_F | Extracts the lowest 3 significant bits of the quotient in the remquof function, that is, the value 3. | math_constants.h |
| ASCRT_REMQUO_MASK_F | Bit mask used to extract the lowest 3 bits from the remquof result. | math_constants.h |
| ASCRT_TRIG_PLOSS_F | 105615.0F, representing the critical threshold of precision loss in trigonometric function computation. | math_constants.h |
| ASCRT_MIN_VAL_S | Defines the minimum value of type int32. | math_constants.h |
| ASCRT_MAX_VAL_S | Defines the maximum value of type int32. | math_constants.h |