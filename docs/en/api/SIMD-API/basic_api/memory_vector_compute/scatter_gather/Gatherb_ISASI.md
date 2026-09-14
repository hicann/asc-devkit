# Gatherb \(ISASI\)<a name="ZH-CN_TOPIC_0000001493030392"></a>

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T12:15:47.988Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id5 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Not supported
<!-- end id9 -->
<!-- npu="310p" id10 -->
- Atlas inference products Vector Core: Not supported
<!-- end id10 -->
<!-- npu="910" id11 -->
- Atlas training products: Not supported
<!-- end id11 -->


## Description<a name="section17600329101418"></a>

Header file path: `"basic_api/kernel_operator_vec_gather_intf.h"`

The Gatherb API accepts an input tensor (src) and a DataBlock offset tensor (offset), and gathers the input tensor into the result tensor (dst) by DataBlock (32 bytes) according to the index positions.

The calculation principle and reference pseudocode of the API are as follows:

```python
import numpy as np

def Gatherb(dst, src, offset, repeatTime, repeatParams):
    inputType = np.dtype("uint16")
    outputType = np.dtype("uint16")
    oneDataBlockSize = 32;
    if repeatParams.dstBlkStride == 0:
        repeatParams.dstBlkStride = 1
    for i in range(repeatTime): 
        for j in range(8):
            srcBlockStartIndex = offset[i * 8 + j] // inputType.itemsize
            srcBlockEndIndex = srcBlockStartIndex + oneDataBlockSize // inputType.itemsize
            dstBlockStartIndex = (i * oneDataBlockSize * repeatParams.dstRepStride + j * oneDataBlockSize * repeatParams.dstBlkStride) // outputType.itemsize
            dstBlockEndIndex = (i * oneDataBlockSize * repeatParams.dstRepStride + j * oneDataBlockSize * repeatParams.dstBlkStride + oneDataBlockSize) // outputType.itemsize
            dst[dstBlockStartIndex : dstBlockEndIndex] = src[srcBlockStartIndex : srcBlockEndIndex]
```

For a complete example, see [Gatherb Truth Value Calculation](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

## Prototype<a name="section15660625202219"></a>

```cpp
template <typename T>
__aicore__ inline void Gatherb(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& offset, const uint8_t repeatTime, const GatherRepeatParams& repeatParams)
```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameters

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |

**Table 2** Parameters

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand.<br><br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT (Storage Location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| src | Input | Source operand. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT (Storage Location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| offset | Input | Address offset of each DataBlock in the source operand. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT (Storage Location: Unified Buffer).<br><br>The start address of the LocalTensor must be 32-byte aligned.<br><br>The offset is relative to the base address of src. Each element value must be greater than or equal to 0, in bytes. The value requirements are as follows:<br>•The value must ensure the bit width alignment of the src element type.<br>•The offset address must be 32-byte aligned.<br>•The offset address must not exceed the data range of the UB size.<br>•Value range of the address offset: must not exceed the range of uint32_t. |
| repeatTime | Input | Number of instruction iterations. Each iteration collects data from 8 DataBlocks. Value range: repeatTime∈[0,255].<br><br>**Note: repeatTime = 0 indicates that no computation is performed, no data is written to the destination operand, and the API is treated as a NOP (no operation).** |
| repeatParams | Input | Parameters for controlling instruction iteration.<br><br>For details about the GatherRepeatParams parameters, see [Table 3](#table3). |

**Table 3** Parameters in the GatherRepeatParams structure<a id="table3"></a>

| Parameter | Meaning |
| :------- | :--- |
| dstBlkStride | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlocks.<br><br>**Note**: When dstBlkStride is 0, it is processed as 1 by default. |
| dstRepStride | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlocks. |
| blockNumber | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| src0BlkStride | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| src1BlkStride | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| src0RepStride | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| src1RepStride | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| repeatStrideMode | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |
| strideSizeMode | **Note**: Reserved extension parameter. Due to subsequent architecture upgrades, this parameter is deprecated and is not processed. |

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT: supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 Training products/Atlas A3 Inference products: supported data types are uint16_t and uint32_t.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 Training products/Atlas A2 Inference products: supported data types are uint16_t and uint32_t.
<!-- end id3 -->

<!-- npu="310b" id4 -->
Atlas 200I/500 A2 Inference products: supported data types are int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, and int64_t.
<!-- end id4 -->

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [Unified Buffer Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The source operand and destination operand cannot use the same memory address.

## Example<a name="section11276201527"></a>

For the complete example, see scenario 4 of the [Gather class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

```cpp
uint32_t bufferLen = 128;
AscendC::GatherRepeatParams params{1, 8};
uint8_t repeatTime = bufferLen * sizeof(uint16_t) / 256;
AscendC::Gatherb<uint16_t>(y_buf, x_buf, offset_buf, repeatTime, params); // repeatTime is the number of repeated iterations. Each iteration collects data from 8 datablocks.
```

Result example:

```plain
Input data (offsetLocal): [224 192 160 128 96 64 32 0]
Input data (srcLocal): [0 1 2 3 4 5 6 7 ... 120 121 122 123 124 125 126 127]
Output data (dstGlobal):[
112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 
96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111
... 
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
]
```
