# Compare (Result Stored in a Register)<a name="ZH-CN_TOPIC_0000001835561717"></a>

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-08T13:57:21.931Z pushedAt=2026-09-09T07:30:05.620Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->


## Description<a name="section618mcpsimp"></a>

Header file path: `"basic_api/kernel_operator_vec_cmpsel_intf.h"`

Compares two tensors element by element. If the comparison result is true, the corresponding bit of the output result is set to 1; otherwise, it is set to 0. The computation result is stored in the CmpMask 128-bit register and can be used for subsequent Select computation. You can also use the [GetCmpMask](GetCmpMask_ISASI.md) API to obtain the data stored in the register.

The API supports multiple comparison modes:

- LT: less than

- GT: greater than

- GE: greater than or equal to

- EQ: equal to

- NE: not equal to

- LE: less than or equal to

## Prototype<a name="section620mcpsimp"></a>

- Bitwise mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Compare(const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[], const BinaryRepeatParams& repeatParams)
    ```

- Continuous mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Compare(const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask, const BinaryRepeatParams& repeatParams)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand. |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| src0, src1 | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| cmpMode | Input | CMPMODE type, indicating the comparison mode, including EQ, NE, GE, LE, GT, and LT.<br>&bull; LT: src0 is less than src1 <br>&bull; GT: src0 is greater than src1 <br>&bull; GE: src0 is greater than or equal to src1 <br>&bull; EQ: src0 is equal to src1 <br>&bull; NE: src0 is not equal to src1 <br>&bull; LE: src0 is less than or equal to src1<br> |
| mask/mask[] | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatParams | Input | Parameters that control the address stride of the operands. The type is [BinaryRepeatParams](../../aux_data_structures/BinaryRepeatParams.md), which includes parameters such as the address stride of the same DataBlock between adjacent iterations of the operands and the address stride of different DataBlocks within the same iteration of the operands.<br>For details about the address stride parameters between adjacent iterations, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). For details about the address stride parameters of DataBlocks within the same iteration, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

The supported data types are half and float.

## Return Value<a name="section128671456102513"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- This API has no repeat input, and repeat defaults to 1, meaning that one instruction computes 256B of data.

## Example<a name="section642mcpsimp"></a>

In this example, the source operands src0Local and src1Local each store 64 float data elements. The example compares the data in src0Local and src1Local element by element. If an element in src0Local is less than the corresponding element in src1Local, the corresponding bit in the dstLocal result is set to 1; otherwise, it is set to 0. The dstLocal result is stored using uint8\_t type data.

This example shows only part of the key code. For the complete example, see scenario 2 of the [Compare class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/compare).

- mask continuous mode

    ```cpp
    uint64_t mask = 256 / sizeof(float); // 256 is the number of bytes processed per iteration
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // dstBlkStride, src0BlkStride, src1BlkStride = 1, no gap between blocks in one repeat
    // dstRepStride, src0RepStride, src1RepStride = 8, no gap between repeats
    AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
    ```

- mask bit-by-bit mode

    ```cpp
    uint64_t mask[2] = { UINT64_MAX, 0};
    AscendC::BinaryRepeatParams repeatParams = { 1, 1, 1, 8, 8, 8 };
    // srcBlkStride, = 1, no gap between blocks in one repeat
    // dstRepStride, srcRepStride = 8, no gap between repeats
    AscendC::Compare(src0Local, src1Local, AscendC::CMPMODE::LT, mask, repeatParams);
    ```

The result is as follows:

```
Input data (src0Local):
[ 86.72287     9.413112   17.033222  -64.10005   -66.2691    -65.57659
  15.898049   94.61241   -68.920685  -36.16883    15.62852    68.078514
 -59.724575   -9.4302225 -64.770935   66.55523   -84.60122    57.331
  60.42026   -86.78856    37.25265     8.356797  -48.544407   16.73616
  15.28083   -21.889254  -67.93181   -41.01825   -68.79465    20.169441
  44.11346   -27.419518   30.452742  -89.30283   -18.590672   32.45831
   8.392082  -57.198048   98.76846   -81.73067   -38.274437  -83.84363
  64.30617     6.028703  -20.77164    93.71867    54.190437   94.98172
 -47.447758  -65.77461    82.21715    59.953922   23.599781  -77.29708
  26.963976  -63.468987   79.97712   -70.47842    39.00433    52.36555
 -63.94925   -65.77033    26.17237   -71.904884 ]
Input data (src1Local):
[  2.2989323  51.8879    -81.49718    41.189415    6.4081917  92.566666
  53.205498  -94.47063   -75.38387    36.464787   85.60772   -28.70681
  42.58504   -76.15293    38.723816   10.006577   74.53035   -78.38537
  71.945404   -4.060528  -14.501523   28.229202   96.87876    41.558033
 -92.623215   43.318684   35.387154  -16.029816   61.544827    3.3527017
  55.806778  -93.242096   22.86275   -87.506584   35.29523     8.405956
  91.03445   -85.29485    34.30078    -3.8019252  93.40503    15.459968
 -57.99712   -74.39948   -59.900818  -43.132637  -13.123036   41.246174
 -93.01083    75.476875  -45.437893  -99.19293    13.543604   76.23386
  46.192528  -39.23934    75.9787    -38.38979     9.807722  -60.610104
 -23.062874   48.1669     89.913376   73.78631  ]
Output data (cmpMask register):
[122  86 237  94 150   3 226 242]
```
