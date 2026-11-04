# Gatherb\(ISASI\)<a name="ZH-CN_TOPIC_0000001493030392"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:39:01.872Z pushedAt=2026-10-28T03:14:39.570Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/scatter_gather/Gatherb_ISASI_res.md#id1 -->

## Description<a name="section17600329101418"></a>

The header file path is `"basic_api/kernel_operator_vec_gather_intf.h"`.

The Gatherb API accepts an input tensor (src) and a DataBlock offset tensor (offset). Based on the index position, it collects the input tensor into the result tensor (dst) by DataBlock (32 bytes).

The computation principle and reference pseudocode of the API are as follows:

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

For a complete example, see [Gatherb truth value computation](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

## Prototype<a name="section15660625202219"></a>

```cpp
template <typename T>
__aicore__ inline void Gatherb(const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<uint32_t>& offset, const uint8_t repeatTime, const GatherRepeatParams& repeatParams)
```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameter description

| Parameter | Description |
| :----- | :--- |
| T | Data type of the operand. |

**Table 2**  Parameters

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand. Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned. |
| offset | Input | Address offset of each DataBlock in the source operand. Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The start address of LocalTensor must be 32-byte aligned.<br><br>This offset is relative to the base address of src. Each element value must be greater than or equal to 0, in bytes. For value requirements, see [Constraints](#constraints). |
| repeatTime | Input | Number of instruction iterations. Each iteration collects data of 8 DataBlocks. Value range: repeatTime∈[0,255]. |
| repeatParams | Input | Parameters for controlling instruction iteration.<br><br>For details about GatherRepeatParams, see [Table 3](#table3). |

**Table 3**  Description of parameters in the GatherRepeatParams structure<a id="table3"></a>

| Parameter | Meaning |
| :------- | :--- |
| dstBlkStride | Address stride between different DataBlocks of the vector destination operand within a single iteration, in DataBlocks.<br><br>**Note**: When dstBlkStride is 0, it is processed as 1 by default. |
| dstRepStride | Address stride of the same DataBlock of the vector destination operand between adjacent iterations, in DataBlocks. |
| blockNumber | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| src0BlkStride | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| src1BlkStride | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| src0RepStride | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| src1RepStride | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| repeatStrideMode | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |
| strideSizeMode | **Note**: Reserved Extension Parameter. Due to subsequent architecture upgrades, this parameter is deprecated and no longer processed. |

## Data Types

<!-- npu="950" id9 -->
For the Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id9 -->

<!-- npu="A3" id10 -->
For the Atlas A3 products, the supported data types are: uint16_t, uint32_t.
<!-- end id10 -->

<!-- npu="910b" id11 -->
For the Atlas A2 products, the supported data types are: uint16_t, uint32_t.
<!-- end id11 -->

<!-- npu="310b" id12 -->
For the Atlas 200I/500 A2 inference product, the supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t.
<!-- end id12 -->

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [Unified Buffer Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The source operand and the destination operand cannot use the same memory address.
- For operand address overlap constraints, see [General Address Constraints](../../../general_description_and_constraints.md)
- The value requirements for offset are as follows:
  - The value must ensure bit-width alignment of the src element type.
  - The offset address must be 32-byte aligned.
  - The offset address must not exceed the range of the Unified Buffer (UB) data.
  - Value range of the address offset: It must not exceed the range of uint32_t.
- Value range of repeatTime: repeatTime ∈ [0, 255].

<!-- npu="950,A3,910b" id13 -->
- When the value of count or repeatTime is 0, the behavior of this API is as follows:
  <!-- npu="A3,910b" id14 -->
  - For the following models, when the value of repeatTime is 0, no computation is performed, no write is made to the destination operand, and this API is treated as a NOP (no operation).
    <!-- npu="A3" id15 -->
    - Atlas A3 products
    <!-- end id15 -->
    <!-- npu="910b" id16 -->
    - Atlas A2 products
    <!-- end id16 -->
  <!-- end id14 -->
  <!-- npu="950" id17 -->
  - For Ascend 950PR&950DT products, this API implements compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the value of repeatTime is 0, it is not guaranteed that this API is treated as a NOP (no operation).
  <!-- end id17 -->
<!-- end id13 -->

<!-- npu="A3,910b" id18 -->
- For the following models, when the value of dstBlkStride is 0, it is processed as 1 by default.
  <!-- npu="A3" id19 -->
  - Atlas A3 products
  <!-- end id19 -->
  <!-- npu="910b" id20 -->
  - Atlas A2 products
  <!-- end id20 -->
<!-- end id18 -->
## Examples<a name="section11276201527"></a>

For a complete usage example, see scenario 4 in [Gather sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

```cpp
uint32_t bufferLen = 128;
AscendC::GatherRepeatParams params{1, 8};
uint8_t repeatTime = bufferLen * sizeof(uint16_t) / 256;
AscendC::Gatherb<uint16_t>(y_buf, x_buf, offset_buf, repeatTime, params); // repeatTime is the number of repeated iterations. Each iteration collects 8 datablocks of data.
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
