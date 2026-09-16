# Brcb<a name="ZH-CN_TOPIC_0000002543424524"></a>

<!-- md-trans-meta sourceCommit=ab2c72ba23cbc7229f8b141e516d96812390984a translatedAt=2026-09-09T14:25:12.642Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Supported
<!-- end id13 -->
<!-- npu="310p" id14 -->
- Atlas inference products Vector Core: Not supported
<!-- end id14 -->
<!-- npu="910" id15 -->
- Atlas training products: Not supported
<!-- end id15 -->


## Description<a name="section17600329101418"></a>

Header file path: `"basic_api/kernel_operator_vec_brcb_intf.h"`.

The Brcb API performs a fill operation on the input data. Each time, it takes 8 numbers from the input tensor and fills each number into one DataBlock (32 bytes) of the result tensor.

The API computation principle and reference pseudocode are as follows:

``` python
import numpy as np

def Brcb(src, dst, repeatTime, repeatParams):
    outputType = np.dtype("uint16")
    oneDataBlockSize = 32
    if repeatParams.dstBlkStride == 0:
        repeatParams.dstBlkStride = 1
    
    for i in range(repeatTime): 
        for j in range(8):
            srcEleIndex = i * 8 + j
            dstBlockStartIndex = (oneDataBlockSize * i * repeatParams.dstRepStride + oneDataBlockSize * j * repeatParams.dstBlkStride) // outputType.itemsize
            dstBlockEndIndex = (oneDataBlockSize * i * repeatParams.dstRepStride + oneDataBlockSize * j * repeatParams.dstBlkStride + oneDataBlockSize) // outputType.itemsize
            dst[dstBlockStartIndex : dstBlockEndIndex] = src[srcEleIndex]
```

## Prototype<a name="section15660625202219"></a>

```cpp
template <typename T>
__aicore__ inline void Brcb(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t repeatTime, const BrcbRepeatParams& repeatParams)
```

## Parameters<a name="section1619484392111"></a>

**Table 1** Template parameters

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |

**Table 2** API parameters

| Parameter | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Target operand, whose type is LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (stored in the Unified Buffer).<br> The start address of LocalTensor must be 32-byte aligned.|
| src | Input | Source operand, whose type is LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT (stored in the Unified Buffer).<br> It must be 32-byte aligned.<br> Note: Eight elements are read from src in each iteration. Therefore, the number of elements in src must be no less than 8 * repeatTime.|
| repeatTime | Input | Number of instruction iterations. Each iteration collects eight DataBlocks. Value range: repeatTime∈[0,255].|
| repeatParams | Input | Parameters for controlling instruction iterations. The type is BrcbRepeatParms.<br> For details about BrcbRepeatParams, see [Table 3 BrcbRepeatParams structure parameters](#table3).|

**Table 3**  BrcbRepeatParams structure parameters<a id="table3"></a>

| Parameter | Meaning |
| ------ | ------ |
| dstBlkStride | Address stride between different DataBlocks of the vector target operand within a single iteration, in DataBlocks.|
| dstRepStride | Address stride of the same DataBlock of the vector target operand between adjacent iterations, in DataBlocks.|
| blockNumber | Note: Reserved extension parameter. Currently, due to subsequent architecture upgrade, this parameter is deprecated, and no business processing is performed on it.|
| src1BlkStride | Note: Reserved extension parameter. Currently, due to subsequent architecture upgrade, this parameter is deprecated, and no business processing is performed on it.|
| src1RepStride | Note: Reserved extension parameter. Currently, due to subsequent architecture upgrade, this parameter is deprecated, and no business processing is performed on it.|
| repeatStrideMode | Note: Reserved extension parameter. Currently, due to subsequent architecture upgrade, this parameter is deprecated, and no business processing is performed on it.|
| strideSizeMode | Note: Reserved extension parameter. Currently, due to subsequent architecture upgrade, this parameter is deprecated, and no business processing is performed on it.|

## Data Type

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT, supported data types are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
Atlas A3 training products/Atlas A3 inference products, supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
Atlas A2 training products/Atlas A2 inference products, supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="310p" id4 -->
Atlas inference products AI Core, supported data types are: int16_t, uint16_t, half, int32_t, uint32_t, float.
<!-- end id4 -->

## Return Value<a name="section459672612511"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The source operand and the target operand must not use the same memory address.
<!-- npu="A3,910b" id8 -->
- When repeatTime = 0, no computation is performed and no data is written to the target operand. The API is treated as a NOP (no operation). This constraint applies to the following models.
  <!-- npu="A3" id5 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id5 -->
  <!-- npu="910b" id6 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id6 -->
<!-- end id8 -->

- The dstBlkStride parameter in the BrcbRepeatParams structure does not support being set to 0. Setting it to 0 may cause undefined behavior.
<!-- npu="310p" id7 -->
- For the Atlas inference products AI Core, reserve 8 KB of Unified Buffer space as the temporary data storage area for the API.
<!-- end id7 -->

## Key Feature Description

The source operands do not support Stride parameter configuration and are always read contiguously.

### Contiguous Target Operands

The dstBlkStride and dstRepStride parameters are used for contiguous writes. Eight elements of the half data type are fetched from the source operand and filled into eight contiguous DataBlocks.

dstBlkStride is 1: This indicates that the addresses of adjacent DataBlocks in the target operand within the same iteration are contiguous.

dstRepStride is 8: This indicates that the addresses of the first and last DataBlocks in the target operand between adjacent iterations are contiguous.

**Figure 1**  Contiguous target operands

![Contiguous target operands](../../../../figures/Brcb_1.png)

### Target Operands Are Non-Contiguous

Use the dstBlkStride and dstRepStride parameters to perform strided writes, filling 8 elements of the half data type from the source operand into 8 non-contiguous DataBlocks.

dstBlkStride is 2: This indicates that the addresses of adjacent DataBlocks in the target operand within the same iteration are non-contiguous (with an interval of 1 DataBlock).

dstRepStride is 16: This indicates that the addresses of the first and last DataBlocks in the target operand between adjacent iterations are non-contiguous (with an interval of 1 DataBlock).

**Figure 2** Target operands are non-contiguous

![Target operands are non-contiguous](../../../../figures/Brcb_2.png)

## Example<a name="section11276201527"></a>

This sample shows only some key code. To run the sample code, copy this code segment and replace the code in the Compute function of the complete [Brcb sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/brcb) template.

```cpp
// repeatTime = 2, 128 elements one repeat, 256 elements total
// The data type of srcLocal is half, and the data type of dstLocal is half.
// dstBlkStride = 1, no gap between blocks in one repeat
// dstRepStride = 8, no gap between repeats 
AscendC::Brcb(dstLocal, srcLocal, 2, {1,8});
```

The result is as follows:

```plain
Input data (srcLocal):
[1 2 3 ... 16]
Initial value of output data (dstLocal):
[0. 0. 0. 0. 0. 0. ... 0.]
After Brcb computation, output data (dstLocal):
[1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 ... 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 15 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16 16]
```
