# Fixpipe (L0C to L1 Data Copy)<a id="ZH-CN_TOPIC_0000002538231198"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:33:04.901Z pushedAt=2026-10-26T07:41:44.122Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supports APIs that include the **FixpipeParamsV220**/**FixpipeParamsArch3510** parameter.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supports only APIs that include the **FixpipeParamsV220** parameter.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supports only APIs that include the **FixpipeParamsV220** parameter.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToL1_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

The header file path is: `"basic_api/kernel_operator_fixpipe_intf.h, basic_api/kernel_operator_data_copy_intf.h"`.

The matrix computation results are stored in the L0C Buffer. The Fixpipe API is used to copy the results to the L1 Buffer, and it supports operations such as on-the-fly format conversion during the copy process.

<!-- npu="950,A3,910b" id10 -->

The following figure shows the valid combinations, intermediate data types, and data paths of on-the-fly quantization, on-the-fly ReLU, and on-the-fly channel merging. In the following figure, F32-\>F16 and F32-\>BF16 are non-quantization modes that only perform Cast, while the rest are on-the-fly scalar/tensor quantization modes.

<!-- npu="A3,910b" id11 -->
**Figure 1** L0C2L1 flowchart ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id11 -->

<!-- npu="950" id13 -->
**Figure 2** L0C2L1 flowchart ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002542828493_fig1828513492547"></a>  

![](../../../../figures/L0C2L1_Function_Combination_950.png)
<!-- end id13 -->
<!-- end id10 -->

## Prototype<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

To facilitate user usage and reduce learning and understanding costs, a combined matrix copy-out API **Fixpipe** is provided for frequently used scenario combinations. The API integrates register configuration and data copy capabilities. The corresponding APIs are as follows:

**APIs that take the FixpipeParamsArch3510 parameter:**

- Path L0C Buffer(CO1)->L1 Buffer(C1), without enabling the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- Path L0C Buffer(CO1)->L1 Buffer(C1), with the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**APIs that take the FixpipeParamsV220 parameter:**

- Path L0C Buffer(CO1)->L1 Buffer(C1), without enabling the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- Path L0C Buffer(CO1)->L1 Buffer(C1), with the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

## Parameters<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Types](#zh-cn_topic_0000002511188540_section4219135304818). |
| config | Fixpipe-related configuration parameter, of the type FixpipeConfig. The values are as follows:<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR (default value)**: Enables NZ2ND, and the output data format is ND. <!-- npu="A3,910b" id17 -->For Atlas A3 products and Atlas A2 products, this does not take effect on the L0C Buffer -> L1 Buffer path. <!-- end id17 --><br>&nbsp;&nbsp;&bull; CFG_NZ: NZ2NZ, and the output data format is NZ.<br>&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR: Enables NZ2DN, and the output data format is DN. This is supported only by Ascend 950PR&950DT products, and is not supported by Atlas A2 products or Atlas A3 products.<br><br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is in the Unified Buffer (UB). <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // Enables NZ2ND, and the output data format is ND.<br>    COLUMN_MAJOR, // Enables NZ2DN, and the output data format is DN.<br>};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; |
| S | Data type of the parameter cbufWorkspace, that is, the data type of the on-the-fly quantization parameter.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and cbufWorkspace use basic data types, the template parameter S must be of the uint64_t type; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull;When the destination operand, source operand, and cbufWorkspace use the TensorTrait type, the LiteType of the template parameter S must be of the uint64_t type; otherwise, compilation fails.<br>The template parameter following the template parameter S is used only for the preceding data type check, and users do not need to pay attention to it. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the type LocalTensor. The data format is NZ. The NZ address must meet 32-byte alignment. <!-- npu="950" id20 --><br>For Ascend 950PR&950DT products, the ND and DN data formats are also supported, and the address must meet 32-byte alignment. <!-- end id20 --> |
| src | Input | Source operand, of the type LocalTensor. The supported physical address is L0C Buffer (TPosition is CO1), which is the result computed by the Mmad API. The data format is NZ, and the address must meet 64-byte alignment. |
| intriParams | Input | Fixpipe copy parameter. For the specific definition, see "basic_api/kernel_struct_fixpipe.h".<br>For the parameter description, see the structure description of the Fixpipe copy parameters (FixpipeParamsArch3510 and FixpipeParamsV220). |
| cbufWorkspace | Input | Quantization parameter, of the type `LocalTensor<uint64_t>`. The supported physical address is L1 Buffer (TPosition is C1), and the address must meet 32-byte alignment.<br>&nbsp;&nbsp;&bull;Supported when quantPre is VDEQF16, VQF322B8_PRE, or VREQ8.<br><!-- npu="950" id21 -->&nbsp;&nbsp;&bull;For Ascend 950PR&950DT products, in addition to the preceding modes, VQF322FP8_PRE, VQF322HIF8_PRE, VQF322HIF8_PRE_HYBRID, VQS322BF16_PRE, VQF322F16_PRE, VQF322BF16_PRE, and VQF322F32_PRE are also supported.<br><!-- end id21 -->For the introduction to quantPre, see the quantPre part in the Fixpipe copy parameter structure. |

**Table 3** Structure description of the Fixpipe copy parameter (FixpipeParamsArch3510)

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ matrix in the N direction. The value range is nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit function is not enabled, nSize must be a multiple of 16.<br>&nbsp;&nbsp;&bull; If NZ2ND is enabled, nSize*sizeof(T) must be a multiple of 32.<br>**Note: nSize=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ matrix in the M direction. The value range is mSize∈[0, 65535]. If NZ2DN is enabled, mSize*sizeof(T) must be a multiple of 32.<br>**Note: mSize=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Address offset of the start address of adjacent Z layouts in the source NZ matrix. The value range is srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be set to mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &bull; When NZ2ND/NZ2DN is not enabled ([NZ2NZ copy](../cube_store_key_features/NZ2NZ.md)): address offset of the start address of adjacent Z layouts in the destination NZ matrix. The value is not 0, in the unit of element. (Compared with dstStride in the Fixpipe copy parameter (FixpipeParamsV220), the unit has changed. In FixpipeParamsV220, the unit of dstStride is datablock (32 bytes), while in FixpipeParamsArch3510, the unit of dstStride is element.)<br>&bull; When [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) is enabled: number of elements in each row of the destination ND/DN matrix. The value is not 0, in the unit of element. |
| [quantPre](../cube_store_key_features/accompanying_quantization.md) | Optional input |Used to control the quantization mode, of the QuantMode_t type. The default value is `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter must be configured manually by the user, and the corresponding quantization mode is not automatically derived.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disables quantization.<br>    F322F16,               // Float32_2_Float16: casts float to half, with the cast mode of CAST_RINT.<br>    F322BF16,              // Float32_2_BFloat16: casts float to bfloat16_t, with the cast mode of CAST_RINT.<br>    DEQF16,                // DeQuant_Float16: quantizes int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: quantizes int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: quantizes int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: quantizes int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantizes float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantizes float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantizes float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: quantizes int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: quantizes float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: quantizes float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: quantizes float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: quantizes float to float, scalar quantization. The precision can reach dual-0.1% but cannot reach dual-0.01%. If dual-0.01% precision is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: quantizes float to float, tensor quantization. The precision can reach dual-0.1% but cannot reach dual-0.01%. If dual-0.01% precision is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>}; |
| deqScalar | Optional input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the quantPre quantization mode is [on-the-fly quantization](../cube_store_key_features/accompanying_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/accompanying_relu.md) | Optional input | Switch for enabling NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional input | unitFlag is a fine-grained parallelism between the Mmad instruction and the Fixpipe instruction. After this function is enabled, the computation result is moved out each time the hardware finishes computing one fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this function is enabled, the unitFlag values of the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter configuration scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the description of key features in Mmad computation. |
| params | Optional input | Used to select and configure different on-the-fly format conversions (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of the TransformParams type. The TransformParams structure is a type selector based on template parameters, used to automatically select the corresponding parameter type at compile time according to the template parameters used when defining the FixpipeParamsArch3510 copy parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the CO2Layout layout type is ROW_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2ND conversion. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: number of source NZ matrices, that is, the number of ND matrices to be transferred. The value range is ndNum∈[0, 65535]. **Note: ndNum=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: interval between the start addresses of different NZ matrices. The value range is srcNdStride∈[0, 65535], in the unit of C0_SIZE. When ndNum is set to 1, srcNdStride can be set to 0, and it does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: offset between the start addresses of adjacent destination ND matrices. The value range is dstNdStride∈[1, 2^32 - 1], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0, and it does not take effect.<br><br>When the CO2Layout layout type is COLUMN_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination location, with NZ2DN conversion. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: number of DN matrices to be transferred. The value range is dnNum∈[0, 65535]. **Note: dnNum=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzMatrixStride: offset between different source NZ matrices (head to head). The value range is srcNzMatrixStride∈[0, 65535], in the unit of C0_SIZE. When dnNum is set to 1, srcNzMatrixStride can be set to 0, and it does not take effect.<br>&nbsp;&nbsp;&bull; dstDnMatrixStride: offset between the start addresses of adjacent destination DN matrices. The value range is dstDnMatrixStride∈[0, 2^32 - 1], in the unit of element. When dnNum is set to 1, dstDnMatrixStride can be set to 0, and it does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: address offset of adjacent rows in the NZ fractal of the source matrix (head to head). The value range is srcNzC0Stride∈[0, 65535], in the unit of C0_SIZE. When NZ2DN is enabled, srcNzC0Stride cannot be 0.<br><br>When the CO2Layout layout type is NZ, it is the normal copy DMA mode, indicating normal data movement from L0C Buffer to the destination location. |
| dualDstCtrl | Optional input | This parameter takes effect only on the L0C Buffer to UB path. |
| subBlockId | Optional input | This parameter takes effect only on the L0C Buffer to UB path. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | This parameter takes effect only on the L0C Buffer to Global Memory path. |

**Table 4** Structure description of the Fixpipe copy parameter (FixpipeParamsV220)

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ matrix in the N direction. The value range is nSize∈[0, 4095], and nSize must be a multiple of 16.<br>**Note: nSize=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ matrix in the M direction. The value range is mSize∈[0, 65535].<br>**Note: mSize=0 indicates that no copy is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Address offset of the start address of adjacent Z layouts in the source NZ matrix. The value range is srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be set to mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | Address offset of the start address of adjacent Z layouts in the destination NZ matrix. The value is not 0, in the unit of datablock (32 bytes). |
| quantPre | Optional input | Used to control the quantization mode, of the QuantMode_t type. The default value is `QuantMode_t::NoQuant`. The specific definition is as follows:<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, this parameter must be set to `QuantMode_t::F322F16`/`QuantMode_t::F322BF16`.<br>Note: This parameter must be configured manually by the user, and the corresponding quantization mode is not automatically derived. The NoQuant mode is not supported on this path.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Disables quantization. This is not supported on this path.<br>    F322F16,      // Float32_2_Float16: casts float to half, with the cast mode of CAST_RINT.<br>    F322BF16,     // Float32_2_BFloat16: casts float to bfloat16_t, with the cast mode of CAST_RINT.<br>    DEQF16,       // DeQuant_Float16: quantizes int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: quantizes int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: quantizes float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: quantizes int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: quantizes int32_t to int8_t/uint8_t, tensor quantization.<br>}; |
| deqScalar | Optional input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the quantPre quantization mode is [on-the-fly quantization](../cube_store_key_features/accompanying_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/accompanying_relu.md) | Optional input | Switch for enabling NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional input | unitFlag is a fine-grained parallelism between the Mmad instruction and the Fixpipe instruction. After this function is enabled, the computation result is moved out each time the hardware finishes computing one fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this function is enabled, the unitFlag values of the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter configuration scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the description of key features in Mmad computation. |
| [channelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | Takes effect only on the L0C Buffer (CO1)->GM path. Set this parameter to false on this path. |
| ndNum | Optional input | Takes effect only on the L0C Buffer (CO1)->GM path. No additional setting is required on this path. |
| srcNdStride | Optional input | Takes effect only on the L0C Buffer (CO1)->GM path. No additional setting is required on this path. |
| dstNdStride | Optional input | Takes effect only on the L0C Buffer (CO1)->GM path. No additional setting is required on this path. |

## Data Type<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**Supported data type combinations for the source matrix and destination matrix**

<!-- npu="950" id22 -->
For the Ascend 950PR&950DT products, the supported data type combinations are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |

<!-- end id22 -->

<!-- npu="A3" id23 -->
For the Atlas A3 products, the supported data type combinations are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |

<!-- end id23 -->

<!-- npu="910b" id24 -->
For the Atlas A2 products, the supported data type combinations are as follows:

| source matrix (L0C Buffer) | destination matrix (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |

<!-- end id24 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- The following describes the case where the quantization input is of the float32 data type:
    - The standard IEEE 754 float32 format is: 1 sign bit, 8 exponent bits, and 23 mantissa bits. The float32 format supported by the current AI processor is: 1 sign bit, 8 exponent bits, and 10 mantissa bits.
    - If the user provides standard IEEE 754 float32 input, the API internally converts it to the float32 format supported by the processor for computation. In this case, if standard IEEE 754 float32 data is used during golden data generation, a precision mismatch may occur. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source matrix NZ format address must be 64-byte aligned, and the destination matrix NZ format address must be 32-byte aligned.
- When any of the copied **mSize**, **nSize**, or **ndNum** is 0, the instruction is not executed. A warning is reported when **ndNum** is 0.
- The quantization and ReLU parameters cannot be inf/nan or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware reports no warning or error, and the write order of the overlapping data is not guaranteed.
- Enabling the **unitFlag** feature requires enabling **Mmad** at the same time.
<!-- npu="950" id27 -->
- When NZ2DN is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
<!-- end id27 -->
<!-- npu="A3,910b" id28 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id29 -->
    Atlas A3 products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    Atlas A2 products
    <!-- end id30 -->
    For floating-point inf/nan input and output, the CTRL\[48\] bit of the CTRL register (control register) can be set to control the saturation mode when floating-point values are quantized and moved out:

    - Non-saturation mode: Set CTRL\[48\] to 1'b1, and inf/nan are output as-is.
    - Saturation mode: Set CTRL\[48\] to 1'b0, and inf output is saturated to ±MAX, while nan output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
    For a summary of the boundary values of the L0C Buffer-to-L1 Buffer copy API, see [Summary of Boundary Values of Matrix Computation Move-Out APIs](../../../../appendix/matrix_compute_move_out_interface_boundary_value_summary.md).
<!-- end id28 -->
<!-- npu="950" id31 -->
- Special value/boundary value constraints for the Ascend 950PR&950DT products:

    For floating-point inf/nan input and output, the CTRL\[48\] bit of the CTRL register (control register) can be set to control the saturation mode when floating-point values are quantized and moved out;

    For the fp8\_e4m3fn\_t type, finer settings can be made through the CTRL\[48\] bit of the CTRL register (control register);

    - Non-saturation mode: CTRL\[48\] is set to 1'b1, and inf/nan remain output as-is. For the fp8\_e4m3fn\_t type, if the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is nan.
    - Saturation mode: CTRL\[48\] is set to 1'b0, the inf output is saturated to ±MAX, and the nan output is saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, nan is converted to zero; when CTRL\[50\] is set to 1'b1, nan remains unchanged. If the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id31 -->

## Example<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

<!-- npu="950,A3,910b" id32 -->
For a complete Fixpipe sample, see [fixpipe\_l0c2l1 sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2l1).

- Example: data path L0C Buffer -> L1 Buffer. The data type of input matrices A and B is half, and the output matrix C is int8_t. NZ2ND format conversion is disabled, and scalar quantization is enabled.

    ```cpp
        AscendC::LocalTensor<outputType> c1Local(AscendC::TPosition::C1, c1Addr, cSizeAlignL1);
    
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        uint16_t c0Size = 32;
        AscendC::FixpipeParamsV220 fixpipeParams;
        // In the NZ2NZ scenario, the dstStride unit of FixpipeParamsV220 is a datablock (32 bytes).
        fixpipeParams.dstStride = baseM * c0Size * sizeof(outputType) / AscendC::ONE_BLK_SIZE;
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        // scalar quant
        fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE;
        uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t *>(&quantScalar));
        constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
        deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
        fixpipeParams.deqScalar = deqScalar;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(c1Local, c, fixpipeParams);
    
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        uint16_t c0Size = 32;
        AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::NZ> fixpipeParams;
        // In the NZ2NZ scenario, the dstStride unit of FixpipeParamsArch3510 is the number of elements.
        fixpipeParams.dstStride = baseM * c0Size;
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        // scalar quant
        fixpipeParams.quantPre = QuantMode_t::QF322B8_PRE;
        uint64_t deqScalar = static_cast<uint64_t>(*reinterpret_cast<int32_t *>(&quantScalar));
        constexpr bool sign = (AscendC::IsSameType<outputType, int8_t>::value) ? true : false;
        deqScalar = (deqScalar & ~(static_cast<uint64_t>(1) << 46)) | (static_cast<uint64_t>(sign) << 46);
        fixpipeParams.deqScalar = deqScalar;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_NZ>(c1Local, c, fixpipeParams);
    #endif
    ```
<!-- end id32 -->
