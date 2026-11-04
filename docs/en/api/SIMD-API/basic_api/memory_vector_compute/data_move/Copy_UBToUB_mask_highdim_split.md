# Copy (UBToUB Masked High-Dimensional Data Movement)<a name="ZH-CN_TOPIC_0000002575088175"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:29:43.930Z pushedAt=2026-10-28T03:14:39.480Z nl=2 -->

## Applicable Products<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_move/Copy_UBToUB_mask_highdim_split_res.md#id1 -->

## Description<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports data movement within the Unified Buffer (UB). The format and content remain unchanged during data movement, and mask operations and DataBlock interval operations are supported.

The specific supported data paths are (represented by [logical position TPosition](../../aux_data_structures/TPosition.md)):

- UB -> UB
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

## Prototype<a name="zh-cn_topic_0000002567699435_section82039854412"></a>

- **mask** bit-by-bit mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

- **mask** continuous mode 

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

## Parameters<a name="zh-cn_topic_0000002567699435_section16128134420472"></a>

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Types](#zh-cn_topic_0000002567699435_section4219135304818). |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: The mask is set inside the API.<br>&bull; false: The mask is set outside the API. You need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, of the [LocalTensor](../../data_structures/LocalTensor/LocalTensor_intro.md) type. It is stored in the UB, and the destination address must be 32-byte aligned. |
| src | Input | Source operand, of the LocalTensor type. It is stored in the UB, and the source address must be 32-byte aligned. |
| mask[]/mask | Input | mask controls the elements involved in the computation within each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, all data must be read and computed through multiple iterations (repeats). repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dim_split.md).|
| repeatParams | Input | Parameter that controls the address stride of the operands.<br>Of the CopyRepeatParams type, including the address stride of the same DataBlock between adjacent iterations of the operands, the address stride of different DataBlocks within the same iteration of the operands, and other parameters. For details about the CopyRepeatParams parameters, see Table 3.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_data_copy.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3**  CopyRepeatParams structure parameter definition

| Parameter | Description |
| --- | --- |
| dstStride | Address stride of adjacent DataBlocks within the same iteration of the destination operand. Data type: uint16_t. Value range: dstStride∈[0, 65535]. Unit: DataBlock (32B). |
| srcStride | Address stride of adjacent DataBlocks within the same iteration of the source operand. Data type: uint16_t. Value range: srcStride∈[0, 65535]. Unit: DataBlock (32B). |
| dstRepeatSize | Address stride of the same DataBlock between adjacent iterations of the destination operand. Data type: uint16_t. Value range: dstRepeatSize∈[0, 4095]. Unit: DataBlock (32B). |
| srcRepeatSize | Address stride of the same DataBlock between adjacent iterations of the source operand. Data type: uint16_t. Value range: srcRepeatSize∈[0, 4095]. Unit: DataBlock (32B). |

## Data Types<a name="zh-cn_topic_0000002567699435_section4219135304818"></a>

The data types of the source operand and the destination operand must be consistent.

<!-- npu="950" id10 -->

- Ascend 950PR&950DT products, the supported data types are: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, int64_t, uint64_t.

<!-- end id10 -->

<!-- npu="A3" id11 -->

- Atlas A3 products, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

<!-- end id11 -->

<!-- npu="910b" id12 -->

- Atlas A2 products, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

<!-- end id12 -->

<!-- npu="310b" id13 -->

- Atlas 200I/500 A2 inference products, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.

<!-- end id13 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- The address in the UB must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The values of the CopyRepeatParams structure parameters must be within the value range:

    **Table 4**  Value range of the CopyRepeatParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | dstStride | [0, 65535] |
    | srcStride | [0, 65535] |
    | dstRepeatSize | [0, 4095] |
    | srcRepeatSize | [0, 4095] |

<!-- npu="A3,910b,950" id20 -->
- When the repeatTime parameter is set to 0, the API behaves as follows:
    <!-- npu="A3,910b" id18 -->
  - For the following models, when the repeatTime parameter is set to 0, no compute operation is performed and no data is written to the destination operand. The API is treated as a NOP (no operation).
     <!-- npu="A3" id16 -->
    - Atlas A3 products
     <!-- end id16 -->
     <!-- npu="910b" id17 -->
    - Atlas A2 products
     <!-- end id17 -->
    <!-- end id18 -->
    <!-- npu="950" id19 -->
  - For Ascend 950PR&950DT products, this API is implemented for compatibility by calling the [Reg vector compute](../../reg_vector_compute/reg_vector_compute.md) API through VF. When the repeatTime parameter is set to 0, the API is not guaranteed to be treated as a NOP (no operation).
    <!-- end id19 -->
<!-- end id20 -->

- Like vector compute APIs, Copy supports use with mask operation APIs. However, when the Counter mode is used with the high-dimensional split compute API, it differs from the [general Counter mode](../SIMD_compute/mask.md#setting-the-mask-inside-the-api). The specific differences are as follows:
    - General Counter mode: Mask represents **the total number of elements involved in the entire vector compute operation, and the iteration count does not take effect.**
    - Counter mode of the Copy high-dimensional split compute API: Mask represents **the number of elements processed in each Repeat, and the iteration count takes effect.**
    In the Counter mode of the Copy high-dimensional split computation API shown in Figure 1, the iteration count takes effect, and the number of DataBlocks involved in each iteration can be calculated from Mask:

    $$
    \text{N}=\text{ceil}(\text{Mask}*\text{sizeof(T)}/\text{datablockSize})
    $$

    **Figure 1**  Counter mode of the Copy high-dimensional split computation API  
    ![](../../../../figures/repeat-times-12.png)

## Examples<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
    if constexpr (scenarioNum == 1) {
        // Scenario 1: Move [1, 512] data with mask in continuous mode, where the source and destination spaces are of the same size.
        // mask=64 means 64 int32 elements are processed per iteration.
        // repeatTime=8 means 8 iterations, processing 512 elements in total.
        // srcStride=1, dstStride=1: No gap between DataBlocks within the same iteration.
        // srcRepeatSize=8, dstRepeatSize=8: The stride between adjacent iterations is 8 blocks (each block contains 8 int32 elements).
        uint64_t mask = 64;
        AscendC::Copy(dstLocal, srcLocal, mask, 8, {1, 1, 8, 8});
    } else if constexpr (scenarioNum == 2) {
        // Scenario 2: Move [18, 8] data from [18, 64] data.
        // Source data shape: [18, 64], 1152 elements in total.
        // Destination data shape: [18, 8], 144 elements in total.
        // Process 8 elements in one row per iteration, 18 iterations in total.
        // mask=8: Process 8 elements per iteration.
        // repeatTime=18: Iterate 18 times (corresponding to 18 rows).
        // srcStride=1, dstStride=1: No gap within the same iteration.
        // srcRepeatSize=8: Skip 64 elements after each source iteration (jump to the start of the next row).
        // dstRepeatSize=1: Skip 8 elements after each destination iteration (compact layout of the next row).
        uint64_t mask = 8;
        AscendC::Copy(dstLocal, srcLocal, mask, 18, {1, 1, 1, 8});
    } else if constexpr (scenarioNum == 3) {
        // Scenario 3: Copy [18, 8] data from [18, 64] in Counter mode.
        // Source data shape: [18, 64], 1152 elements in total.
        // Destination data shape: [18, 8], 144 elements in total.
        // In Counter mode, Mask represents the number of elements processed in each Repeat.
        // mask=144: Process 144 elements per iteration.
        // repeatTime=1: Iterate once (corresponding to line 18).
        // srcStride=8: The address stride between two source DataBlocks is 8 (each time select the starting DataBlock of each row).
        // dstStride=1: Destination data is stored contiguously without gaps.
        // srcRepeatSize=8, dstRepeatSize=8: These parameters are invalid when repeatTime=1.
        AscendC::SetMaskCount();
        AscendC::SetVectorMask<int32_t, AscendC::MaskMode::COUNTER>(144);
        AscendC::Copy<int32_t, false>(dstLocal, srcLocal, AscendC::MASK_PLACEHOLDER, 1, {1, 8, 8, 8});
        AscendC::SetMaskNorm();
        AscendC::ResetMask();
    } else {
        uint64_t mask = 64;
        AscendC::Copy(dstLocal, srcLocal, mask, 8, {1, 1, 8, 8});
    }
```

For the complete sample, see [Copy sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/copy_ub2ub).
