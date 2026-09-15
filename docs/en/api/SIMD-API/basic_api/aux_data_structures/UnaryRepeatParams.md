# UnaryRepeatParams<a name="ZH-CN_TOPIC_0000001487959374"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-08-27T12:02:58.116Z -->

**UnaryRepeatParams** is a data structure used to control the address stride of operands. The structure contains parameters such as the address stride of the same **DataBlock** between adjacent iterations of an operand, and the address stride of different **DataBlock**s within the same iteration of an operand.

For details about the address stride parameter between adjacent iterations, see [repeatStride](../memory_vector_compute/SIMD_compute/high_dimension_slicing.md#address-stride-configuration). For details about the address stride parameter of **DataBlock** within the same iteration, see [dataBlockStride](../memory_vector_compute/SIMD_compute/high_dimension_slicing.md#address-stride-configuration).

The structure is defined as follows:

```cpp
const int32_t DEFAULT_BLK_NUM = 8;
const int32_t DEFAULT_BLK_STRIDE = 1;
const uint8_t DEFAULT_REPEAT_STRIDE = 8;

struct UnaryRepeatParams {
    __aicore__ UnaryRepeatParams() {}
    __aicore__ UnaryRepeatParams(const uint16_t dstBlkStrideIn, const uint16_t srcBlkStrideIn,
        const uint8_t dstRepStrideIn, const uint8_t srcRepStrideIn)
        : dstBlkStride(dstBlkStrideIn),
          srcBlkStride(srcBlkStrideIn),
          dstRepStride(dstRepStrideIn),
          srcRepStride(srcRepStrideIn)
    {}
    __aicore__ UnaryRepeatParams(const uint16_t dstBlkStrideIn, const uint16_t srcBlkStrideIn,
        const uint8_t dstRepStrideIn, const uint8_t srcRepStrideIn, const bool halfBlockIn)
        : dstBlkStride(dstBlkStrideIn),
          srcBlkStride(srcBlkStrideIn),
          dstRepStride(dstRepStrideIn),
          srcRepStride(srcRepStrideIn),
          halfBlock(halfBlockIn)
    {}
    uint32_t blockNumber = DEFAULT_BLK_NUM;
    uint16_t dstBlkStride = DEFAULT_BLK_STRIDE;
    uint16_t srcBlkStride = DEFAULT_BLK_STRIDE;
    uint8_t dstRepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t srcRepStride = DEFAULT_REPEAT_STRIDE;
    bool repeatStrideMode = false;
    bool strideSizeMode = false;
    bool halfBlock = false;
};
```

Among them, **blockNumber**, **repeatStrideMode**, and **strideSizeMode** are reserved parameters that users do not need to care about and can use with their default values. **halfBlock** indicates whether the result of the **CastDequant** instruction is written to the upper half (**halfBlock** = true) or the lower half (**halfBlock** = false) of the corresponding UB. Users need to define the DataBlock Stride parameters, including **dstBlkStride** and **srcBlkStride**, as well as the Repeat Stride parameters, including **dstRepStride** and **srcRepStride**.
