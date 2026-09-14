# UB-to-UB Masked High-Dimensional Data Transfer (Copy)<a name="ZH-CN_TOPIC_0000002575088175"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T13:05:28.255Z -->

## Applicable Products<a name="zh-cn_topic_0000002567699435_section796754519912"></a>

<!-- npu="950" id15 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id15 -->
<!-- npu="A3" id16 -->
- Atlas A3 training products/Atlas A3 inference products: Supported
<!-- end id16 -->
<!-- npu="910b" id17 -->
- Atlas A2 training products/Atlas A2 inference products: Supported
<!-- end id17 -->
<!-- npu="310b" id18 -->
- Atlas 200I/500 A2 inference products: Supported
<!-- end id18 -->
<!-- npu="310p" id19 -->
- Atlas inference products AI Core: Not supported
<!-- end id19 -->
<!-- npu="310p" id20 -->
- Atlas inference products Vector Core: Not supported
<!-- end id20 -->
<!-- npu="910" id21 -->
- Atlas training products: Not supported
<!-- end id21 -->

## Description<a name="zh-cn_topic_0000002567699435_section106841136114319"></a>

Header file path: `"basic_api/kernel_operator_data_copy_intf.h"`.

Supports data copy between Unified Buffers. The format and content remain unchanged during the copy. Mask operations and DataBlock interval operations are supported.

The supported data paths are as follows (represented by the [logical position TPosition](../../aux_data_structures/TPosition.md)):

- Unified Buffer -> Unified Buffer
    - VECIN -> VECCALC
    - VECIN -> VECOUT
    - VECCALC -> VECIN
    - VECCALC -> VECOUT
    - VECOUT -> VECIN
    - VECOUT -> VECCALC

## Prototype<a name="zh-cn_topic_0000002567699435_section82039854412"></a>

- Bitwise mask mode

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

- Continuous mask mode 

    ```cpp
    template <typename T, bool isSetMask = true>
    __aicore__ inline void Copy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime, const CopyRepeatParams& repeatParams)
    ```

## Parameters<a name="zh-cn_topic_0000002567699435_section16128134420472"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand or destination operand. For supported data types, see [Data Type](#zh-cn_topic_0000002567699435_section4219135304818). |
| isSetMask | Whether to set the mask inside the API.<br>&bull; true: the mask is set inside the API.<br>&bull; false: the mask is set outside the API. In this case, developers need to use the [SetVectorMask](../mask_operations/SetVectorMask.md) API to set the mask value. In this mode, the mask value in the API input parameter is set to the placeholder `MASK_PLACEHOLDER`, which is used only as a placeholder and has no actual meaning. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand, of type [LocalTensor](../../data_structures/LocalTensor/localtensor_introduction.md), stored in Unified Buffer. The destination address must be 32-byte aligned. |
| src | Input | Source operand, of type LocalTensor, stored in Unified Buffer. The source address must be 32-byte aligned. |
| mask[]/mask | Input | **mask** controls the elements that participate in the computation in each iteration. For details, see [Mask](../SIMD_compute/mask.md). |
| repeatTime | Input | Number of repeated iterations. The vector computation unit reads 256 consecutive bytes of data for computation each time. To complete the processing of the input data, all data must be read and computed through multiple iterations (repeat). repeatTime indicates the number of iterations.<br>For details about this parameter, see [High-Dimensional Split](../SIMD_compute/high_dimension_slicing.md).|
| repeatParams | Input | Parameter that controls the address stride of the operands.<br>Of type CopyRepeatParams, it contains parameters such as the address stride of the same DataBlock between adjacent iterations of an operand and the address stride of different DataBlocks within the same iteration of an operand. For details about CopyRepeatParams, see Table 3.<br>For the specific definition, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/interface/kernel\_struct\_data\_copy.h, and replace \$\{INSTALL\_DIR\} with the file storage path after CANN software installation. |

**Table 3** CopyRepeatParams structure parameter definition

| Parameter | Description |
| --- | --- |
| dstStride | Address stride of adjacent DataBlocks within the same iteration of the destination operand. Data type: uint16_t. Value range: dstStride∈[0, 65535]. Unit: DataBlock (32B). |
| srcStride | Address stride of adjacent DataBlocks within the same iteration of the source operand. Data type: uint16_t. Value range: srcStride∈[0, 65535]. Unit: DataBlock (32B) |
| dstRepeatSize | Address stride of the same DataBlock between adjacent iterations of the destination operand. Data type: uint16_t. Value range: dstRepeatSize∈[0, 4095]. Unit: DataBlock (32B). |
| srcRepeatSize | Address stride of the same DataBlock between adjacent iterations of the source operand. Data type: uint16_t. Value range: srcRepeatSize∈[0, 4095]. Unit: DataBlock (32B). |

## Data Type<a name="zh-cn_topic_0000002567699435_section4219135304818"></a>

The source operand and destination operand support the same data types.

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT, the supported data types are: uint8_t, int8_t, hifloat8_t, fp8_e4m3fn_t, fp8_e5m2_t, fp4x2_e2m1_t, fp4x2_e1m2_t, fp8_e8m0_t, uint16_t, int16_t, half, bfloat16_t, float, uint32_t, int32_t, uint64_t, int64_t.
<!-- end id1 -->

<!-- npu="A3" id2 -->
- Atlas A3 Training Series/Atlas A3 Inference Series, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id2 -->

<!-- npu="910b" id3 -->
- Atlas A2 Training Series/Atlas A2 Inference Series, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id3 -->

<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 Inference Product, the supported data types are: int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float.
<!-- end id4 -->

## Return Value

None

## Constraints<a name="zh-cn_topic_0000002567699435_section2045914466492"></a>

- The address in the Unified Buffer must be 32-byte aligned.
- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md#general-address-overlap-constraints).
- The values of the CopyRepeatParams structure parameters must be within the value range:

    **Table 4**  Value range of CopyRepeatParams structure parameters

    | Parameter | Value Range |
    | --- | --- |
    | dstStride | [0, 65535] |
    | srcStride | [0, 65535] |
    | dstRepeatSize | [0, 4095] |
    | srcRepeatSize | [0, 4095] |

<!-- npu="A3,910b" id7 -->
- repeatTime = 0 indicates that no computation is performed and no write is made to the destination operand. The API is treated as a NOP (no operation). This description applies to the following models:
  <!-- npu="A3" id5 -->
  - Atlas A3 training products/Atlas A3 inference products
  <!-- end id5 -->
  <!-- npu="910b" id6 -->
  - Atlas A2 training products/Atlas A2 inference products
  <!-- end id6 -->
<!-- end id7 -->

- Like vector computation APIs, Copy supports use with mask operation APIs. However, when the Counter mode is used with the high-dimensional split computation API, it differs from the [general Counter mode](../SIMD_compute/mask.md). The specific differences are as follows:
    - General Counter mode: Mask represents **the number of elements involved in the entire vector computation, and the iteration count does not take effect.**
    - Counter mode of the Copy high-dimensional split computation API: Mask represents **the number of elements processed in each Repeat, and the iteration count takes effect.**

    In the Counter mode of the Copy high-dimensional split computation API shown in Figure 1, the iteration count takes effect, and the number of DataBlocks involved in the computation in each iteration can be calculated from the Mask:

    $$
    \text{N}=\text{ceil}(\text{Mask}*\text{sizeof(T)}/\text{datablockSize})
    $$

    **Figure 1**  Counter mode of the Copy high-dimensional split computation API  
    ![](../../../../figures/repeat-times-12.png)

## Calling Example<a name="zh-cn_topic_0000002567699435_section088124295117"></a>

```cpp
    if constexpr (scenarioNum == 1) {
        // Scenario 1: Copy [1, 512] data, mask continuous mode, source and destination space are the same size.
        // mask=64 means each iteration processes 64 int32 elements.
        // repeatTime=8 means 8 iterations, processing 512 elements in total.
        // srcStride=1, dstStride=1: no gap between DataBlocks within the same iteration.
        // srcRepeatSize=8, dstRepeatSize=8: the stride between adjacent iterations is 8 blocks (each block has 8 int32 elements).
        uint64_t mask = 64;
        AscendC::Copy(dstLocal, srcLocal, mask, 8, {1, 1, 8, 8});
    } else if constexpr (scenarioNum == 2) {
        // Scenario 2: Copy [18, 8] data from [18, 64] data.
        // Source data shape: [18, 64], 1152 elements in total.
        // Destination data shape: [18, 8], 144 elements in total.
        // Each iteration processes 8 elements in one row, 18 iterations in total.
        // mask=8: Process 8 elements per iteration.
        // repeatTime=18: Iterate 18 times (corresponding to 18 rows).
        // srcStride=1, dstStride=1: No gap within the same iteration.
        // srcRepeatSize=8: Skip 64 elements after each source iteration (jump to the start of the next row).
        // dstRepeatSize=1: Skip 8 elements after each destination iteration (compactly arrange the next row).
        uint64_t mask = 8;
        AscendC::Copy(dstLocal, srcLocal, mask, 18, {1, 1, 1, 8});
    } else if constexpr (scenarioNum == 3) {
        // Scenario 3: Copy [18, 8] data from [18, 64] in Counter mode.
        // Source data shape: [18, 64], a total of 1152 elements.
        // Destination data shape: [18, 8], a total of 144 elements.
        // In Counter mode, Mask represents the number of elements processed in each Repeat.
        // mask=144: Process 144 elements per iteration.
        // repeatTime=1: Iterate once (corresponding to line 18).
        // srcStride=8: The address stride between two DataBlocks of the source data is 8 (select the starting DataBlock of each row each time).
        // dstStride=1: The destination data is stored contiguously without gaps.
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

For the complete sample, see [Copy sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/00_data_movement/copy_ub2ub).
