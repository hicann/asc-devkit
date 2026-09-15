# Introduction

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:15:01.883Z -->

The **NumericLimits** utility class is used to query properties such as the maximum and minimum values of a specified data type.

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
