# GatherMask<a name="ZH-CN_TOPIC_0000001500267978"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:53:19.489Z pushedAt=2026-10-28T03:14:39.443Z nl=2 -->

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
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/compare_and_select/GatherMask_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_gather_mask_intf.h"`.

Uses the binary representation corresponding to the **built-in fixed pattern** or the **user-defined input Tensor** values as the gather mask (the mask for data collection), and selects elements from the source operand to write into the destination operand. 1 means selected, and 0 means not selected.

- Built-in fixed mode: corresponds to the case where the data type of src1Pattern in the function prototype is uint8\_t type. The gather mask is specified by the src1Pattern parameter and remains fixed across all repeat iterations. The value range of src1Pattern is \[1, 7\].
    - 1: 01010101…0101 \# each repeat takes even-indexed elements
    - 2: 10101010…1010 \# each repeat takes odd-indexed elements
    - 3: 00010001…0001 \# each repeat takes the first element of every four elements
    - 4: 00100010…0010 \# each repeat takes the second element of every four elements
    - 5: 01000100…0100 \# each repeat takes the third element of every four elements
    - 6: 10001000…1000 \# each repeat takes the fourth element of every four elements
    - 7: 11111111…1111 \# All elements are taken in each repeat.

    <!-- npu="950" id8 -->
    Ascend 950PR&950DT products: supported modes 1-7.
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 products: supported modes 1-7.
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 products: supported modes 1-7.
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas inference products AI Core: supported modes 1-6.
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    Atlas 200I/500 A2 inference products: supported modes 1-7.
    <!-- end id12 -->

- User-defined mode: In the function prototype, the data type of src1Pattern is LocalTensor. The interval between iterations is determined by src1RepeatStride, and src1Pattern is consumed continuously within an iteration.

The following figure shows the computation process of the GatherMask API. The parameters are set as follows: mask=70, repeatTimes=2, src0BlockStride=1, src0RepeatStride=8, src1RepeatStride=0.

The following examples are all based on Counter mode, which supports two configuration methods:

- Configuration method 1: Each repeat operates on mask elements, and the total data computation amount is repeatTimes * mask elements. The mask value is configured as the number of elements computed in each repeat.

- Configuration method 2: The total data computation amount is mask elements. mask is configured as the total data computation amount.
The repeatTimes value does not take effect. The number of instruction iterations is determined by the source operand and mask together.

    <!-- npu="950" id13 -->
    Ascend 950PR&950DT products support configuration method 1.
    <!-- end id13 -->

    <!-- npu="A3" id14 -->
    Atlas A3 products support configuration method 1.
    <!-- end id14 -->

    <!-- npu="910b" id15 -->
    Atlas A2 products support configuration method 1.
    <!-- end id15 -->

    <!-- npu="310b" id16 -->
    Atlas 200I/500 A2 inference products support configuration method 1.
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    Atlas inference products AI Core support configuration method 2.
    <!-- end id17 -->

Figure 1 uses built-in fixed mode 2 for computation, where each repeat takes elements at odd indices.

Figure 2 and Figure 3 use the user-defined mode for computation. The user-defined input src1Pattern is \[0x99999999,...,0x99999999\], whose binary representation is 1001...1001, taking the first and fourth elements of every four elements.

**Figure 1**  GatherMask built-in fixed mode computation diagram

![](../../../../figures/GatherMask1.png)

<!-- npu="950,A3,910b,310b,310p" id18 -->
**Figure 2**  GatherMask user-defined mode computation diagram (configuration method 1)

![](../../../../figures/GatherMask2.png)
<!-- end id18 -->

<!-- npu="310p" id19 -->
**Figure 3**  GatherMask user-defined mode computation diagram (configuration method 2)

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

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the source operand src0 and the destination operand dst. |
| U | Data type of src1Pattern in user-defined mode. |
| mode | Reserved parameter for future functions. A default value is provided, and you do not need to set this parameter. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned. |
| src0 | Input | Source operand.<br>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br>The start address of LocalTensor must be 32-byte aligned.<br>The data type must be consistent with that of the destination operand. |
| src1Pattern | Input | Gather mask (mask for data collection), which is divided into built-in fixed mode and user-defined mode. Based on the binary mask corresponding to the built-in fixed mode or the binary mask corresponding to the Tensor value input by the user, elements are selected from the source operand and written to the destination operand. 1 indicates selection, and 0 indicates no selection. For details, please refer to [Description](#section618mcpsimp).<br>Note: In built-in fixed mode, the src1RepeatStride parameter is invalid. |
| reduceMode | Input | Used to select the mask parameter mode. The data type is bool, and the following values are supported:<br>&bull; false: Normal mode. In this mode, 256 bytes of data are processed in each repeat operation, and the total computation amount is repeatTimes * 256 bytes. In this mode, the mask parameter is invalid, and it is recommended to set it to 0.<br>&bull; true: Counter mode. Depending on the meanings of parameters such as mask, this mode has two configuration methods. For details, please refer to [Description](#section618mcpsimp).<br>Both values support:<br>&bull;Configuring the src0BlockStride and src0RepeatStride parameters as needed.<br>&bull;Configuring src1Pattern as built-in fixed mode or user-defined mode. In user-defined mode, src1RepeatStride can be configured as needed.<br> |
| mask | Input | Used to control the elements that participate in computation within each iteration. Based on reduceMode, it is divided into two modes:<br>&bull; Normal mode: mask is invalid, and it is recommended to set it to 0.<br>&bull; Counter mode: value range [1, 2<sup>32</sup> – 1]. In Counter mode, the meaning of the mask parameter differs across versions and models. For specific configuration rules, refer to the reduceMode parameter description above.<br> |
| gatherMaskParams | Input | Data structure that controls the address step size of operands, of the GatherMaskParams type.<br>For specific parameter descriptions, see [Table 3 GatherMaskParams structure parameter description](#GatherMaskParams). |
| rsvdCnt | Output | Count of elements retained after filtering by this instruction, corresponding to the number of valid elements in dstLocal. The data type is uint64_t. |

**Table 3**  GatherMaskParams structure parameter description<a id="GatherMaskParams"></a>

| Parameter | Description |
| --- | --- |
| src0BlockStride | Used to set the address step size (interval between start addresses) between different DataBlocks in the same iteration of src0. The unit is DataBlock. For a detailed description, please refer to [dataBlockStride](../SIMD_compute/high_dim_split.md). |
| repeatTimes | Number of repeated iterations. The vector compute unit reads 256 consecutive bytes of data for computation each time. To complete processing of the input data, all data must be read and computed through multiple iterations (repeat). repeatTimes indicates the number of iterations.<br>For a specific description of this parameter, please refer to [high-dimensional split](../SIMD_compute/high_dim_split.md). |
| src0RepeatStride | Used to set the address step size (interval between start addresses) between adjacent iterations of src0. The unit is DataBlock. For a detailed description, please refer to [repeatStride](../SIMD_compute/high_dim_split.md). |
| src1RepeatStride | Used to set the address step size (interval between start addresses) between adjacent iterations of src1. The unit is DataBlock. For a detailed description, please refer to [repeatStride](../SIMD_compute/high_dim_split.md). For special notes on this parameter on some chip models, please refer to [related constraints](#src1RepeatStride-310p). |


## Data Type

<!-- npu="950" id20 -->

**Table**  Data type combination cases for Ascend 950PR&950DT products

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

**Table**  Data type combination cases for Atlas A3 products

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

**Table**  Data type combination cases for Atlas A2 products

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

**Table**  Data type combination cases for Atlas 200I/500 A2 inference products

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

**Table**  Data type combination cases for Atlas inference products (AI Core)

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

<!-- npu="310p" id25 -->
<a id="src1RepeatStride-310p"></a>
- For the Atlas inference products AI Core, in user-defined mode, src1RepeatStride behaves differently in Normal mode and Counter mode:
<br>&bull; Normal mode: The value of src1RepeatStride only distinguishes between 0 and non-zero. When it is set to 0, the same segment of gather mask is reused in each iteration. When it is set to a non-zero value, the gather masks read in multiple iterations are stored contiguously, and the start addresses of the gather masks in adjacent iterations are spaced 32 / sizeof(T) bytes apart. For example, when T is a 16-bit data type, the spacing is 16 bytes; when T is a 32-bit data type, the spacing is 8 bytes.
<br>&bull; Counter mode: src1RepeatStride does not take effect, and the gather masks read in multiple iterations are always stored contiguously.
<!-- end id25 -->

- If the mode is Counter mode before this API is called, you need to explicitly set it back to Counter mode after calling this API (the mode is set to Normal mode after the internal execution of the API ends).

<!-- npu="950" id26 -->
- Description of the Unified Buffer (UB) space usage. For Ascend 950PR&950DT products:
  - The built-in fixed mode occupies 8KB of UB temporary space.
  - The user-defined mode does not involve the occupation of the 8KB UB temporary space.
<!-- end id26 -->

## Example<a name="section642mcpsimp"></a>

-   User-defined tensor example. For the complete call sample, see [Gather sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather) scenario 2.

    ```cpp
    uint32_t mask = 70; // Elements involved in the computation in each iteration.
    uint64_t rsvdCnt = 0; // Number of retained elements.

    // src0Local: source operand.
    // src1Local: tensor that stores the mask for data gathering.
    // dstLocal: destination operand.
    // reduceMode = true; use Counter mode.
    // Parameters in {}:
    // src0BlockStride = 1; data is spaced by one DataBlock within a single iteration, that is, data is read and written continuously.
    // repeatTimes = 2;      In Counter mode, this takes effect only on some product models.
    // src0RepeatStride = 4; the data interval between source operand iterations is 4 DataBlocks.
    // src1RepeatStride = 0; the data interval between src1 iterations is 0 DataBlocks, that is, read at the original position.
    AscendC::GatherMask (dstLocal, src0Local, src1Local, true, mask, { 1, 2, 4, 0 }, rsvdCnt);
    ```

- Built-in fixed mode example. For the complete call sample, see [Gather samples](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/gather) scenario 1.

    ```cpp
    uint32_t mask = 0; // Elements participating in the computation in each iteration. In normal mode, set mask to 0.
    uint64_t rsvdCnt = 0; // Used to save the number of elements retained after filtering.
    uint8_t src1Pattern = 2; // Built-in fixed mode.

    // src0Local: source operand.
    // src1Pattern: value of the built-in fixed mode.
    // src0Local: the destination address is reused with the source address.
    // reduceMode = false; use normal mode.
    // Parameters in {}:
    // src0BlockStride = 1; data within a single iteration is spaced by 1 Block, that is, data is read and written continuously.
    // repeatTimes = 4; repeat the iteration 4 times.
    // src0RepeatStride = 8; source operand iteration data is spaced by 8 DataBlocks.
    // src1RepeatStride = 0; repeated once, so it is set to 0.
    AscendC::GatherMask(src0Local, src0Local, src1Pattern, false, mask, { 1, 4, 8, 0 }, rsvdCnt);
    ```

    The result is as follows:

    ```plain
    Input data src0Local: [1 2 3 ... 256]
    Input data src1Pattern: src1Pattern = 2;
    Output data dstLocal: [
    2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58 60 62 64 66 68 70 72 74 76 78 80 82 84 86 88 90 92 94 96 98 100 102 104 106 108 110 112 114 116 118 120 122 124 126 128 130 132 134 136 138 140 142 144 146 148 150 152 154 156 158 160 162 164 166 168 170 172 174 176 178 180 182 184 186 188 190 192 194 196 198 200 202 204 206 208 210 212 214 216 218 220 222 224 226 228 230 232 234 236 238 240 242 244 246 248 250 252 254 256 undefined ..undefined]
    Output data rsvdCnt: 128
    ```
