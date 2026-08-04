# 简介

NumericLimits工具类，用于查询指定数据类型的最大值/最小值等属性。

头文件路径为：`"basic_api/kernel_operator_limits_intf.h"`。

通过模板参数T指定数据类型。原型定义如下：

```cpp
template <typename T>
struct NumericLimits {
    // 标量接口，返回值为标量
    constexpr __aicore__ static inline T Max();
    constexpr __aicore__ static inline T Lowest();
    constexpr __aicore__ static inline T Min();
    constexpr __aicore__ static inline T Infinity();
    constexpr __aicore__ static inline T NegativeInfinity();
    constexpr __aicore__ static inline T QuietNaN();
    constexpr __aicore__ static inline T SignalingNaN();
    constexpr __aicore__ static inline T DeNormMin();

    // 为dstLocal前count个元素赋值
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

**表 1**  NumericLimits类功能说明

| 接口名称 | 功能说明 | 数据类型 |
| --- | --- | --- |
| [Max](Max.md) | 返回指定数据类型的最大有限值。 | int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float |
| [Lowest](Lowest.md) | 返回指定数据类型的最低有限值，即满足无其他有限值y符合y < x的有限值x。 | int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float |
| [Min](Min.md) | 返回指定数据类型的最小有限值。 | int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float |
| [Infinity](Infinity.md) | 返回指定数据类型的正无穷大值。 | half、bfloat16_t、float |
| [NegativeInfinity](NegativeInfinity.md) | 返回指定数据类型的负无穷大值。 | half、bfloat16_t、float |
| [QuietNaN](QuietNaN.md) | 返回指定数据类型的安静NaN值（浮点尾数最高位为1）。 | half、bfloat16_t、float |
| [SignalingNaN](SignalingNaN.md) | 返回指定数据类型的发信NaN值（浮点尾数最高位为0）。 | half、bfloat16_t、float |
| [DeNormMin](DeNormMin.md) | 返回指定数据类型的最小正的非正规值。 | half、bfloat16_t、float |
