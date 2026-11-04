# Fixpipe (L0C to GM Data Copy)<a id="ZH-CN_TOPIC_0000002569070977"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:32:47.817Z pushedAt=2026-10-26T07:41:44.120Z nl=2 -->

## Applicable Products<a id="zh-cn_topic_0000002542828493_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported for APIs that include the **FixpipeParamsV220**/**FixpipeParamsArch3510** parameters.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported only for APIs that include the **FixpipeParamsV220** parameter.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported only for APIs that include the **FixpipeParamsV220** parameter.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported only for APIs that include the **FixpipeParamsM300** parameter.
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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_store/Fixpipe_L0CToGM_res.md#id1 -->

## Description<a id="zh-cn_topic_0000002542828493_section106841136114319"></a>

The header file path is: `"basic_api/kernel_operator_fixpipe_intf.h, basic_api/kernel_operator_data_copy_intf.h"`.

The computation results of matrix operations are stored in the L0C Buffer. The Fixpipe API is used to copy the results to Global Memory (GM), and it supports operations such as in-line format conversion during the copy process.

<!-- npu="950,A3,910b" id10 -->

The following figure shows the valid combinations, intermediate data types, and data paths of on-the-fly quantization, in-line ReLU, in-line format conversion, in-line channel split, and in-line channel merge. In the figure, F32-\>F16 and F32-\>BF16 are non-quantization modes that perform only Cast, while the rest are in-line scalar/tensor quantization modes.

<!-- npu="A3,910b" id11 -->
**Figure 1** L0C2GM flowchart ([NPU architecture version 2201](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002542828493_fig542810249417"></a>  

![](../../../../figures/L0C2GM_Function_Combination.png)
<!-- end id11 -->

<!-- npu="950" id13 -->
**Figure 2** L0C2GM flowchart ([NPU architecture version 3510](../../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md))<a id="zh-cn_topic_0000002542828493_fig542810249613"></a>  

![](../../../../figures/L0C2GM_Function_Combination_950.png)
<!-- end id13 -->
<!-- end id10 -->

## Prototype<a id="zh-cn_topic_0000002542828493_section82039854412"></a>

For user convenience and to reduce learning and understanding costs, a combined matrix copy-out API **Fixpipe** is provided for frequently used scenario combinations. The API incorporates register configuration and data copy capabilities. The corresponding APIs are as follows:

**APIs that take the FixpipeParamsArch3510 parameter:**

- Path L0C Buffer (CO1)-\>GM, without enabling the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- Path L0C Buffer (CO1)->GM, with the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**APIs that take the FixpipeParamsV220 parameter:**

- Path L0C Buffer (CO1)-\>GM, without enabling the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- Path L0C Buffer (CO1)->GM, with the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

**APIs that take the FixpipeParamsM300 parameter:**

- Path L0C Buffer (CO1)-\>GM, without enabling the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300& intriParams)
    ```

- Path L0C Buffer (CO1)-\>GM, enable the on-the-fly [tensor quantization](../cube_store_key_features/accompanying_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsM300& intriParams)
    ```

## Parameters<a id="zh-cn_topic_0000002542828493_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Type](#zh-cn_topic_0000002542828493_section4219135304818). |
| config | Fixpipe-related configuration parameter, of type **FixpipeConfig**. The values are as follows:<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR (default value)**: Enables NZ2ND, and the output data format is ND.<br>&nbsp;&nbsp;&bull; CFG_NZ: NZ2NZ, and the output data format is NZ.<br><!-- npu="950" id14 -->&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR: For Ascend 950PR&950DT products, enables NZ2DN, and the output data format is DN.<br><!-- end id14 --><br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is in the Unified Buffer (UB). <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // Enable NZ2ND; the output data format is ND.<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // Supported only on Ascend 950PR&950DT products. Enable NZ2DN; the output data format is DN.<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // Supported only on Ascend 950PR&950DT products.<!-- end id16 --> |
| S | Data type of the **cbufWorkspace** parameter, that is, the data type of the on-the-fly quantization parameter.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use basic data types, template parameter **S** must be of type **uint64_t**; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use the **TensorTrait** type, the **LiteType** of template parameter **S** must be of type **uint64_t**; otherwise, compilation fails.<br>The template parameter following template parameter **S** is used only for the data type check described above and does not require user attention. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of type **GlobalTensor**. The data format is NZ or ND. The ND address must satisfy 1-byte alignment, and the NZ address must satisfy 32-byte alignment.<!-- npu="950" id17 --><br>For Ascend 950PR&950DT products, the DN data format is also supported, and the address must satisfy 32-byte alignment.<!-- end id17 --> |
| src | Input | Source operand, of type **LocalTensor**. The supported physical address is L0C Buffer (**TPosition** is CO1), which is the result computed by the Mmad API. The data format is NZ, and the address must satisfy 64-byte alignment. |
| intriParams | Input | Fixpipe copy parameter. For the specific definition, see "basic_api/kernel_struct_fixpipe.h".<br>For parameter description, see the Fixpipe copy parameter (FixpipeParamsArch3510, FixpipeParamsV220, FixpipeParamsM300) structure description. |
| cbufWorkspace | Input | Quantization parameter, of type `LocalTensor<uint64_t>`. The supported physical address is L1 Buffer (**TPosition** is C1), and the address must satisfy 32-byte alignment.<br>&nbsp;&nbsp;&bull; Supported when **quantPre** is VDEQF16, VQF322B8_PRE, or VREQ8.<br><!-- npu="950" id18 -->&nbsp;&nbsp;&bull; For Ascend 950PR&950DT products, in addition to the above, VQF322FP8_PRE, VQF322HIF8_PRE, VQF322HIF8_PRE_HYBRID, VQS322BF16_PRE, VQF322F16_PRE, VQF322BF16_PRE, and VQF322F32_PRE are also supported.<br><!-- end id18 -->For the introduction to **quantPre**, see the **quantPre** section in the Fixpipe copy parameter structure. |

**Table 3** Fixpipe copy parameter (FixpipeParamsArch3510) structure description

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory Input | Size of the source NZ matrix in the N direction. Value range: nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) feature is enabled, **nSize** must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit feature is not enabled, **nSize** must be a multiple of 16.<br>**Note: nSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory Input | Size of the source NZ matrix in the M direction. Value range: mSize∈[0, 65535].<br>**Note: mSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory Input | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: srcStride∈[0, 65535], in units of C0_Size (16*sizeof(T)), where T is the data type of **src**. Its value should be set to **mSize** rounded up to a multiple of 16. |
| dstStride | Mandatory Input | &nbsp;&nbsp;&bull; When NZ2ND/NZ2DN is not enabled ([NZ2NZ copy](../cube_store_key_features/NZ2NZ.md)): start address offset between adjacent Z layouts in the destination NZ matrix. The value must not be 0, in units of element. (Compared with **dstStride** in the Fixpipe copy parameter (FixpipeParamsV220), the unit has changed: in FixpipeParamsV220, the unit of **dstStride** is datablock (32 bytes), while in FixpipeParamsArch3510, the unit of **dstStride** is element.)<br>&nbsp;&nbsp;&bull; When [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) is enabled: number of elements in each row of the destination ND/DN matrix. The value must not be 0, in units of element. |
| [quantPre](../cube_store_key_features/accompanying_quantization.md) | Optional Input | Used to control the quantization mode, of type **QuantMode_t**. Default value: `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter must be configured manually by the user; the corresponding quantization mode is not automatically derived.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disable quantization.<br>    F322F16,               // Float32_2_Float16: cast float to half, with CAST_RINT cast mode.<br>    F322BF16,              // Float32_2_BFloat16: cast float to bfloat16_t, with CAST_RINT cast mode.<br>    DEQF16,                // DeQuant_Float16: quantize int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: quantize int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: quantize float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: quantize float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: quantize float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: quantize float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: quantize int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: quantize int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: quantize float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: quantize float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: quantize float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: quantize float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: quantize float to float, scalar quantization. The precision can reach dual-0.1% but cannot reach one dual-0.01%. If dual-0.01% precision is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: quantize float to float, tensor quantization. The precision can reach dual-0.1% but cannot reach one dual-0.01%. If dual-0.01% precision is required, use the [AscendDeQuant](../../../adv_api/quantization/AscendDequant.md) high-level API.<br>}; |
| deqScalar | Optional Input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the **quantPre** quantization mode is [on-the-fly quantization](../cube_store_key_features/accompanying_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/accompanying_relu.md) | Optional Input | Whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional Input | unitFlag is a form of fine-grained parallelism between Mmad and Fixpipe instructions. After this feature is enabled, the hardware moves out the computation result after computing each fractal. Value description:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled, and after the hardware executes an instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled, and after the hardware executes an instruction, the unit flag bit is reset.<br>When this feature is enabled, set the **unitFlag** value of both the Mmad and Fixpipe instructions to 2 or 3.<br>For parameter configuration and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| params | Optional Input | Used to select and configure different on-the-fly format conversions (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of type **TransformParams**. The **TransformParams** structure is a type selector based on template parameters, used to automatically select the corresponding parameter type at compile time according to the template parameters used when defining the FixpipeParamsArch3510 copy parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the CO2Layout layout type is ROW_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination with NZ2ND conversion. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: number of source NZ matrices, that is, the number of ND matrices to be transferred. Value range: ndNum∈[0, 65535]. **Note: ndNum=0 means no copy is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: interval between the start addresses of different NZ matrices. Value range: srcNdStride∈[0, 65535], in units of C0_SIZE. When **ndNum** is set to 1, **srcNdStride** can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: offset between the start addresses of adjacent destination ND matrices. Value range: dstNdStride∈[1, 2^32 -1], in units of element. When **ndNum** is set to 1, **dstNdStride** can be set to 0 and does not take effect.<br><br>When the CO2Layout layout type is COLUMN_MAJOR, this instruction is defined as data movement from L0C Buffer to the destination with NZ2DN conversion. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzMatrixStride = 0;<br>    uint32_t dstDnMatrixStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: number of DN matrices to be transferred. Value range: dnNum∈[0, 65535]. **Note: dnNum=0 means no copy is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzMatrixStride: offset between different source NZ matrices (head to head). Value range: srcNzMatrixStride∈[0, 65535], in units of C0_SIZE. When **dnNum** is set to 1, **srcNzMatrixStride** can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstDnMatrixStride: offset between the start addresses of adjacent destination ND matrices. Value range: dstDnMatrixStride∈[0, 2^32 -1], in units of element. When **dnNum** is set to 1, **dstDnMatrixStride** can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: address offset between adjacent rows in the source NZ fractal (head to head). Value range: srcNzC0Stride∈[0, 65535], in units of C0_SIZE. When NZ2DN is enabled, **srcNzC0Stride** cannot be 0.<br><br>When the CO2Layout layout type is NZ, it is the normal copy DMA mode, indicating normal data movement from L0C Buffer to the destination. |
| dualDstCtrl | Optional Input | This parameter is valid only on the L0C Buffer-to-UB path. |
| subBlockId | Optional Input | This parameter is valid only on the L0C Buffer-to-UB path. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional Input | Whether to enable channel split. The default value is false, meaning the feature is not enabled. Channel split can be enabled only when both **src** and **dst** are float, and ChannelSplit and NZ2ND/NZ2DN cannot be enabled at the same time. |

**Table 4** Fixpipe copy parameter (FixpipeParamsV220) structure description

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory Input | Size of the source NZ matrix in the N direction. Value range: nSize∈[0, 4095]. **nSize** must be a multiple of 16.<br>For destination matrix NZ output: when the output type is float, if the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) feature is enabled, **nSize** must be a multiple of 8.<br>**Note: nSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory Input | Size of the source NZ matrix in the M direction.<br>&nbsp;&nbsp;&bull; When the on-the-fly NZ2ND feature is not enabled ([NZ2NZ copy](../cube_store_key_features/NZ2NZ.md)), the value range is mSize∈[0, 65535].<br>&nbsp;&nbsp;&bull; When the on-the-fly [NZ2ND](../cube_store_key_features/NZ2ND.md) feature is enabled, the value range is mSize∈[0, 8192].<br>**Note: mSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory Input | Start address offset between adjacent Z layouts in the source NZ matrix. Value range: srcStride∈[0, 65535], in units of C0_Size (16*sizeof(T)), where T is the data type of **src**. Its value should be set to **mSize** rounded up to a multiple of 16. |
| dstStride | Mandatory Input | &nbsp;&nbsp;&bull; When the NZ2ND feature is not enabled ([NZ2NZ copy](../cube_store_key_features/NZ2NZ.md)): start address offset between adjacent Z layouts in the destination NZ matrix. The value must not be 0, in units of datablock (32 bytes).<br>&nbsp;&nbsp;&bull; When the on-the-fly [NZ2ND](../cube_store_key_features/NZ2ND.md) feature is enabled: number of elements in each row of the destination ND matrix. The value must not be 0, in units of element. |
| quantPre | Optional Input | Used to control the quantization mode, of type **QuantMode_t**. Default value: `QuantMode_t::NoQuant`. The specific definition is as follows:<br>&nbsp;&nbsp;&bull; For float/int32_t output, this parameter must be set to `QuantMode_t::NoQuant`.<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, this parameter must be set to `QuantMode_t::F322F16`/`QuantMode_t::F322BF16`.<br>Note: This parameter must be configured manually by the user; the corresponding quantization mode is not automatically derived.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Disable quantization.<br>    F322F16,      // Float32_2_Float16: cast float to half, with CAST_RINT cast mode.<br>    F322BF16,     // Float32_2_BFloat16: cast float to bfloat16_t, with CAST_RINT cast mode.<br>    DEQF16,       // DeQuant_Float16: quantize int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: quantize int32_t to int8_t/uint8_t, tensor quantization.<br>}; |
| deqScalar | Optional Input | Scalar quantization parameter, indicating a single scale value. This parameter must be set when the **quantPre** quantization mode is [on-the-fly quantization](../cube_store_key_features/accompanying_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/accompanying_relu.md) | Optional Input | Whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional Input | **unitFlag** is a form of instruction-level parallelism between Mmad and Fixpipe instructions. After this feature is enabled, the hardware moves out the computation result after computing each fractal. Value description:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled, and after the hardware executes an instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled, and after the hardware executes an instruction, the unit flag bit is reset.<br>When this feature is enabled, set the **unitFlag** value of both the Mmad and Fixpipe instructions to 2 or 3.<br>For parameter configuration and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| [channelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional Input | Takes effect only for L0C Buffer(CO1) -> GM NZ float output. Whether to enable channel split. The default value is `false`, meaning the feature is not enabled. |
| ndNum | Optional Input | Number of source NZ matrices on L0C Buffer, that is, the number of ND matrices to be transferred. The default value is 1. Value range: ndNum∈[0, 65535]. The number of multiple NZ non-contiguous matrices for [NZ2ND](../cube_store_key_features/NZ2ND.md).<br>**Note: ndNum=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcNdStride | Optional Input | Interval between the start addresses of different NZ matrices. Value range: srcNdStride∈[0, 512], in units of data block 16*C0_Size. When **ndNum** is set to 1, **srcNdStride** can be set to 0 and does not take effect. |
| dstNdStride | Optional Input | Offset between the start addresses of adjacent destination ND matrices. Value range: dstNdStride∈[0, 65535], in units of element. When **ndNum** is set to 1, **dstNdStride** can be set to 0 and does not take effect. |

**Table 5** Fixpipe copy parameter (FixpipeParamsM300) structure description

| Parameter Name | Data Type | Description |
| ---------- | ---------- | ---------- |
| nSize | Input | Size of the source NZ matrix in the N direction, with a value range of nSize ∈ [0, 4095].<br>&nbsp;&nbsp;&bull;If channelSplit is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull;If channelSplit is not enabled, nSize must be a multiple of 16.<br>**Note: nSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| mSize | Input | Size of the source NZ matrix in the M direction.<br>&nbsp;&nbsp;&bull;When NZ2ND is not enabled, the value range is mSize ∈ [0, 65535].<br>&nbsp;&nbsp;&bull;When NZ2ND is enabled, the value range is mSize ∈ [0, 8192].<br>**Note: mSize=0 means no copy is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Input | Start address offset between adjacent Z layouts in the source NZ matrix, with a value range of srcStride ∈ [0, 65535], in units of C0_Size(16\*sizeof(T), where T is the data type of src). |
| dstStride | Input | &nbsp;&nbsp;&bull; When NZ2ND is not enabled, the start address offset between adjacent Z layouts in the destination NZ matrix, with a non-zero value, in units of datablock (32 bytes).<br>&nbsp;&nbsp;&bull; When NZ2ND is enabled, the number of elements in each row of the destination ND matrix, with a non-zero value, in units of element. |
| quantPre | Input | QuantMode_t is an enumeration type used to control the quantization mode. The default value is QuantMode_t::NoQuant, which means quantization is not enabled. The values of QuantMode_t are as follows:<br>&nbsp;&nbsp;&bull; NoQuant: quantization is not enabled;<br>&nbsp;&nbsp;&bull; F322F16: float cast to half, with the cast mode being CAST_RINT;<br>&nbsp;&nbsp;&bull; F322BF16: float cast to bfloat16_t, with the cast mode being CAST_RINT;<br>&nbsp;&nbsp;&bull; DEQF16: int32_t quantized to half, scalar quantization;<br>&nbsp;&nbsp;&bull; VDEQF16: int32_t quantized to half, tensor quantization;<br>&nbsp;&nbsp;&bull; QF322B8_PRE: float quantized to uint8_t/int8_t, scalar quantization;<br>&nbsp;&nbsp;&bull; VQF322B8_PRE: float quantized to uint8_t/int8_t, tensor quantization;<br>&nbsp;&nbsp;&bull; REQ8: int32_t quantized to uint8_t/int8_t, scalar quantization;<br>&nbsp;&nbsp;&bull; VREQ8: int32_t quantized to uint8_t/int8_t, tensor quantization. |
| deqScalar | Input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the quantPre quantization mode is scalar quantization. The supported data type is uint64_t. |
| ndNum | Input | Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum ∈ [1, 65535]. |
| srcNdStride | Input | Interval between the start addresses of different NZ matrices. When ndNum is greater than 1, the value range is srcNdStride ∈ [1, 512]; when ndNum is configured as 1, srcNdStride can be configured as 0, in which case it does not take effect. The unit is data block (16 \* C0_Size). |
| dstNdStride | Input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride ∈ [1, 65535], in units of element. When ndNum is configured as 1, dstNdStride can be configured as 0, in which case it does not take effect. |
| reluEn | Input | Whether to enable ReLU:<br>&nbsp;&nbsp;&bull; false: ReLU is not enabled;<br>&nbsp;&nbsp;&bull; true: ReLU is enabled. |
| unitFlag | Input | unitFlag is a type of instruction-level parallelism between Mmad and Fixpipe instructions. After this feature is enabled, the hardware moves out the computation result after computing each fractal. This feature is not applicable to scenarios where accumulation is performed in the L0C Buffer. Value description:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled;<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled, and after the hardware executes an instruction, the register is not set;<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled, and after the hardware executes an instruction, unitFlag is turned off. |
| isChannelSplit | Input | Whether to enable channel split. The default value is false, which means the feature is not enabled. Channel split can be enabled only when both src and dst are float, and ChannelSplit and NZ2ND cannot be enabled at the same time. |

## Data Type<a id="zh-cn_topic_0000002542828493_section4219135304818"></a>

**Supported Data Type Combinations for Source and Destination Matrices**

<!-- npu="950" id19 -->
For the Ascend 950PR&950DT products, the Fixpipe API supports the following data type combinations:

| Source Matrix (L0C Buffer) | Destination Matrix (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |

<!-- end id19 -->

<!-- npu="A3" id20 -->
For the Atlas A3 products, the Fixpipe API supports the following data type combinations:

| Source Matrix (L0C Buffer) | Destination Matrix (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

<!-- end id20 -->

<!-- npu="910b" id21 -->
For the Atlas A2 products, the Fixpipe API supports the following data type combinations:

| Source Matrix (L0C Buffer) | Destination Matrix (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

<!-- end id21 -->

<!-- npu="310b" id22 -->
For the Atlas 200I/500 A2 inference products, the Fixpipe API supports the following data type combinations:

| Source Matrix (L0C Buffer) | Destination Matrix (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |

<!-- end id22 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002542828493_section2045914466492"></a>

- The description of the float32 data type for quantization input is as follows:
    - The standard IEEE 754 float32 format is: 1-bit sign bit, 8-bit exponent bits, and 23-bit mantissa bits. The float32 format supported by the current AI processor is: 1-bit sign bit, 8-bit exponent bits, and 10-bit mantissa bits.
    - If the user provides standard IEEE 754 float32 input, the API internally converts it to the float32 format supported by the processor for computation. In this case, if standard IEEE 754 float32 data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source matrix NZ format address requires 64-byte alignment, the destination matrix ND format address requires 1-byte alignment, and the NZ format address requires 32-byte alignment.
- When any of the copied-out **mSize**, **nSize**, or **ndNum** is 0, the instruction is not executed. A warning is reported when **ndNum**=0.
- The **cbufWorkspace** quantized tensor is located in the L1 Buffer, and its address requires 32-byte alignment.
- The quantization and ReLU parameters cannot be inf/nan or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware does not report any warning or error, nor does it guarantee the write order of the overlapping data.
- Enabling the **unitFlag** feature requires **Mmad** to be enabled at the same time.
<!-- npu="950" id28 -->
- When NZ2DN is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
<!-- end id28 -->
<!-- npu="A3,910b" id27 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id25 -->
    Atlas A3 products
    <!-- end id25 -->
    <!-- npu="910b" id26 -->
    Atlas A2 products
    <!-- end id26 -->
    For floating-point inf/nan input and output, the CTRL\[48\] bit of the **CTRL** register (control register) can be set to control the saturation mode when floating-point values are quantized and copied out:

    - Non-saturation mode: Set CTRL\[48\] to 1'b1, and inf/nan are output as-is.
    - Saturation mode: Set CTRL\[48\] to 1'b0, and inf output is saturated to ±MAX, while nan output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id27 -->
<!-- npu="950" id29 -->
- For the Ascend 950PR&950DT products, the special value/boundary value constraints are described as follows:

    For floating-point inf/nan input and output, the CTRL\[48\] bit of the CTRL register (control register) can be set to control the saturation mode when quantized floating-point data is moved out;

    For the fp8\_e4m3fn\_t type, the CTRL\[48\] bit of the CTRL register (control register) can be used for finer-grained settings;

    - Non-saturation mode: When CTRL\[48\] is set to 1'b1, inf/nan is output as is. For the fp8\_e4m3fn\_t type, if the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the output result is nan.
    - Saturation mode: When CTRL\[48\] is set to 1'b0, inf output is saturated to ±MAX, and nan output is saturated to 0. For the fp8\_e4m3fn\_t type, when CTRL\[50\] is set to 1'b0, nan is converted to zero; when CTRL\[50\] is set to 1'b1, nan remains unchanged. If the absolute value of the result is inf or greater than the maximum normalized value of fp8\_e4m3fn\_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
    For a summary of the boundary values of the L0C Buffer-to-GM copy API, see [Summary of Boundary Values of Matrix Computation Move-Out Related APIs](../../../../appendix/matrix_compute_move_out_interface_boundary_value_summary.md).
<!-- end id29 -->

## Example<a id="zh-cn_topic_0000002542828493_section088124295117"></a>

<!-- npu="950,A3,910b" id30 -->
For the complete Fixpipe sample, see [fixpipe\_l0c2gm sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2gm).

- Example: path L0C Buffer-\>GM. The input matrices A and B use the half data type, the output matrix C uses float, and NZ2ND format conversion is enabled.

    ```cpp
        AscendC::GlobalTensor<outputType> cGM;
        cGM.SetGlobalBuffer((__gm__ outputType *)c);
    
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        AscendC::FixpipeParamsV220 fixpipeParams;
        fixpipeParams.ndNum = 1;
        fixpipeParams.srcNdStride = 0;
        fixpipeParams.dstNdStride = 0;
    #elif defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
        AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
    #endif
        fixpipeParams.mSize = baseM;
        fixpipeParams.srcStride = CeilAlign(baseM, CUBE_BLOCK);
        fixpipeParams.nSize = baseN;
        fixpipeParams.dstStride = baseN;
        AscendC::Fixpipe<outputType, l0cType, AscendC::CFG_ROW_MAJOR>(cGM, c, fixpipeParams);
    ```
<!-- end id30 -->
