# Introduction

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T05:31:58.792Z pushedAt=2026-11-18T01:25:31.189Z nl=2 -->

**NumericLimits** is a utility class used to query properties such as the maximum and minimum values of a specified data type.

The header file path is `"basic_api/kernel_operator_limits_intf.h"`.

The data type is specified through the template parameter **T**. The prototype is defined as follows:

```cpp
template <typename T>
struct NumericLimits {
    // Scalar API, returning a scalar value.
    constexpr __aicore__ static inline T Max();
    constexpr __aicore__ static inline T Lowest();
    constexpr __aicore__ static inline T Min();
    constexpr __aicore__ static inline T Infinity();
    constexpr __aicore__ static inline T NegativeInfinity();
    constexpr __aicore__ static inline T QuietNaN();
    constexpr __aicore__ static inline T SignalingNaN();
    constexpr __aicore__ static inline T DeNormMin();

    // Assign values to the first count elements of dstLocal.
    __aicore__ static inline void Max(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void Lowest(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void Min(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void Infinity(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void NegativeInfinity(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void QuietNaN(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void SignalingNaN(const LocalTensor<T> &dstLocal, uint32_t count);
    __aicore__ static inline void DeNormMin(const LocalTensor<T> &dstLocal, uint32_t count);
};
```

**Table 1**  NumericLimits class description

| API name | Description | Data Type |
| --- | --- | --- |
| [Max](Max.md) | Returns the maximum finite value of the specified data type. | int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float |
| [Lowest](Lowest.md) | Returns the lowest finite value of the specified data type, that is, the finite value x for which there is no other finite value y satisfying y < x. | int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float |
| [Min](Min.md) | Returns the minimum finite value of the specified data type. | int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float |
| [Infinity](Infinity.md) | Returns the positive infinity value of the specified data type. | half, bfloat16_t, float |
| [NegativeInfinity](NegativeInfinity.md) | Returns the negative infinity value of the specified data type. | half, bfloat16_t, float |
| [QuietNaN](QuietNaN.md) | Returns the quiet NaN value of the specified data type (the highest bit of the floating-point mantissa is 1). | half, bfloat16_t, float |
| [SignalingNaN](SignalingNaN.md) | Returns the signaling NaN value of the specified data type (the highest bit of the floating-point mantissa is 0). | half, bfloat16_t, float |
| [DeNormMin](DeNormMin.md) | Returns the smallest positive denormal value of the specified data type. | half, bfloat16_t, float |
