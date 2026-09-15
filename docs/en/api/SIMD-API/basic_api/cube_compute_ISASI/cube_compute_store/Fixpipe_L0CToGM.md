# Fixpipe (L0C to GM Data Move-out)<a id="ZH-CN_TOPIC_0000002569070977"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T15:46:44.192Z -->

## Applicable Products<a id="zh-cn_topic_0000002542828493_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supports the APIs that contain the FixpipeParamsV220/FixpipeParamsArch3510 parameters.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supports only the APIs that contain the FixpipeParamsV220 parameter.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supports only the APIs that contain the FixpipeParamsV220 parameter.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Supports only the APIs that contain the FixpipeParamsM300 parameter.
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a id="zh-cn_topic_0000002542828493_section106841136114319"></a>

The header file paths are `"basic_api/kernel_operator_fixpipe_intf.h, basic_api/kernel_operator_data_copy_intf.h"`.

The results of cube computation are stored in the L0C Buffer. The Fixpipe API is used to move the results to Global Memory (GM), and it supports operations such as inline format conversion during the move.

<!-- npu="950,A3,910b" id10 -->

The following figure shows the valid combinations, intermediate data types, and data paths of inline quantization, inline ReLU, inline format conversion, inline channel split, and inline channel merge. In the figure, F32-\>F16 and F32-\>BF16 are non-quantization modes that perform only Cast, while the rest are inline scalar/tensor quantization modes.

<!-- npu="A3,910b" id11 -->
**Figure 1** L0C2GM flowchart ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002542828493_fig542810249417"></a>  

![](../../../../figures/L0C2GM_Function_Combination.png)
<!-- end id11 -->

<!-- npu="950" id13 -->
**Figure 2** L0C2GM flowchart ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002542828493_fig542810249613"></a>  

![](../../../../figures/L0C2GM_Function_Combination_950.png)
<!-- end id13 -->
<!-- end id10 -->

## Prototype<a id="zh-cn_topic_0000002542828493_section82039854412"></a>

To facilitate user operations and reduce the learning cost, a combined cube move-out API **Fixpipe** is provided for frequently used scenario combinations. The API integrates register configuration and data movement capabilities. The corresponding APIs are as follows:

**APIs that take FixpipeParamsArch3510 parameters:**

- Path L0C Buffer (CO1)-\>GM, without enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- Path L0C Buffer (CO1)-\>GM, with the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**APIs that take FixpipeParamsV220 parameters:**

- Path L0C Buffer (CO1)-\>GM, without enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- Path L0C Buffer (CO1)-\>GM, with the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

**APIs that take FixpipeParamsM300 parameters:**

- Path L0C Buffer (CO1)-\>GM, without enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsM300& intriParams)
    ```

- Path L0C Buffer (CO1) → GM, enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) function:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const GlobalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsM300& intriParams)
    ```

## Parameters<a id="zh-cn_topic_0000002542828493_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Type](#zh-cn_topic_0000002542828493_section4219135304818). |
| config | Fixpipe-related configuration parameter, of the **FixpipeConfig** type. The values are as follows:<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR (default value)**: Enable NZ2ND; the output data format is ND.<br>&nbsp;&nbsp;&bull; CFG_NZ: NZ2NZ; the output data format is NZ.<br><!-- npu="950" id14 -->&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR: For Ascend 950PR/Ascend 950DT, enable NZ2DN; the output data format is DN.<br><!-- end id14 --><br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is in UB. <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // Enable NZ2ND; the output data format is ND.<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // Supported only on Ascend 950PR/Ascend 950DT. Enable NZ2DN; the output data format is DN.<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // Supported only on Ascend 950PR/Ascend 950DT.<!-- end id16 --><br>|
| S | Data type of the **cbufWorkspace** parameter, that is, the data type of the inline quantization parameter.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use basic data types, the template parameter **S** must be of the **uint64_t** type; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use the **TensorTrait** type, the **LiteType** of the template parameter **S** must be of the **uint64_t** type; otherwise, compilation fails.<br>The template parameter following the template parameter **S** is used only for the preceding data type check and does not require user attention. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **GlobalTensor** type. The data format is NZ or ND. The ND address must satisfy 1-byte alignment, and the NZ address must satisfy 32-byte alignment.<!-- npu="950" id17 --><br>For Ascend 950PR/Ascend 950DT, the DN data format is also supported, and the address must satisfy 32-byte alignment.<!-- end id17 --> |
| src | Input | Source operand, of the **LocalTensor** type. The supported physical address is L0C Buffer (**TPosition** is **CO1**), which is the result computed by the **Mmad** API. The data format is NZ, and the address must satisfy 64-byte alignment. |
| intriParams | Input | Fixpipe move parameter. For the specific definition, see "basic_api/API/kernel_struct_fixpipe.h".<br>For the parameter description, see the Fixpipe move parameter (FixpipeParamsArch3510, FixpipeParamsV220, FixpipeParamsM300) structure description. |
| cbufWorkspace | Input | Quantization parameter, of the `LocalTensor<uint64_t>` type. The supported physical address is L1 Buffer (**TPosition** is **C1**), and the address must satisfy 32-byte alignment.<br>&nbsp;&nbsp;&bull; Supported when **quantPre** is **VDEQF16**, **VQF322B8_PRE**, or **VREQ8**.<br><!-- npu="950" id18 -->&nbsp;&nbsp;&bull; For Ascend 950PR/Ascend 950DT, in addition to the preceding modes, **VQF322FP8_PRE**, **VQF322HIF8_PRE**, **VQF322HIF8_PRE_HYBRID**, **VQS322BF16_PRE**, **VQF322F16_PRE**, **VQF322BF16_PRE**, and **VQF322F32_PRE** are also supported.<br><!-- end id18 -->For the introduction of **quantPre**, see the **quantPre** section in the Fixpipe move parameter structure. |

**Table 3** Fixpipe move parameter (FixpipeParamsArch3510) structure description

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ cube in the N direction, with a value range of nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) feature is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit feature is not enabled, nSize must be a multiple of 16.<br>**Note: nSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ cube in the M direction, with a value range of mSize∈[0, 65535].<br>**Note: mSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Start address offset between adjacent Z layouts in the source NZ cube, with a value range of srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &nbsp;&nbsp;&bull; When the NZ2ND/NZ2DN feature is not enabled ([NZ2NZ move](../cube_store_key_features/NZ2NZ.md)): start address offset between adjacent Z layouts in the destination NZ cube, with a non-zero value, in the unit of element. (Compared with dstStride in the Fixpipe move parameter (FixpipeParamsV220), the unit differs: in FixpipeParamsV220, the unit of dstStride is datablock (32 bytes), whereas in FixpipeParamsArch3510, the unit of dstStride is element.)<br>&nbsp;&nbsp;&bull; When the [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) feature is enabled: number of elements in each row of the destination ND/DN cube, with a non-zero value, in the unit of element. |
| [quantPre](../cube_store_key_features/on_the_fly_quantization.md) | Optional input | Controls the quantization mode, of the **QuantMode_t** type, with a default value of `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter must be configured manually by the user; the corresponding quantization mode is not automatically inferred.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disable quantization.<br>    F322F16,               // Float32_2_Float16: Cast float to half, with CAST_RINT cast mode.<br>    F322BF16,              // Float32_2_BFloat16: Cast float to bfloat16_t, with CAST_RINT cast mode.<br>    DEQF16,                // DeQuant_Float16: Quantize int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: Quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: Quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: Quantize int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: Quantize float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: Quantize float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: Quantize float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: Quantize float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: Quantize int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: Quantize int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: Quantize float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: Quantize float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: Quantize float to float, scalar quantization. The precision can reach two thousandths but cannot reach two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: Quantize float to float, tensor quantization. The precision can reach two thousandths but cannot reach two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>};<br>|
| deqScalar | Optional input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the quantPre quantization mode is [inline quantization](../cube_store_key_features/on_the_fly_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/on_the_fly_relu.md) | Optional input | Switch for whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: Disable NormReLU. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: Enable NormReLU. |
| unitFlag | Optional input | unitFlag is a form of instruction-level parallelism between Mmad instructions and Fixpipe instructions. When this feature is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): Disable unitFlag.<br>&nbsp;&nbsp;&bull; 2 (2'b10): Enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): Enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this feature is enabled, the unitFlag value of both the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter configuration scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| params | Optional input | Used to select and configure different inline format conversions (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of the **TransformParams** type. The **TransformParams** structure is a template-parameter-based type selector that automatically selects the corresponding parameter type at compile time based on the template parameter used when defining the FixpipeParamsArch3510 move parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the CO2Layout layout type is ROW_MAJOR, this instruction is defined as a data move from L0C Buffer to the destination location with an NZ2ND conversion. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum∈[0, 65535]. **Note: ndNum=0 means no move is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: Interval between the start addresses of different NZ matrices, with a value range of srcNdStride∈[0, 65535], in the unit of C0_SIZE. When ndNum is set to 1, srcNdStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[1, 2^32 -1], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0 and does not take effect.<br><br>When the CO2Layout layout type is COLUMN_MAJOR, this instruction is defined as a data move from L0C Buffer to the destination location with an NZ2DN conversion. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzCubeStride = 0;<br>    uint32_t dstDnCubeStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: Number of DN matrices to be transferred, with a value range of dnNum∈[0, 65535]. **Note: dnNum=0 means no move is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzCubeStride: Offset between different source NZ matrices (head to head), with a value range of srcNzCubeStride∈[0, 65535], in the unit of C0_SIZE. When dnNum is set to 1, srcNzCubeStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstDnCubeStride: Offset between the start addresses of adjacent destination ND matrices, with a value range of dstDnCubeStride∈[0, 2^32 -1], in the unit of element. When dnNum is set to 1, dstDnCubeStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: Address offset between adjacent rows in the source cube NZ fractal (head to head), with a value range of srcNzC0Stride∈[0, 65535], in the unit of C0_SIZE. When NZ2DN is enabled, srcNzC0Stride cannot be 0.<br><br>When the CO2Layout layout type is NZ, this is the normal DMA move mode, representing a normal data move from L0C Buffer to the destination location. |
| dualDstCtrl | Optional input | This parameter takes effect only on the L0C Buffer-to-Unified Buffer path. |
| subBlockId | Optional input | This parameter takes effect only on the L0C Buffer-to-Unified Buffer path. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | Whether to enable channel split. The default value is false, meaning the feature is disabled. Channel split can be enabled only when both src and dst are float, and ChannelSplit and NZ2ND/NZ2DN cannot be enabled at the same time. |

**Table 4** Fixpipe move parameter (FixpipeParamsV220) structure description

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ cube in the N direction. The value range is nSize∈[0, 4095], and nSize must be a multiple of 16.<br>For NZ output of the destination cube: when the output type is float, if the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) feature is enabled, nSize must be a multiple of 8.<br>**Note: nSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ cube in the M direction.<br>&nbsp;&nbsp;&bull; When the inline NZ2ND feature is not enabled ([NZ2NZ move](../cube_store_key_features/NZ2NZ.md)), the value range is mSize∈[0, 65535].<br>&nbsp;&nbsp;&bull; When the inline [NZ2ND](../cube_store_key_features/NZ2ND.md) feature is enabled, the value range is mSize∈[0, 8192].<br>**Note: mSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Start address offset between adjacent Z layouts in the source NZ cube, with a value range of srcStride∈[0, 65535], in the unit of C0_Size (16*sizeof(T)), where T is the data type of src. Its value should be mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &nbsp;&nbsp;&bull; When the NZ2ND feature is not enabled ([NZ2NZ move](../cube_store_key_features/NZ2NZ.md)): start address offset between adjacent Z layouts in the destination NZ cube, with a non-zero value, in the unit of datablock (32 bytes).<br>&nbsp;&nbsp;&bull; When the inline [NZ2ND](../cube_store_key_features/NZ2ND.md) feature is enabled: number of elements in each row of the destination ND cube, with a non-zero value, in the unit of element. |
| [quantPre](../cube_store_key_features/on_the_fly_quantization.md) | Optional input | Controls the quantization mode, of the **QuantMode_t** type, with a default value of `QuantMode_t::NoQuant`. The specific definition is as follows:<br>&nbsp;&nbsp;&bull; For float/int32_t output, this parameter must be set to `QuantMode_t::NoQuant`.<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, this parameter must be set to `QuantMode_t::F322F16`/`QuantMode_t::F322BF16`.<br>Note: This parameter must be configured manually by the user; the corresponding quantization mode is not automatically inferred.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Disable quantization.<br>    F322F16,      // Float32_2_Float16: Cast float to half, with CAST_RINT cast mode.<br>    F322BF16,     // Float32_2_BFloat16: Cast float to bfloat16_t, with CAST_RINT cast mode.<br>    DEQF16,       // DeQuant_Float16: Quantize int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: Quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: Quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: Quantize int32_t to int8_t/uint8_t, tensor quantization.<br>};<br>|
| deqScalar | Optional input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the quantPre quantization mode is [inline quantization](../cube_store_key_features/on_the_fly_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/on_the_fly_relu.md) | Optional input | Switch for whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: Disable NormReLU. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: Enable NormReLU. |
| unitFlag | Optional input | unitFlag is a form of instruction-level parallelism between Mmad instructions and Fixpipe instructions. When this feature is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): Disable unitFlag.<br>&nbsp;&nbsp;&bull; 2 (2'b10): Enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): Enable unitFlag. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this feature is enabled, the unitFlag value of both the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter configuration scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| [channelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | Takes effect only for L0C Buffer (CO1) -> GM NZ float output. Whether to enable channel split. The default value is `false`, meaning the feature is disabled. |
| ndNum | Optional input | Number of source NZ matrices on L0C Buffer, that is, the number of ND matrices to be transferred. The default value is 1, and the value range is ndNum∈[0, 65535]. It is the number of multiple non-contiguous NZ matrices for [NZ2ND](../cube_store_key_features/NZ2ND.md).<br>**Note: ndNum=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| srcNdStride | Optional input | Interval between the start addresses of different NZ matrices, with a value range of srcNdStride∈[0, 512], in the unit of data block 16*C0_Size. When ndNum is set to 1, srcNdStride can be set to 0 and does not take effect. |
| dstNdStride | Optional input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[0, 65535], in the unit of element. When ndNum is set to 1, dstNdStride can be set to 0 and does not take effect. |

**Table 5** Fixpipe move parameter (FixpipeParamsM300) structure description

| Parameter | Data Type | Description |
| ---------- | ---------- | ---------- |
| nSize | Input | Size of the source NZ cube in the N direction, with a value range of nSize ∈ [0, 4095].<br>&nbsp;&nbsp;&bull;If the channelSplit function is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull;If the channelSplit function is not enabled, nSize must be a multiple of 16.<br>**Note: nSize = 0 indicates that no move is performed, and this API is treated as a NOP (no operation).** |
| mSize | Input | Size of the source NZ cube in the M direction.<br>&nbsp;&nbsp;&bull;When the NZ2ND function is not enabled, the value range is mSize ∈ [0, 65535].<br>&nbsp;&nbsp;&bull;When the NZ2ND function is enabled, the value range is mSize ∈ [0, 8192].<br>**Note: mSize = 0 indicates that no move is performed, and this API is treated as a NOP (no operation).** |
| srcStride | Input | Offset of the start address of adjacent Z layouts in the source NZ cube, with a value range of srcStride ∈ [0, 65535], in units of C0_Size (16\*sizeof(T), where T is the data type of src). |
| dstStride | Input | &bull;When the NZ2ND function is not enabled, the offset of the start address of adjacent Z layouts in the destination NZ cube, with a non-zero value, in units of datablock (32 bytes).<br>&bull;When the NZ2ND function is enabled, the number of elements in each row of the destination ND cube, with a non-zero value, in units of element. |
| quantPre | Input | QuantMode_t is an enumeration type used to control the quantization mode. The default value is QuantMode_t::NoQuant, which means the quantization function is not enabled. The values of QuantMode_t are as follows:<br>&nbsp;&nbsp;&bull; NoQuant: The quantization function is not enabled.<br>&nbsp;&nbsp;&bull; F322F16: Casts float to half, with the cast mode being CAST_RINT.<br>&nbsp;&nbsp;&bull; F322BF16: Casts float to bfloat16_t, with the cast mode being CAST_RINT.<br>&nbsp;&nbsp;&bull; DEQF16: Quantizes int32_t to half, scalar quantization.<br>&nbsp;&nbsp;&bull; VDEQF16: Quantizes int32_t to half, tensor quantization.<br>&nbsp;&nbsp;&bull; QF322B8_PRE: Quantizes float to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; VQF322B8_PRE: Quantizes float to uint8_t/int8_t, tensor quantization.<br>&nbsp;&nbsp;&bull; REQ8: Quantizes int32_t to uint8_t/int8_t, scalar quantization.<br>&nbsp;&nbsp;&bull; VREQ8: Quantizes int32_t to uint8_t/int8_t, tensor quantization. |
| deqScalar | Input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the quantPre quantization mode is scalar quantization. The supported data type is uint64_t. |
| ndNum | Input | Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum ∈ [1, 65535]. |
| srcNdStride | Input | Interval between the start addresses of different NZ matrices. When ndNum is greater than 1, the value range is srcNdStride ∈ [1, 512]. When ndNum is set to 1, srcNdStride can be set to 0, in which case it does not take effect. The unit is data block (16 \* C0_Size). |
| dstNdStride | Input | Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride ∈ [1, 65535], in units of element. When ndNum is set to 1, dstNdStride can be set to 0, in which case it does not take effect. |
| reluEn | Input | Switch for enabling ReLU:<br>&nbsp;&nbsp;&bull; false: The ReLU function is not enabled.<br>&nbsp;&nbsp;&bull; true: The ReLU function is enabled. |
| unitFlag | Input | unitFlag is an instruction-level parallelism between Mmad instructions and Fixpipe instructions. When this function is enabled, the hardware moves out the computation result each time it finishes computing one fractal. This function is not applicable to scenarios where accumulation is performed in the L0C Buffer. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled, and the hardware does not set the register after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled, and the hardware disables unitFlag after executing the instruction. |
| isChannelSplit | Input | Whether to enable the channel split function. The default value is false, which means the function is not enabled. Channel split can be enabled only when both src and dst are float, and ChannelSplit and NZ2ND cannot be enabled at the same time. |

## Data Type<a id="zh-cn_topic_0000002542828493_section4219135304818"></a>

**Supported data type combinations for the source cube and destination cube**

<!-- npu="950" id19 -->
For Ascend 950PR/Ascend 950DT, the Fixpipe API supports the following data type combinations:

| Source cube (L0C Buffer) | Destination cube (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |
<!-- end id19 -->

<!-- npu="A3" id20 -->
For Atlas A3 training products/Atlas A3 inference products, the Fixpipe API supports the following data type combinations:

| Source cube (L0C Buffer) | Destination cube (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |
<!-- end id20 -->

<!-- npu="910b" id21 -->
For Atlas A2 training products/Atlas A2 inference products, the Fixpipe API supports the following data type combinations:

| Source cube (L0C Buffer) | Destination cube (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |
<!-- end id21 -->

<!-- npu="310b" id22 -->
For Atlas 200I/500 A2 inference products, the Fixpipe API supports the following data type combinations:

| Source cube (L0C Buffer) | Destination cube (GM) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, int32_t. |
<!-- end id22 -->

## Return Value Description

None

## Constraints<a id="zh-cn_topic_0000002542828493_section2045914466492"></a>

- For quantization input of the float32 data type, the following applies:
    - The standard IEEE 754 float32 format consists of a 1-bit sign bit, 8-bit exponent bits, and 23-bit mantissa bits. The float32 format supported by the current AI processor consists of a 1-bit sign bit, 8-bit exponent bits, and 10-bit mantissa bits.
    - If the user provides standard IEEE 754 float32 input, the API internally converts it to the processor-supported float32 format for computation. In this case, if standard IEEE 754 float32 data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source cube NZ-format address must be 64-byte aligned. The destination cube ND-format address must satisfy 1-byte alignment, and the NZ-format address must satisfy 32-byte alignment.
- When any of the moved **mSize**, **nSize**, or **ndNum** is 0, the instruction is not executed. When **ndNum** is 0, a warning is reported.
- The **cbufWorkspace** quantization tensor resides in the L1 Buffer, and its address must be 32-byte aligned.
- The quantization and ReLU parameters cannot be Inf/NaN or denormalized numbers.
- The destination data must not overlap. If overlapping writes occur to the destination address, the hardware reports no warning or error, and the write order of overlapping data is not guaranteed.
- Enabling the **unitFlag** feature must be accompanied by enabling **Mmad**.
<!-- npu="950" id28 -->
- When **NZ2DN** is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
<!-- end id28 -->
<!-- npu="A3,910b" id27 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id25 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id25 -->
    <!-- npu="910b" id26 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id26 -->
    For floating-point Inf/NaN input and output, the saturation mode during floating-point quantization move-out can be controlled by setting the **CTRL**\[48\] bit of the **CTRL** register:

    - Non-saturation mode: Set **CTRL**\[48\] to 1'b1, and Inf/NaN are output as-is.
    - Saturation mode: Set **CTRL**\[48\] to 1'b0, Inf output is saturated to ±MAX, and NaN output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id27 -->
<!-- npu="950" id29 -->
- Special value/boundary value constraints for Ascend 950PR/Ascend 950DT:

    For floating-point Inf/NaN input and output, the CTRL[48] bit of the CTRL register (control register) can be used to configure the saturation mode during floating-point quantization move-out.

    For the fp8_e4m3fn_t type, the CTRL[48] bit of the CTRL register (control register) can be used for finer-grained configuration.

    - Non-saturation mode: Set CTRL[48] to 1'b1, and Inf/NaN are output as-is. For the fp8_e4m3fn_t type, if the absolute value of the result is Inf or greater than the maximum normalized value of fp8_e4m3fn_t, the output result is NaN.
    - Saturation mode: Set CTRL[48] to 1'b0, and Inf output is saturated to ±MAX, while NaN output is saturated to 0. For the fp8_e4m3fn_t type, when CTRL[50] is set to 1'b0, NaN is converted to zero; when CTRL[50] is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of fp8_e4m3fn_t, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id29 -->

## Example<a id="zh-cn_topic_0000002542828493_section088124295117"></a>

<!-- npu="950,A3,910b" id30 -->
For the complete Fixpipe sample, see [fixpipe\_l0c2gm sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_cube_compute/fixpipe_l0c2gm).

- Example: path L0C Buffer-\>GM. The input matrices A and B are of the half data type, and the output cube C is of the float data type, with NZ2ND format conversion enabled.

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
