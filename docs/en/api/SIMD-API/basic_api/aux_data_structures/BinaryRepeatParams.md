# BinaryRepeatParams<a name="ZH-CN_TOPIC_0000001533390357"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-02T02:40:27.392Z pushedAt=2026-11-18T01:25:06.649Z nl=2 -->

**BinaryRepeatParams** is a data structure used to control the address stride of operands. The structure contains parameters such as the address stride of the same **DataBlock** between adjacent iterations of an operand, and the address stride of different **DataBlock**s within the same iteration of an operand.

For the description of the address stride parameter between adjacent iterations, see [repeatStride](../memory_vector_compute/SIMD_compute/high_dim_split.md#address-stride-configuration). For the description of the address stride parameter of **DataBlock** within the same iteration, see [dataBlockStride](../memory_vector_compute/SIMD_compute/high_dim_split.md#address-stride-configuration).

The structure is defined as follows:

```cpp
const int32_t DEFAULT_BLK_NUM = 8;
const int32_t DEFAULT_BLK_STRIDE = 1;
const uint8_t DEFAULT_REPEAT_STRIDE = 8;

struct BinaryRepeatParams {
    __aicore__ BinaryRepeatParams() {}
    __aicore__ BinaryRepeatParams(const uint8_t dstBlkStrideIn, const uint8_t src0BlkStrideIn,
        const uint8_t src1BlkStrideIn, const uint8_t dstRepStrideIn, const uint8_t src0RepStrideIn,
        const uint8_t src1RepStrideIn)
        : dstBlkStride(dstBlkStrideIn),
          src0BlkStride(src0BlkStrideIn),
          src1BlkStride(src1BlkStrideIn),
          dstRepStride(dstRepStrideIn),
          src0RepStride(src0RepStrideIn),
          src1RepStride(src1RepStrideIn)
    {}
    uint32_t blockNumber = DEFAULT_BLK_NUM;
    uint8_t dstBlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src0BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t src1BlkStride = DEFAULT_BLK_STRIDE;
    uint8_t dstRepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src0RepStride = DEFAULT_REPEAT_STRIDE;
    uint8_t src1RepStride = DEFAULT_REPEAT_STRIDE;
    bool repeatStrideMode = false;
    bool strideSizeMode = false;
};
```

Among them, **blockNumber**, **repeatStrideMode**, and **strideSizeMode** are reserved parameters that users do not need to care about and can use with their default values. Users need to define the DataBlock Stride parameters, including **dstBlkStride**, **src0BlkStride**, and **src1BlkStride**, as well as the Repeat Stride parameters, including **dstRepStride**, **src0RepStride**, and **src1RepStride**.
