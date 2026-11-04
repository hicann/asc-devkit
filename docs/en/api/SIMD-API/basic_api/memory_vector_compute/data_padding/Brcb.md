# Brcb<a name="ZH-CN_TOPIC_0000002543424524"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:57:04.952Z pushedAt=2026-10-28T03:14:39.514Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_padding/Brcb_res.md#id1 -->

## Description<a name="section17600329101418"></a>

The header file path is `"basic_api/kernel_operator_vec_brcb_intf.h"`.

The Brcb API performs a padding operation on the input data by taking 8 numbers from the input tensor each time and filling each number into one DataBlock (32 bytes) of the result tensor.

The computation principle of the API and the reference pseudocode are as follows:

```python
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

**Table 1** Template parameter description

| Parameter | Description |
| ------ | ------ |
| T | Data type of the operand. |

**Table 2** API parameter description

| Parameter | Input/Output | Meaning |
| ------ | ------ | ------ |
| dst | Output | Target operand, whose type is LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT.<br> The start address of LocalTensor must be 32-byte aligned.|
| src | Input | Source operand, whose type is LocalTensor. The supported TPosition values are VECIN/VECCALC/VECOUT.<br> It must be 32-byte aligned.<br> Note: Each iteration reads 8 elements from src, so the number of elements in src must be no less than 8 * repeatTime.|
| repeatTime | Input | Number of instruction iterations. Each iteration completes data collection of 8 DataBlocks. Value range: repeatTime∈[0,255].|
| repeatParams | Input | Related parameters for controlling instruction iteration, whose type is BrcbRepeatParms.<br> For details about BrcbRepeatParams, see [Table 3 BrcbRepeatParams structure parameters](#table3).|

**Table 3**  BrcbRepeatParams structure parameters<a id="table3"></a>

| Parameter | Meaning |
| ------ | ------ |
| dstBlkStride | Address stride between different DataBlocks of the vector target operand within a single iteration, in DataBlocks.|
| dstRepStride | Address stride of the same DataBlock of the vector target operand between adjacent iterations, in DataBlocks.|
| blockNumber | Note: Reserved extension parameter. Currently due to subsequent architecture upgrade, this parameter is deprecated and not processed for business.|
| src1BlkStride | Note: Reserved extension parameter. Currently due to subsequent architecture upgrade, this parameter is deprecated and not processed for business.|
| src1RepStride | Note: Reserved extension parameter. Currently due to subsequent architecture upgrade, this parameter is deprecated and not processed for business.|
| repeatStrideMode | Note: Reserved extension parameter. Currently due to subsequent architecture upgrade, this parameter is deprecated and not processed for business.|
| strideSizeMode | Note: Reserved extension parameter. Currently due to subsequent architecture upgrade, this parameter is deprecated and not processed for business.|

## Data Types

<!-- npu="950" id10 -->
- For the Ascend 950PR&950DT products, the supported data types for T are: int8_t, uint8_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, and uint64_t.
<!-- end id10 -->
<!-- npu="A3" id11 -->
- For the Atlas A3 products, the supported data types for T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id11 -->
<!-- npu="910b" id12 -->
- For the Atlas A2 products, the supported data types for T are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, and float.
<!-- end id12 -->
<!-- npu="310p" id13 -->
- For the Atlas inference products AI Core, the supported data types for T are: int16_t, uint16_t, half, int32_t, uint32_t, and float.
<!-- end id13 -->

## Return Value<a name="section459672612511"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- Using the same memory address for the source operand and the target operand is not supported.
- The dstBlkStride parameter in the BrcbRepeatParams structure does not support being set to 0. Setting it to 0 may cause undefined behavior.
<!-- npu="310p" id16 -->
- For Atlas inference products AI Core, reserve 8 KB of Unified Buffer (UB) space as the temporary data storage area for the API.
<!-- end id16 -->
<!-- npu="A3,910b,950" id17 -->
- When the repeatTime parameter is set to 0, the API behaves as follows:
  <!-- npu="A3,910b" id18 -->
  - For the following models, the API does not perform computation or write to the target operand, and is treated as a NOP (no operation).
    <!-- npu="A3" id19 -->
    - Atlas A3 products
    <!-- end id19 -->
    <!-- npu="910b" id20 -->
    - Atlas A2 products
    <!-- end id20 -->
  <!-- end id18 -->
  <!-- npu="950" id21 -->
  - For Ascend 950PR&950DT products, the API is implemented for compatibility by calling the [Reg vector compute API](../../reg_vector_compute/reg_vector_compute.md) through VF. When the repeatTime parameter is set to 0, it is not guaranteed that the API is treated as a NOP (no operation).
  <!-- end id21 -->
<!-- end id17 -->
## Key Features

The source operand does not support Stride parameter configuration and always uses contiguous reads.

### Contiguous Target Operands

Use the dstBlkStride and dstRepStride parameters for contiguous writes, taking 8 elements of the half data type from the source operand and filling them into 8 contiguous DataBlocks.

dstBlkStride is 1: This indicates that the addresses of adjacent DataBlocks in the target operand are contiguous within the same iteration.

dstRepStride is 8: This indicates that the addresses of the first and last DataBlocks in the target operand are contiguous between adjacent iterations.

**Figure 1**  Contiguous target operands

![Contiguous target operands](../../../../figures/Brcb_1.png)

### Non-contiguous Target Operands

Use the dstBlkStride and dstRepStride parameters for skip writing to fill eight half-type elements from the source operand into eight non-contiguous DataBlocks.

dstBlkStride is 2: the addresses of adjacent DataBlocks in the target operand within the same iteration are non-contiguous (with an interval of one DataBlock).

dstRepStride is 16: the addresses of the first and last DataBlocks in the target operand between adjacent iterations are non-contiguous (with an interval of one DataBlock).

**Figure 2**  Non-contiguous target operands

![Non-contiguous target operands](../../../../figures/Brcb_2.png)

## Examples<a name="section11276201527"></a>

This example shows only the API-related code from the complete sample. To run the sample code, compile and execute the complete sample [Brcb sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/brcb).

```cpp
// repeatTime = 2, 128 elements one repeat, 256 elements total
// The data type of srcLocal is half, and the data type of dstLocal is half.
// dstBlkStride = 1, no gap between blocks in one repeat
// dstRepStride = 8, no gap between repeats
AscendC::Brcb(dstLocal, srcLocal, 2, {1, 8});
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
