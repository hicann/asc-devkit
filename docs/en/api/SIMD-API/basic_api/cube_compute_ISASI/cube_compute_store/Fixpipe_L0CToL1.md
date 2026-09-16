# Fixpipe (L0C to L1 Data Movement)<a id="ZH-CN_TOPIC_0000002538231198"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T16:01:54.727Z -->

## Applicable Products<a id="zh-cn_topic_0000002511188540_section796754519912"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supports the APIs that contain the **FixpipeParamsV220**/**FixpipeParamsArch3510** parameters.
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Supports only the APIs that contain the **FixpipeParamsV220** parameter.
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Supports only the APIs that contain the **FixpipeParamsV220** parameter.
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
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
## Description<a id="zh-cn_topic_0000002511188540_section106841136114319"></a>

The header file path is `"basic_api/kernel_operator_fixpipe_intf.h, basic_api/kernel_operator_data_copy_intf.h"`.

The result of cube computation is stored in the L0C Buffer. The Fixpipe API is used to move the result to the L1 Buffer, and it supports operations such as inline format conversion during the move.

<!-- npu="950,A3,910b" id10 -->

The following figure shows the valid combinations of inline quantization, inline ReLU, and inline channel merging, as well as the intermediate data types and data paths. In the figure, F32->F16 and F32->BF16 are non-quantization modes that perform only Cast, while the rest are inline scalar/tensor quantization modes.

<!-- npu="A3,910b" id11 -->
**Figure 1** L0C2L1 flowchart ([NPU architecture version 2201](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002511188540_fig8956371257"></a>  

![](../../../../figures/L0C2L1_Function_Combination.png)
<!-- end id11 -->

<!-- npu="950" id13 -->
**Figure 2** L0C2L1 flowchart ([NPU architecture version 3510](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/language_extension/SIMT-BuiltIn_keyword.md))<a id="zh-cn_topic_0000002542828493_fig1828513492547"></a>  

![](../../../../figures/L0C2L1_Function_Combination_950.png)
<!-- end id13 -->
<!-- end id10 -->

## Prototype<a id="zh-cn_topic_0000002511188540_section82039854412"></a>

To facilitate user usage and reduce learning and understanding costs, a combined cube move-out API **Fixpipe** is provided for frequently used scenario combinations. The API incorporates register configuration and data movement capabilities. The corresponding APIs are as follows:

**APIs that take the FixpipeParamsArch3510 parameter:**

- Path L0C Buffer (CO1) -> L1 Buffer (C1), without enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

- Path L0C Buffer (CO1) -> L1 Buffer (C1), with the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<uint64_t>& cbufWorkspace, const FixpipeParamsArch3510<config.format>& intriParams)
    ```

**APIs that take the FixpipeParamsV220 parameter:**

- Path L0C Buffer (CO1) -> L1 Buffer (C1), without enabling the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const FixpipeParamsV220& intriParams)
    ```

- Path L0C Buffer (CO1) -> L1 Buffer (C1), with the inline [tensor quantization](../cube_store_key_features/on_the_fly_quantization.md) feature enabled:

    ```cpp
    template <typename T, typename U, const FixpipeConfig& config = CFG_ROW_MAJOR, typename S = uint64_t, typename Std::enable_if<Std::is_same<PrimT<S>, uint64_t>::value, bool>::type = true>
    __aicore__ inline void Fixpipe(const LocalTensor<T>& dst, const LocalTensor<U>& src, const LocalTensor<S>& cbufWorkspace, const FixpipeParamsV220& intriParams)
    ```

## Parameters<a id="zh-cn_topic_0000002511188540_section16128134420472"></a>

**Table 1** Fixpipe template parameter description

| Parameter | Description |
| ---------- | ---------- |
| T/U | Data type of the destination operand/source operand. For supported data types, see [Data Types](#zh-cn_topic_0000002511188540_section4219135304818). |
| config | Fixpipe-related configuration parameter, of the type **FixpipeConfig**. The values are as follows:<br>&nbsp;&nbsp;&bull; **CFG_ROW_MAJOR (default value)**: Enables NZ2ND, and the output data format is ND. For Atlas A3 training products/Atlas A3 inference products, Atlas A2 training products/Atlas A2 inference products, this value does not take effect on the L0C Buffer -> L1 Buffer path. <br>&nbsp;&nbsp;&bull; CFG_NZ: NZ2NZ, and the output data format is NZ.<br><!-- npu="950" id14 -->&nbsp;&nbsp;&bull; CFG_COLUMN_MAJOR: For Ascend 950PR/Ascend 950DT, enables NZ2DN, and the output data format is DN.<br><!-- end id14 --><br>struct FixpipeConfig {<br>    CO2Layout format;<br>    bool isToUB; // Used to specify whether the destination address is in UB. <br>};<br>enum class CO2Layout : uint8_t {<br>    NZ = 0, // The output data format remains NZ.<br>    ROW_MAJOR, // Enables NZ2ND, and the output data format is ND.<br><!-- npu="950" id15 -->    COLUMN_MAJOR, // Supported only by Ascend 950PR/Ascend 950DT. Enables NZ2DN, and the output data format is DN.<br><!-- end id15 -->};<br>constexpr FixpipeConfig CFG_NZ = {CO2Layout::NZ};<br>constexpr FixpipeConfig CFG_ROW_MAJOR = {CO2Layout::ROW_MAJOR};<!-- npu="950" id16 --><br>constexpr FixpipeConfig CFG_COLUMN_MAJOR = {CO2Layout::COLUMN_MAJOR}; // Supported only by Ascend 950PR/Ascend 950DT. <!-- end id16 --><br>|
| S | Data type of the **cbufWorkspace** parameter, that is, the data type of the inline quantization parameter.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use basic data types, the template parameter **S** must be of the **uint64_t** type; otherwise, compilation fails.<br>&nbsp;&nbsp;&bull; When the destination operand, source operand, and **cbufWorkspace** use the **TensorTrait** type, the LiteType of the template parameter **S** must be of the **uint64_t** type; otherwise, compilation fails.<br>The template parameter following **S** is used only for the preceding data type check, and users do not need to pay attention to it. |

**Table 2** Fixpipe parameter description

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the type **LocalTensor**. The data format is NZ. The NZ address must meet 32-byte alignment. <!-- npu="950" id20 --><br>For Ascend 950PR/Ascend 950DT, the ND and DN data formats are also supported, and the address must meet 32-byte alignment. <!-- end id20 --> |
| src | Input | Source operand, of the type **LocalTensor**. The supported physical address is L0C Buffer (**TPosition** is CO1), which is the result computed by the **Mmad** API. The data format is NZ, and the address must meet 64-byte alignment. |
| intriParams | Input | Fixpipe move parameter. For the specific definition, see "basic_api/API/kernel_struct_fixpipe.h".<br>For the parameter description, see the description of the Fixpipe move parameter (**FixpipeParamsArch3510** and **FixpipeParamsV220**) structures. |
| cbufWorkspace | Input | Quantization parameter, of the type `LocalTensor<uint64_t>`. The supported physical address is L1 Buffer (**TPosition** is C1), and the address must meet 32-byte alignment.<br>&nbsp;&nbsp;&bull; Supported when **quantPre** is VDEQF16, VQF322B8_PRE, or VREQ8.<br><!-- npu="950" id21 -->&nbsp;&nbsp;&bull; For Ascend 950PR/Ascend 950DT, in addition to the preceding values, VQF322FP8_PRE, VQF322HIF8_PRE, VQF322HIF8_PRE_HYBRID, VQS322BF16_PRE, VQF322F16_PRE, VQF322BF16_PRE, and VQF322F32_PRE are also supported.<br><!-- end id21 -->For the introduction to **quantPre**, see the **quantPre** section in the Fixpipe move parameter structure. |

**Table 3** Fixpipe move parameter (FixpipeParamsArch3510) structure description

| Parameter | Data Type | Description |
| --- | --- | --- |
| nSize | Mandatory input | Size of the source NZ cube in the N direction, with a value range of nSize∈[0, 4095].<br>&nbsp;&nbsp;&bull; If the [channelSplit](../cube_store_key_features/F32-Channel-Split.md) feature is enabled, nSize must be a multiple of 8.<br>&nbsp;&nbsp;&bull; If the channelSplit feature is not enabled, nSize must be a multiple of 16.<br>&nbsp;&nbsp;&bull; If NZ2ND is enabled, nSize*sizeof(T) must be a multiple of 32.<br>**Note: nSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| mSize | Mandatory input | Size of the source NZ cube in the M direction, with a value range of mSize∈[0, 65535]. If NZ2DN is enabled, mSize*sizeof(T) must be a multiple of 32.<br>**Note: mSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| srcStride | Mandatory input | Start address offset between adjacent Z layouts in the source NZ cube, with a value range of srcStride∈[0, 65535], in units of C0_Size (16*sizeof(T)), where T is the data type of src. The value should be set to mSize rounded up to a multiple of 16. |
| dstStride | Mandatory input | &bull; When the NZ2ND/NZ2DN feature is not enabled ([NZ2NZ](../cube_store_key_features/NZ2NZ.md)): Start address offset between adjacent Z layouts in the destination NZ cube. The value must not be 0, in units of element. (Compared with dstStride in the Fixpipe move parameter (FixpipeParamsV220), the unit differs: in FixpipeParamsV220, dstStride is in units of datablock (32 bytes), whereas in FixpipeParamsArch3510, dstStride is in units of element.)<br>&bull; When the [NZ2ND](../cube_store_key_features/NZ2ND.md)/[NZ2DN](../cube_store_key_features/NZ2DN.md) feature is enabled: Number of elements in each row of the destination ND/DN cube. The value must not be 0, in units of element. |
| [quantPre](../cube_store_key_features/on_the_fly_quantization.md) | Optional input | Controls the quantization mode. The type is QuantMode_t, and the default value is `QuantMode_t::NoQuant`. The specific definition is as follows:<br>Note: This parameter must be configured manually by the user; the corresponding quantization mode is not automatically derived.<br>enum QuantMode_t<br>{<br>    NoQuant,               // Disable quantization.<br>    F322F16,               // Float32_2_Float16: Cast float to half, with CAST_RINT cast mode.<br>    F322BF16,              // Float32_2_BFloat16: Cast float to bfloat16_t, with CAST_RINT cast mode.<br>    DEQF16,                // DeQuant_Float16: Quantize int32_t to half, scalar quantization.<br>    VDEQF16,               // Vector_DeQuant_Float16: Quantize int32_t to half, tensor quantization.<br>    QF322B8_PRE,           // Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE,          // Vector_Quant_Float32_2_B8: Quantize float to int8_t/uint8_t, tensor quantization.<br>    REQ8,                  // ReQuant_int8: Quantize int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,                 // Vector_ReQuant_int8: Quantize int32_t to int8_t/uint8_t, tensor quantization.<br>    QF322FP8_PRE,          // Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, scalar quantization.<br>    VQF322FP8_PRE,         // Vector_Quant_Float32_2_FP8: Quantize float to fp8_e4m3fn_t, tensor quantization.<br>    QF322HIF8_PRE,         // Quant_Float32_2_HIF8: Quantize float to hifloat8_t (Half to Away Round), scalar quantization.<br>    VQF322HIF8_PRE,        // Vector_Quant_Float32_2_HIF8: Quantize float to hifloat8_t (Half to Away Round), tensor quantization.<br>    QF322HIF8_PRE_HYBRID,  // Quant_Float32_2_HIF8_Hybrid: Quantize float to hifloat8_t (Hybrid Round), scalar quantization.<br>    VQF322HIF8_PRE_HYBRID, // Vector_Quant_Float32_2_HIF8_Hybrid: Quantize float to hifloat8_t (Hybrid Round), tensor quantization.<br>    QS322BF16_PRE,         // Quant_Int32_2_BFloat16: Quantize int32_t to bfloat16_t, scalar quantization.<br>    VQS322BF16_PRE,        // Vector_Quant_Int32_2_BFloat16: Quantize int32_t to bfloat16_t, tensor quantization.<br>    QF322F16_PRE,          // Quant_Float32_2_Float16: Quantize float to half, scalar quantization.<br>    VQF322F16_PRE,         // Vector_Quant_Float32_2_Float16: Quantize float to half, tensor quantization.<br>    QF322BF16_PRE,         // Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, scalar quantization.<br>    VQF322BF16_PRE,        // Vector_Quant_Float32_2_BFloat16: Quantize float to bfloat16_t, tensor quantization.<br>    QF322F32_PRE,          // Quant_Float32_2_Float32: Quantize float to float, scalar quantization. The precision can reach two thousandths but not two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>    VQF322F32_PRE,         // Vector_Quant_Float32_2_Float32: Quantize float to float, tensor quantization. The precision can reach two thousandths but not two ten-thousandths. If two ten-thousandths precision is required, use the [AscendDeQuant](../../../advanced_api/quantization_operations/AscendDequant.md) high-level API.<br>};<br> |
| deqScalar | Optional input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the quantPre quantization mode is [inline quantization](../cube_store_key_features/on_the_fly_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/on_the_fly_relu.md) | Optional input | Whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is not enabled. The default value is `false`.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | Optional input | unitFlag is a fine-grained parallelism between Mmad instructions and Fixpipe instructions. When this feature is enabled, the computation result is moved out as soon as the hardware finishes computing each fractal. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): unitFlag is not enabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): unitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is not reset.<br>&nbsp;&nbsp;&bull; 3 (2'b11): unitFlag is enabled. After the hardware finishes executing the instruction, the unit flag bit is reset.<br>When this feature is enabled, the unitFlag value of both the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For the parameter configuration scheme and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| params | Optional input | Selects and configures different inline format conversions (NZ2NZ/NZ2ND/NZ2DN). This parameter is a structure of the TransformParams type. The TransformParams structure is a type selector based on template parameters, used to automatically select the corresponding parameter type at compile time according to the template parameter used when defining the FixpipeParamsArch3510 move parameter.<br>template \<CO2Layout format\><br>struct TransformParams {};<br>template \<\><br>struct TransformParams\<CO2Layout::NZ\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = uint8_t;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::ROW_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2NdParams;<br>};<br>template \<\><br>struct TransformParams\<CO2Layout::COLUMN_MAJOR\> {<br>    \_\_aicore\_\_ inline TransformParams(){};<br>    using PARAMS = Nz2DnParams;<br>};<br>When the CO2Layout layout type is ROW_MAJOR, the instruction is defined as a data move from L0C Buffer to the destination location with NZ2ND conversion. The structure parameters are as follows:<br>struct Nz2NdParams {<br>    uint16_t ndNum = 1;<br>    uint16_t srcNdStride = 0;<br>    uint32_t dstNdStride = 0; <br>};<br>&nbsp;&nbsp;&bull; ndNum: Number of source NZ matrices, that is, the number of ND matrices to be transferred, with a value range of ndNum∈[0, 65535]. **Note: ndNum=0 means no move is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNdStride: Interval between the start addresses of different NZ matrices, with a value range of srcNdStride∈[0, 65535], in units of C0_SIZE. When ndNum is set to 1, srcNdStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstNdStride: Offset between the start addresses of adjacent destination ND matrices, with a value range of dstNdStride∈[1, 2^32 -1], in units of element. When ndNum is set to 1, dstNdStride can be set to 0 and does not take effect.<br><br>When the CO2Layout layout type is COLUMN_MAJOR, the instruction is defined as a data move from L0C Buffer to the destination location with NZ2DN conversion. The structure parameters are as follows:<br>struct Nz2DnParams {<br>    uint16_t dnNum = 1;<br>    uint16_t srcNzCubeStride = 0;<br>    uint32_t dstDnCubeStride = 0;<br>    uint16_t srcNzC0Stride = 0; <br>};<br>&nbsp;&nbsp;&bull; dnNum: Number of DN matrices to be transferred, with a value range of dnNum∈[0, 65535]. **Note: dnNum=0 means no move is performed, and the API is treated as a NOP (no operation).**<br>&nbsp;&nbsp;&bull; srcNzCubeStride: Offset between different source NZ matrices (head-to-head), with a value range of srcNzCubeStride∈[0, 65535], in units of C0_SIZE. When dnNum is set to 1, srcNzCubeStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; dstDnCubeStride: Offset between the start addresses of adjacent destination ND matrices, with a value range of dstDnCubeStride∈[0, 2^32 -1], in units of element. When dnNum is set to 1, dstDnCubeStride can be set to 0 and does not take effect.<br>&nbsp;&nbsp;&bull; srcNzC0Stride: Address offset between adjacent rows in the source NZ cube fractal (head-to-head), with a value range of srcNzC0Stride∈[0, 65535], in units of C0_SIZE. When NZ2DN is enabled, srcNzC0Stride must not be 0.<br><br>When the CO2Layout layout type is NZ, it is the normal DMA move mode, representing a normal data move from L0C Buffer to the destination location. |
| dualDstCtrl | Optional input | This parameter is valid only on the L0C Buffer-to-Unified Buffer path. |
| subBlockId | Optional input | This parameter is valid only on the L0C Buffer-to-Unified Buffer path. |
| [isChannelSplit](../cube_store_key_features/F32-Channel-Split.md) | Optional input | This parameter is valid only on the L0C Buffer-to-Global Memory path. |

**Table 4** Fixpipe move parameter (FixpipeParamsV220) structure description

| parameter name | data type | description |
| --- | --- | --- |
| nSize | mandatory input | Size of the source NZ cube in the N direction. Value range: **nSize**∈[0, 4095], and **nSize** must be a multiple of 16.<br>**Note: nSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| mSize | mandatory input | Size of the source NZ cube in the M direction. Value range: **mSize**∈[0, 65535].<br>**Note: mSize=0 means no move is performed, and the API is treated as a NOP (no operation).** |
| srcStride | mandatory input | Start address offset between adjacent Z layouts in the source NZ cube. Value range: **srcStride**∈[0, 65535], in units of C0_Size (16*sizeof(T)), where T is the data type of src. The value should be set to mSize rounded up to a multiple of 16. |
| dstStride | mandatory input | Start address offset between adjacent Z layouts in the destination NZ cube. The value must not be 0, in units of datablock (32 bytes). |
| [quantPre](../cube_store_key_features/on_the_fly_quantization.md) | optional input | Controls the quantization mode. It is of the **QuantMode_t** type, with a default value of `QuantMode_t::NoQuant`. The specific definition is as follows:<br>&nbsp;&nbsp;&bull; For half/bfloat16_t output, this parameter must be configured as `QuantMode_t::F322F16`/`QuantMode_t::F322BF16`.<br>Note: This parameter must be configured manually by the user and is not automatically derived. This path does not support the NoQuant mode.<br>enum QuantMode_t<br>{<br>    NoQuant,      // Does not enable quantization. This path does not support it.<br>    F322F16,      // Float32_2_Float16: Casts float to half, with the cast mode set to CAST_RINT.<br>    F322BF16,     // Float32_2_BFloat16: Casts float to bfloat16_t, with the cast mode set to CAST_RINT.<br>    DEQF16,       // DeQuant_Float16: Quantizes int32_t to half, scalar quantization.<br>    VDEQF16,      // Vector_DeQuant_Float16: Quantizes int32_t to half, tensor quantization.<br>    QF322B8_PRE,  // Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, scalar quantization.<br>    VQF322B8_PRE, // Vector_Quant_Float32_2_B8: Quantizes float to int8_t/uint8_t, tensor quantization.<br>    REQ8,         // ReQuant_int8: Quantizes int32_t to int8_t/uint8_t, scalar quantization.<br>    VREQ8,        // Vector_ReQuant_int8: Quantizes int32_t to int8_t/uint8_t, tensor quantization.<br>};<br>|
| deqScalar | optional input | Scalar quantization parameter, representing a single scale value. This parameter must be set when the **quantPre** quantization mode is [inline quantization](../cube_store_key_features/on_the_fly_quantization.md). The supported data type is `uint64_t`. |
| [reluEn](../cube_store_key_features/on_the_fly_relu.md) | optional input | Whether to enable NormReLU:<br>&nbsp;&nbsp;&bull; `false`: NormReLU is disabled, which is the default value.<br>&nbsp;&nbsp;&bull; `true`: NormReLU is enabled. |
| unitFlag | optional input | **unitFlag** enables fine-grained parallelism between Mmad and Fixpipe instructions. When this feature is enabled, the hardware moves out the computation result each time a fractal is computed. The values are described as follows:<br>&nbsp;&nbsp;&bull; 0 (2'b00): **unitFlag** is disabled.<br>&nbsp;&nbsp;&bull; 2 (2'b10): **unitFlag** is enabled, and the hardware does not reset the unit flag bit after executing the instruction.<br>&nbsp;&nbsp;&bull; 3 (2'b11): **unitFlag** is enabled, and the hardware resets the unit flag bit after executing the instruction.<br>When this feature is enabled, the **unitFlag** value of both the Mmad instruction and the Fixpipe instruction must be set to 2 or 3.<br>For parameter configuration and feature details, see the [UnitFlag](../mmad_compute_key_features/UnitFlag.md) section in the key feature description of Mmad computation. |
| [channelSplit](../cube_store_key_features/F32-Channel-Split.md) | optional input | Takes effect only on the L0C Buffer (CO1) -> GM path. Set this parameter to false on this path. |
| ndNum | optional input | Takes effect only on the L0C Buffer (CO1) -> GM path. No additional configuration is required on this path. |
| srcNdStride | optional input | Takes effect only on the L0C Buffer (CO1) -> GM path. No additional configuration is required on this path. |
| dstNdStride | optional input | Takes effect only on the L0C Buffer (CO1) -> GM path. No additional configuration is required on this path. |

## Data Type<a id="zh-cn_topic_0000002511188540_section4219135304818"></a>

**Supported data type combinations for the source cube and destination cube**

<!-- npu="950" id22 -->
For Ascend 950PR/Ascend 950DT, the supported data type combinations are as follows:

| source cube (L0C Buffer) | destination cube (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, hifloat8_t, fp8_e4m3fn_t, half, bfloat16_t, float. |
| int32_t | int8_t, uint8_t, half, bfloat16_t, int32_t. |
<!-- end id22 -->

<!-- npu="A3" id23 -->
For Atlas A3 training products/Atlas A3 inference products, the supported data type combinations are as follows:

| source cube (L0C Buffer) | destination cube (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |
<!-- end id23 -->

<!-- npu="910b" id24 -->
For Atlas A2 training products/Atlas A2 inference products, the supported data type combinations are as follows:

| source cube (L0C Buffer) | destination cube (L1 Buffer) |
| ---------- | ---------- |
| float | int8_t, uint8_t, half, bfloat16_t. |
| int32_t | int8_t, uint8_t, half. |
<!-- end id24 -->

## Return Value

None

## Constraints<a id="zh-cn_topic_0000002511188540_section2045914466492"></a>

- The following describes the case where the quantization input is of the **float32** data type:
    - The standard IEEE 754 **float32** format consists of a 1-bit sign, an 8-bit exponent, and a 23-bit mantissa. The **float32** format supported by the current AI processor consists of a 1-bit sign, an 8-bit exponent, and a 10-bit mantissa.
    - If the user provides standard IEEE 754 **float32** input, the API internally converts it to the **float32** format supported by the processor for computation. In this case, if standard IEEE 754 **float32** data is used during golden data generation, a precision mismatch may be introduced. The golden data generation must be corrected by clearing the lower 13 bits of the 23-bit mantissa of the quantization parameter before it participates in quantization computation.
- The source cube in NZ format requires 64-byte address alignment, and the destination cube in NZ format requires 32-byte address alignment.
- When any of the moved **mSize**, **nSize**, or **ndNum** is 0, the instruction is not executed. When **ndNum** is 0, a warning is reported.
- The quantization and **ReLU** parameters cannot be Inf/NaN or denormalized numbers.
- The destination data must not overlap. If overlapping writes are performed on the destination address, the hardware does not report any warning or error, and the write order of the overlapping data is not guaranteed.
- Enabling the **unitFlag** feature must be accompanied by enabling **Mmad**.
<!-- npu="950" id27 -->
- When NZ2DN is enabled and **srcNzC0Stride** is not equal to 1, **unitFlag** cannot be enabled at the same time.
<!-- end id27 -->
<!-- npu="A3,910b" id28 -->
- For the following product models, the special value/boundary value constraints are described as follows:
    <!-- npu="A3" id29 -->
    Atlas A3 training products/Atlas A3 inference products
    <!-- end id29 -->
    <!-- npu="910b" id30 -->
    Atlas A2 training products/Atlas A2 inference products
    <!-- end id30 -->
    For floating-point Inf/NaN input and output, the CTRL\[48\] bit of the CTRL register (control register) can be used to set the saturation mode for floating-point quantization move-out:

    - Non-saturation mode: Set CTRL\[48\] to 1'b1, and Inf/NaN are output as-is.
    - Saturation mode: Set CTRL\[48\] to 1'b0, and Inf output is saturated to ±MAX, while NaN output is saturated to 0.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id28 -->
<!-- npu="950" id31 -->
- Special value/boundary value constraints for Ascend 950PR/Ascend 950DT:

    For floating-point Inf/NaN input and output, the CTRL\[48\] bit of the CTRL register (control register) can be used to set the saturation mode for floating-point quantization move-out;

    For the **fp8_e4m3fn_t** type, finer-grained settings can be made through the CTRL\[48\] bit of the CTRL register (control register).

    - Non-saturation mode: Set CTRL\[48\] to 1'b1, and Inf/NaN are output as-is. For the **fp8_e4m3fn_t** type, if the absolute value of the result is Inf or greater than the maximum normalized value of **fp8_e4m3fn_t**, the output result is NaN.
    - Saturation mode: Set CTRL\[48\] to 1'b0, and Inf output is saturated to ±MAX, while NaN output is saturated to 0. For the **fp8_e4m3fn_t** type, when CTRL\[50\] is set to 1'b0, NaN is converted to zero; when CTRL\[50\] is set to 1'b1, NaN remains unchanged. If the absolute value of the result is Inf or greater than the maximum normalized value of **fp8_e4m3fn_t**, the result is the maximum normalized value.

        ```cpp
        // Set CTRL[48] to 0 to enable the floating-point saturation mode.
        AscendC::AscendCUtils::SetOverflow(0);
        ```

    For integer types, only the saturation mode is available.
<!-- end id31 -->

## Example<a id="zh-cn_topic_0000002511188540_section088124295117"></a>

<!-- npu="950,A3,910b" id32 -->
For the complete Fixpipe sample, see [fixpipe\_l0c2l1 sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/03_basic_api/03_matrix_compute/fixpipe_l0c2l1).

- Example: The path is L0C Buffer -> L1 Buffer. The input matrices A and B are of the **half** data type, and the output cube C is of the **int8_t** data type. NZ2ND format conversion is disabled, and scalar quantization is enabled.

    ```cpp
        AscendC::LocalTensor<outputType> c1Local(AscendC::TPosition::C1, c1Addr, cSizeAlignL1);
    
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
        uint16_t c0Size = 32;
        AscendC::FixpipeParamsV220 fixpipeParams;
        // In the NZ2NZ scenario, the unit of **dstStride** in **FixpipeParamsV220** is a data block (32 bytes).
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
        // In the NZ2NZ scenario, the unit of **dstStride** in **FixpipeParamsArch3510** is the number of elements.
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
