# GatherMask<a name="ZH-CN_TOPIC_0000001500267978"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T08:07:59.123Z -->

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
- Atlas 200I/500 A2 inference products: Supported
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

Header file path: `"basic_api/kernel_operator_vec_gather_mask_intf.h"`.

Uses the binary corresponding to the **built-in fixed mode** or the **user-defined input Tensor** value as the gather mask (the mask for data collection), and selects elements from the source operand to write into the destination operand. 1 indicates selection, and 0 indicates no selection.

- Built-in fixed mode: The data type of src1Pattern in the corresponding function prototype is the uint8\_t type. The gather mask is specified by the src1Pattern parameter and remains fixed across all repeat iterations. The value range of src1Pattern is \[1, 7\].
    - 1: 01010101...0101 \# Selects the even-indexed element in each repeat
    - 2: 10101010...1010 \# Selects the odd-indexed element in each repeat
    - 3: 00010001...0001 \# Selects the first element of every four elements in each repeat
    - 4: 00100010…0010 \# Take the second element of every four elements in each repeat
    - 5: 01000100…0100 \# Take the third element of every four elements in each repeat
    - 6: 10001000…1000 \# Take the fourth element of every four elements in each repeat
    - 7: 11111111…1111 \# Take all elements in each repeat

    <!-- npu="950" id8 -->
    Ascend 950PR/Ascend 950DT: supports modes 1-7.
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 training products/Atlas A3 inference products: support modes 1-7.
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 training products/Atlas A2 inference products: support modes 1-7.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas inference products AI Core: supports modes 1-6.
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    Atlas 200I/500 A2 inference products: support modes 1-7.
    <!-- end id12 -->

- User-defined mode: the data type of src1Pattern in the corresponding function prototype is LocalTensor. The interval between iterations is determined by src1RepeatStride, and src1Pattern is consumed continuously within an iteration.

The computation process of the GatherMask API is shown in the following figure. The parameters are set as follows: mask=70, repeatTimes=2, src0BlockStride=1, src0RepeatStride=8, src1RepeatStride=0.

The following examples are all based on the Counter mode, which supports two configuration methods:

- Configuration method 1: Each repeat operation processes mask elements, and the total computation amount is repeatTimes * mask elements. The mask value is configured as the number of elements computed in each repeat.

- Configuration method 2: The total computation amount is mask elements. mask is configured as the total computation amount.
The repeatTimes value does not take effect. The number of iterations of the instruction is determined by the source operand and mask together.

    <!-- npu="950" id13 -->
    Ascend 950PR/Ascend 950DT: Configuration method 1 is supported.
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    Atlas A3 training series products/Atlas A3 inference series products: Configuration method 1 is supported.
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    Atlas A2 training series products/Atlas A2 inference series products: Configuration method 1 is supported.
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    Atlas 200I/500 A2 inference products: Configuration method 1 is supported.
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    Atlas inference series products AI Core: Configuration method 2 is supported.
    <!-- end id17 -->

Figure 1 uses built-in fixed mode 2 for calculation, and each repeat takes elements at odd indices.

Figure 2 and Figure 3 use the user-defined mode for calculation. The custom input src1Pattern is \[0x99999999,...,0x99999999\], whose binary representation is 1001...1001, taking the first and fourth elements of every four elements.

**Figure 1**  GatherMask built-in fixed mode

![](../../../../figures/GatherMask1.png)

<!-- npu="950,A3,910b,310b,310p" id18 -->
**Figure 2**  GatherMask user-defined mode (configuration method 1)

![](../../../../figures/GatherMask2.png)
<!-- end id18 -->

<!-- npu="310p" id19 -->
**Figure 3**  GatherMask user-defined mode (configuration method 2)

![](../../../../figures/GatherMask3.png)
<!-- end id19 -->

## Prototype<a name="section620mcpsimp"></a>

- User-defined mode

    ```cpp
    template <typename T, typename U, GatherMaskMode mode = defaultGatherMaskMode>
    __aicore__ inline void GatherMask(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<U>& src1Pattern, const bool reduceMode, const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
    ```

- Built-in fixed mode

    ```cpp
    template <typename T, GatherMaskMode mode = defaultGatherMaskMode>
    __aicore__ inline void GatherMask(const LocalTensor<T>& dst, const LocalTensor<T>& src0, const uint8_t src1Pattern, const bool reduceMode, const uint32_t mask, const GatherMaskParams& gatherMaskParams, uint64_t& rsvdCnt)
    ```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand src0 and the destination operand dst. |
| U | Data type of src1Pattern in user-defined mode. |
| mode | Reserved parameter for future functions. A default value is provided, and users do not need to set this parameter. |

**Table 2** Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned. |
| src0 | Input | Source operand.<br>The type is [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br>The start address of the LocalTensor must be 32-byte aligned.<br>The data type must be consistent with that of the destination operand. |
| src1Pattern | Input | Gather mask (the mask for data collection), which is divided into built-in fixed mode and user-defined mode. Based on the binary mask corresponding to the built-in fixed mode or the binary mask corresponding to the Tensor values input by the user in user-defined mode, elements are selected from the source operand and written to the destination operand. 1 indicates selection, and 0 indicates no selection. For details, see [Description](#description).<br>Note: In built-in fixed mode, the src1RepeatStride parameter is invalid. |
| reduceMode | Input | Used to select the mask parameter mode. The data type is bool, and the supported values are as follows:<br>&bull; false: Normal mode. In this mode, 256 bytes of data are processed in each repeat, and the total computation amount is repeatTimes * 256 bytes. In this mode, the mask parameter is invalid, and it is recommended to set it to 0.<br>&bull; true: Counter mode. Depending on the meanings of parameters such as mask, this mode has two configuration methods. For details, see [Description](#description).<br>Both values support the following:<br>&bull; Configure the src0BlockStride and src0RepeatStride parameters as required.<br>&bull; Configure src1Pattern in built-in fixed mode or user-defined mode. In user-defined mode, src1RepeatStride can be configured as required.<br> |
| mask | Input | Used to control the elements that participate in the computation in each iteration. Depending on reduceMode, there are two modes:<br>&bull; Normal mode: mask is invalid, and it is recommended to set it to 0.<br>&bull; Counter mode: value range [1, 2<sup>32</sup> – 1]. In Counter mode, the mask parameter has different meanings on different versions and models. For the specific configuration rules, see the description of the reduceMode parameter above.<br> |
| gatherMaskParams | Input | Data structure that controls the operand address step size, of the GatherMaskParams type.<br>For details about the parameters, see [Table 3 GatherMaskParams structure parameters](#GatherMaskParams). |
| rsvdCnt | Output | Number of the elements retained after filtering by this instruction, corresponding to the number of valid elements in dstLocal. The data type is uint64_t. |

**Table 3** GatherMaskParams structure parameters<a id="GatherMaskParams"></a>

| Parameter | Description |
| --- | --- |
| src0BlockStride | Used to set the address step size (interval between start addresses) between different DataBlocks in the same iteration of src0. The unit is DataBlock. For details, see [dataBlockStride](../SIMD_compute/high_dimension_slicing.md). |
| repeatTimes | Number of repeated iterations. The vector computation unit reads 256 bytes of continuous data for computation each time. To complete the processing of the input data, multiple iterations (repeats) are required to read and compute all the data. repeatTimes indicates the number of iterations.<br>For details about this parameter, see [High-dimensional Slicing](../SIMD_compute/high_dimension_slicing.md). |
| src0RepeatStride | Used to set the address step size (interval between start addresses) between adjacent iterations of src0. The unit is DataBlock. For details, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). |
| src1RepeatStride | Used to set the address step size (interval between start addresses) between adjacent iterations of src1. The unit is DataBlock. For details, see [repeatStride](../SIMD_compute/high_dimension_slicing.md). |

## Data Type

<!-- npu="950" id20 -->

**Table** Data type combination cases of Ascend 950PR/Ascend 950DT

| src0 data type | src1Pattern data type | dst data type |
| --- | --- | --- |
| int8_t | uint8_t | int8_t |
| uint8_t | uint8_t | uint8_t |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id20 -->

<!-- npu="A3" id21 -->

**Table** Data type combination cases of Atlas A3 training series/Atlas A3 inference series

| src0 data type | src1Pattern data type | dst data type |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id21 -->

<!-- npu="910b" id22 -->

**Table** Data type combination cases of Atlas A2 training series/Atlas A2 inference series

| src0 data type | src1Pattern data type | dst data type |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| bfloat16_t | uint16_t | bfloat16_t |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id22 -->

<!-- npu="310b" id23 -->

**Table**  Atlas 200I/500 A2 inference product data type combination cases

| src0 data type | src1Pattern data type | dst data type |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id23 -->

<!-- npu="310p" id24 -->

**Table**  Atlas inference series product AI Core data type combination cases

| src0 data type | src1Pattern data type | dst data type |
| --- | --- | --- |
| int16_t | uint16_t | int16_t |
| uint16_t | uint16_t | uint16_t |
| half | uint16_t | half |
| int32_t | uint32_t | int32_t |
| uint32_t | uint32_t | uint32_t |
| float | uint32_t | float |

<!-- end id24 -->

## Return Value<a name="section198548421851"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md).

- For operand address overlap constraints, see [General Address Overlap Constraints](../../../general_description_and_constraints.md).

- When the built-in fixed mode is used, the src1RepeatStride parameter is invalid.

- If the mode before calling this API is Counter mode, you need to explicitly set it back to Counter mode after calling this API (the mode is set to Normal mode after the API execution ends).

## Example<a name="section642mcpsimp"></a>

-   For the user-defined Tensor example, see scenario 2 of the [Gather class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

    ```cpp
    uint32_t mask = 70; // Elements involved in the computation in each iteration
    uint64_t rsvdCnt = 0; // Number of retained elements

    // src0Local: source operand
    // src1Local: Tensor that stores the mask for data gathering
    // dstLocal: destination operand
    // reduceMode = true;    Use Counter mode
    // Parameters in {}:
    // src0BlockStride = 1;  Data is spaced by 1 DataBlock within a single iteration, that is, data is read and written continuously
    // repeatTimes = 2;      In Counter mode, this takes effect only on some product models
    // src0RepeatStride = 4; Data of the source operand is spaced by 4 DataBlocks between iterations
    // src1RepeatStride = 0; Data of src1 is spaced by 0 DataBlocks between iterations, that is, read at the original position
    AscendC::GatherMask (dstLocal, src0Local, src1Local, true, mask, { 1, 2, 4, 0 }, rsvdCnt);
    ```

- Built-in fixed mode example. For the complete example, see scenario 1 of the [Gather class example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather).

    ```cpp
    uint32_t mask = 0; // Elements involved in the computation in each iteration. In normal mode, set mask to 0.
    uint64_t rsvdCnt = 0; // Used to save the number of elements retained after filtering
    uint8_t src1Pattern = 2; // Built-in fixed mode

    // src0Local: source operand
    // src1Pattern: value of the built-in fixed mode
    // src0Local: the destination address and source address are reused
    // reduceMode = false; use the normal mode
    // The parameters in {} are:
    // src0BlockStride = 1; data is spaced by 1 Block within a single iteration, that is, data is read and written continuously
    // repeatTimes = 4; repeat the iteration 4 times
    // src0RepeatStride = 8; the source operand iteration data is spaced by 8 DataBlocks
    // src1RepeatStride = 0; repeated once, so it is set to 0
    AscendC::GatherMask(src0Local, src0Local, src1Pattern, false, mask, { 1, 4, 8, 0 }, rsvdCnt);
    ```

    The result is as follows:

    ```
    Input data src0Local: [1 2 3 ... 256]
    Input data src1Pattern: src1Pattern = 2;
    Output data dstLocal: [
    2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 66 68 70 72 74 76 78 80 82 84 86 88 90 92 94 96 98 100 102 104 106 108 110 112 114 116 118 120 122 124 126 128 130 132 134 136 138 140 142 144 146 148 150 152 154 156 158 160 162 164 166 168 170 172 174 176 178 180 182 184 186 188 190 192 194 196 198 200 202 204 206 208 210 212 214 216 218 220 222 224 226 228 230 232 234 236 238 240 242 244 246 248 250 252 254 256 undefined ..undefined]
    Output data rsvdCnt: 128
    ```
